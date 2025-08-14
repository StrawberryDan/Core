#pragma once
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Thread/Worker.hpp"
#include <functional>
#include <thread>
#include <vector>
#include <latch>
#include <future>

#include "Strawberry/Core/Math/Math.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"

namespace Strawberry::Core
{
	class ThreadPool
	{
	public:
		ThreadPool(int threadCount = std::thread::hardware_concurrency());
		ThreadPool(float percentageOfThreads);


		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;


		~ThreadPool();


		void Join();


		using Job = std::function<void()>;


		template <typename F, typename T = std::invoke_result_t<F>>
		QueueResult<PendingTask<T>> QueueTask(F&& task)
		{
			ZoneScoped;

			return mWorkers[GetNextThreadIndex()].Queue(std::forward<F>(task));
		}


		template <std::ranges::viewable_range Range, typename Input = std::ranges::range_value_t<Range>, typename Result = std::invoke_result_t<Input>>
		QueueResult<PendingTaskList<Result>> QueueTasks(Range&& tasks)
		{
			ZoneScoped;

			unsigned int tasksEach = Math::CeilDiv(tasks.size(), mThreadCount);

			std::vector<std::future<Result>> futures;
			if constexpr (std::ranges::sized_range<Range>)
			{
				futures.reserve(tasks.size());
			}


			for (auto iter = tasks.begin(); iter != tasks.end(); std::advance(iter, tasksEach))
			{
				auto threadIndex = GetNextThreadIndex();
				auto batch = std::ranges::subrange(iter, tasks.end()) | std::views::take(tasksEach);
				auto batchFutures = mWorkers[threadIndex].Queue(std::move(batch));
				if (!batchFutures)
				{
					return batchFutures;
				}

				for (auto&& future : batchFutures.Unwrap())
				{
					futures.emplace_back(std::move(future));
				}
			}


			return futures;
		}


		template <std::unsigned_integral T, size_t D, typename F, typename R = std::invoke_result_t<F, Math::Vector<T, D>>>
		[[nodiscard]] QueueResult<std::vector<std::pair<Math::Vector<T, D>, std::future<R>>>> QueueTasks(Math::Vector<T, D> input, F&& function)
		{
			ZoneScoped;

			const size_t inputCount = input.Fold(std::multiplies());

			std::vector<std::pair<Math::Vector<T, D>, std::future<R>>> futures;
			futures.reserve(inputCount);

			std::vector<Math::Vector<T, D>> inputs = input.Rectangle();

			auto tasks = inputs | std::views::transform(
				[=] (auto&& x)
				{
					return std::bind(function, x);
				});

			auto queueResult = QueueTasks(std::move(tasks));
			if (!queueResult)
			{
				return queueResult.Err();
			}

			auto packaged = std::views::zip(inputs, queueResult.Unwrap());
			for (auto&& [x, future] : packaged)
			{
				futures.emplace_back(std::move(x), std::move(future));
			}

			return futures;
		}

	private:
		unsigned int GetNextThreadIndex()
		{
			return mNextQueueIndex.fetch_add(1, std::memory_order_relaxed) % mThreadCount;
		}


		const size_t mThreadCount;
		std::atomic<unsigned int> mNextQueueIndex = 0;
		std::unique_ptr<Worker[]> mWorkers;
	};
}
