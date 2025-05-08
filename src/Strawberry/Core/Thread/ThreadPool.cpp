#include "ThreadPool.hpp"


namespace Strawberry::Core
{
	ThreadPool::ThreadPool(const unsigned int threadCount)
	{
		for (int i = 0; i < threadCount; i++)
		{
			mWorkers.emplace_back(std::make_unique<WorkerThread>(&mRunning));
		}
	}


	ThreadPool::~ThreadPool()
	{
		Join();
	}


	void ThreadPool::Join()
	{
		mRunning = false;
	}


	ThreadPool::WorkerThread::WorkerThread(RunningFlag* runningFlag)
	{
		this->runningFlag = runningFlag;
		thread = std::thread([this] { Run(); });
	}


	ThreadPool::WorkerThread::~WorkerThread()
	{
		Join();
	}

	void ThreadPool::WorkerThread::Queue(Job&& job)
	{
		jobQueue.Lock()->emplace_back(std::move(job));
	}

	void ThreadPool::WorkerThread::Join()
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}

	void ThreadPool::WorkerThread::Run()
	{
		while (true)
		{
			bool wasEmpty = false;

			{
				Optional<Job> job = [&]()-> Optional<Job>
				{
					auto jobQueueLock = jobQueue.Lock();
					if (!jobQueueLock->empty())
					{
						auto job = std::move(jobQueueLock->front());
						jobQueueLock->pop_front();
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
				if (!*runningFlag)
				{
					return;
				}

				std::this_thread::yield();
			}
		}
	}
}
