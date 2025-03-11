#pragma once
#include "Strawberry/Core/Math/Vector.hpp"
#include <deque>
#include <functional>
#include <thread>
#include <vector>

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
							auto jobQueue = queue->Lock();
							if (!jobQueue->empty())
							{
								auto job = std::move(jobQueue->front());
								jobQueue->pop_front();
								std::invoke(job);
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
			auto jobLock = mJobQueues[nextQueue++]->Lock();
			if (nextQueue == mThreads.size() - 1)
			{
				nextQueue = 0;
			}

			jobLock->emplace_back(std::move(job));
		}

		void QueueJobs(std::vector<Job>&& jobs)
		{
			for (auto&& job : jobs)
			{
				auto jobLock = mJobQueues[nextQueue++]->Lock();
				if (nextQueue == mThreads.size() - 1)
				{
					nextQueue = 0;
				}

				jobLock->emplace_back(std::move(job));
			}
		}


		template <std::unsigned_integral T, size_t D, typename F>
		void QueueJobs(Math::Vector<T, D> input, F&& function)
		{
			auto incrementVector = [input](this const auto& self, Math::Vector<T, D>& v, size_t place = 0) -> void
			{
				if (v[place] == input[place] - 1)
				{
					v[place] = 0;
					self(v, place + 1);
				}
				else
				{
					++v[place];
				}
			};

			auto finished = [input](Math::Vector<T, D> v)
			{
				for (int i = 0; i < D; ++i)
				{
					if (v[i] != input[i] - 1)
					{
						return false;
					}
				}

				return true;
			};


			std::vector<Math::Vector<T, D>> inputs;
			Math::Vector<T, D> accumulator;

			while (!finished(accumulator))
			{
				inputs.emplace_back(accumulator);
				incrementVector(accumulator);
			}


			for (auto&& x : inputs)
			{
				auto jobLock = mJobQueues[nextQueue++]->Lock();
				if (nextQueue == mThreads.size())
				{
					nextQueue = 0;
				}

				jobLock->emplace_back(std::move([x, function](){ function(x); }));
			}
		}

	private:
		using JobQueue = std::unique_ptr<Mutex<std::deque<Job>>>;


		std::vector<std::thread> mThreads;
		std::unique_ptr<std::atomic<bool>> mRunning = std::make_unique<std::atomic<bool>>(true);

		unsigned int nextQueue = 0;
		std::vector<JobQueue> mJobQueues;
	};
}
