#pragma once
#include "Strawberry/Core/Math/Vector.hpp"
#include <deque>
#include <functional>
#include <thread>
#include <vector>
#include <latch>

#include "Strawberry/Core/Sync/Mutex.hpp"

namespace Strawberry::Core
{
	class ThreadPool
	{
	public:
		ThreadPool(const unsigned int threadCount = std::thread::hardware_concurrency())
		{
			for (int i = 0; i < threadCount; i++)
			{
				mJobQueues.emplace_back(std::make_unique<Mutex<std::deque<Job>>>());
				mThreads.emplace_back([queue = mJobQueues[i].get(), running = mRunning.get()]()
				{
					while (true)
					{
						bool wasEmpty = false;

						{
							Optional<Job> job = [queue]()-> Optional<Job>
							{
								auto jobQueue = queue->Lock();
								if (!jobQueue->empty())
								{
									auto job = std::move(jobQueue->front());
									jobQueue->pop_front();
									return Optional(job);
								}

								return NullOpt;
							}();

							if (job)
							{
								std::invoke(job.Unwrap());
							}
							else
							{
								wasEmpty = true;
							}
						}

						if (wasEmpty)
						{
							if (!*running)
							{
								return;
							}

							std::this_thread::yield();
						}
					}
				});
			}
		}


		~ThreadPool()
		{
			Join();
		}


		void Join()
		{
			*mRunning = false;
			for (auto&& thread : mThreads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
		}


		using Job = std::function<void()>;


		void QueueJob(Job&& job)
		{
			auto jobLock = mJobQueues[GetNextThreadIndex()]->Lock();

			jobLock->emplace_back(std::move(job));
		}

		void QueueJobs(std::vector<Job>&& jobs)
		{
			for (auto&& job : jobs)
			{
				auto jobLock = mJobQueues[GetNextThreadIndex()]->Lock();

				jobLock->emplace_back(std::move(job));
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
					auto jobLock = mJobQueues[GetNextThreadIndex()]->Lock();

					jobLock->emplace_back(
						std::move(
							[x, function, &latch]()
							{
								function(x);
								latch.count_down();
							}));
				}

				latch.wait();
			});

			return future;
		}

	private:
		unsigned int GetNextThreadIndex()
		{
			auto result = mNextQueueIndex;
			mNextQueueIndex = ++mNextQueueIndex % mThreads.size();
			return result;
		}


		using JobQueue = std::unique_ptr<Mutex<std::deque<Job>>>;


		std::vector<std::thread> mThreads;
		std::unique_ptr<std::atomic<bool>> mRunning = std::make_unique<std::atomic<bool>>(true);

		unsigned int mNextQueueIndex = 0;
		std::vector<JobQueue> mJobQueues;
	};
}
