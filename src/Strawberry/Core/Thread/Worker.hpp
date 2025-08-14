#pragma once
// Strawberry Core
#include "Strawberry/Core/Sync/Mutex.hpp"
// Standard Library
#include <atomic>
#include <deque>
#include <functional>
#include <future>
#include <thread>

#include "Strawberry/Core/Types/Result.hpp"
#include "Strawberry/Core/Types/Variant.hpp"


namespace Strawberry::Core
{
	template<typename T = void>
	using Task = std::function<T()>;

	template <typename T>
	using PendingTask = std::future<T>;

	template <typename T>
	using PendingTaskList = std::vector<PendingTask<T>>;

	class QueueError
	{
	public:
		struct Interupted {};

		template <typename T>
		QueueError(T&& x) : mError(std::forward<T>(x)) {}

	private:
		Core::Variant<Interupted> mError;
	};

	template <typename R>
	using QueueResult = Result<R, QueueError>;


	class Worker
	{
	public:
		Worker();
		Worker(const Worker&) = delete;
		Worker(Worker&&) = delete;
		Worker& operator=(const Worker&) = delete;
		Worker& operator=(Worker&&) = delete;
		~Worker();


		template <typename F, typename T = std::invoke_result_t<F>>
		QueueResult<PendingTask<T>> Queue(F&& task)
		{
			ZoneScoped;

			auto [future, packagedTask] = PackageTask(std::forward<F>(task));

			std::unique_lock lock(mTaskQueueMutex);
			mTaskQueue.emplace_back(std::move(packagedTask));
			mTaskQueueCV.notify_one();
			lock.unlock();

			return std::move(future);
		}


		template <std::ranges::viewable_range Range, typename T = std::invoke_result_t<std::ranges::range_value_t<Range>>>
		QueueResult<PendingTaskList<T>> Queue(Range&& tasks) {
			ZoneScoped;

			std::vector<std::future<T>> futures;
			if constexpr (std::ranges::sized_range<Range>)
			{
				auto size = tasks.size();
				futures.reserve(size);
			}


			std::unique_lock lock(mTaskQueueMutex, std::defer_lock);
			while (true)
			{
				if (lock.try_lock())
				{
					for (Task<T>&& task : tasks)
					{
						auto [future, packagedTask] = PackageTask(std::move(task));
						mTaskQueue.emplace_back(std::move(packagedTask));
						futures.emplace_back(std::move(future));
					}
					mTaskQueueCV.notify_one();
					lock.unlock();
					break;
				}

				if (!mRunningFlag)
				{
					return QueueError::Interupted{};
				}
			}

			return futures;
		}


		void Join();
		void Run();


	private:
		using PackagedTask = std::packaged_task<void()>;
		using TaskQueue = std::deque<PackagedTask>;


		template <typename F, typename T = std::invoke_result_t<F>>
		static std::pair<std::future<T>, PackagedTask> PackageTask(F&& task)
		{	ZoneScoped;
			std::promise<T> promise;
			auto future = promise.get_future();

			PackagedTask packagedTask([promise = std::move(promise), task = std::forward<F>(task)] mutable
			{
				if constexpr (std::is_void_v<T>)
				{
					std::invoke(task);
					promise.set_value();
				}
				else
				{
					promise.set_value(std::invoke(task));
				}
			});

			return std::make_pair(std::move(future), std::move(packagedTask));
		}


		std::atomic_bool mRunningFlag;
		std::thread mThread;

		std::mutex mTaskQueueMutex;
		std::condition_variable mTaskQueueCV;
		TaskQueue mTaskQueue;
	};
}
