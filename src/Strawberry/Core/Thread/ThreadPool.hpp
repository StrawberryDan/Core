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
		ThreadPool(unsigned int threadCount = std::thread::hardware_concurrency());


		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;


		~ThreadPool();


		void Join();


		using Job = std::function<void()>;


		template <typename T>
		std::future<T> QueueTask(Task<T>&& task)
		{
			mWorkers[GetNextThreadIndex()].Queue(std::move(task));
		}


		template <std::ranges::viewable_range Range, typename T = std::invoke_result_t<std::ranges::range_value_t<Range>>>
		std::vector<std::future<T>> QueueTasks(Range&& tasks)
		{
			unsigned int tasksEach = Math::CeilDiv(tasks.size(), mThreadCount);
			auto batches = tasks
				| std::views::chunk(tasksEach);

			std::vector<std::future<T>> futures;
			if constexpr (std::ranges::sized_range<Range>)
			{
				futures.reserve(tasks.size());
			}


			for (auto&& batch : batches)
			{
				auto threadIndex = GetNextThreadIndex();
				std::vector<std::future<T>> batchFutures = mWorkers[threadIndex].Queue(batch);
				for (auto&& future : batchFutures)
				{
					futures.emplace_back(std::move(future));
				}
			}


			return futures;
		}


		template <std::unsigned_integral T, size_t D, typename F, typename R = std::invoke_result_t<F, Math::Vector<T, D>>>
		[[nodiscard]] std::vector<std::pair<Math::Vector<T, D>, std::future<R>>> QueueTasks(Math::Vector<T, D> input, F&& function)
		{
			const size_t inputCount = input.Fold(std::multiplies());

			std::vector<std::pair<Math::Vector<T, D>, std::future<R>>> futures;
			futures.reserve(inputCount);

			std::vector<Math::Vector<T, D>> inputs = input.Rectangle();

			auto tasks = inputs | std::views::transform(
				[=] (auto&& x)
				{
					return std::bind(function, x);
				});

			auto packaged = std::views::zip(inputs, QueueTasks(std::move(tasks)));
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
