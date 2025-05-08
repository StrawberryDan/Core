#pragma once
// Strawberry Core
#include "Strawberry/Core/Sync/Mutex.hpp"
// Standard Library
#include <atomic>
#include <deque>
#include <functional>
#include <future>
#include <thread>



namespace Strawberry::Core
{
	template<typename T = void>
	using Task = std::function<T()>;


	class Worker
	{
	public:
		Worker();
		Worker(const Worker&) = delete;
		Worker(Worker&&) = delete;
		Worker& operator=(const Worker&) = delete;
		Worker& operator=(Worker&&) = delete;


		~Worker();


		template <typename T>
		std::future<T> Queue(Task<T>&& task)
		{
			auto [future, packagedTask] = PackageTask(std::move(task));
			mJobQueue.Lock()->emplace_back(std::move(packagedTask));
			return future;
		}


		template <std::ranges::viewable_range Range, typename T = std::invoke_result_t<std::ranges::range_value_t<Range>>>
		std::vector<std::future<T>> Queue(Range&& tasks)
		{
			std::vector<std::future<T>> futures;
			if constexpr (std::ranges::sized_range<Range>)
			{
				futures.reserve(tasks.size());
			}

			auto queueLock = mJobQueue.Lock();
			for (Task<T>&& task : tasks)
			{
				auto [future, packagedTask] = PackageTask(std::move(task));
				queueLock->emplace_back(std::move(packagedTask));
				futures.emplace_back(std::move(future));
			}

			return futures;
		}


		void Join();
		void Run();


	private:
		using PackagedTask = std::move_only_function<void()>;
		using TaskQueue = Mutex<std::deque<PackagedTask>>;


		template <typename T>
		static std::pair<std::future<T>, PackagedTask> PackageTask(Task<T>&& task)
		{
			std::promise<T> promise;
			auto future = promise.get_future();

			PackagedTask packagedTask = [promise = std::move(promise), task = std::move(task)] mutable
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
			};

			return std::make_pair(std::move(future), std::move(packagedTask));
		}


		std::atomic_bool mRunningFlag;
		std::thread mThread;
		TaskQueue mJobQueue;
	};
}
