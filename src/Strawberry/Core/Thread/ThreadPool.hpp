#pragma once
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Thread/Worker.hpp"
#include <functional>
#include <thread>
#include <vector>
#include <latch>
#include <future>

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


		void QueueJob(Job&& job)
		{
			mWorkers[GetNextThreadIndex()].Queue(std::move(job));
		}


		void QueueJobs(std::vector<Job>&& jobs)
		{
			for (auto&& job : jobs)
			{
				mWorkers[GetNextThreadIndex()].Queue(std::move(job));
			}
		}


		template <std::unsigned_integral T, size_t D, typename F>
		[[nodiscard]] std::future<void> QueueJobs(Math::Vector<T, D> input, F&& function)
		{
			auto future = std::async(std::launch::async, [this, input, function = std::forward<F>(function)](){
				const size_t inputCount = input.Fold(std::multiplies());

				std::vector<Math::Vector<T, D>> inputs = input.Rectangle();

				std::latch latch(inputCount);
				for (auto&& x : inputs)
				{
					mWorkers[GetNextThreadIndex()].Queue(
						[x, function, &latch]()
						{
							function(x);
							latch.count_down();
						}
					);
				}

				latch.wait();
			});

			return future;
		}

	private:
		unsigned int GetNextThreadIndex()
		{
			auto result = mNextQueueIndex;
			mNextQueueIndex = ++mNextQueueIndex % threadCount;
			return result;
		}


		const size_t threadCount;
		unsigned int mNextQueueIndex = 0;
		std::unique_ptr<Worker[]> mWorkers;
	};
}
