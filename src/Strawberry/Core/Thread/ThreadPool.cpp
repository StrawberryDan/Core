#include "ThreadPool.hpp"


namespace Strawberry::Core
{
	ThreadPool::ThreadPool(const unsigned int threadCount)
		: threadCount(threadCount)
		, mWorkers(std::make_unique<WorkerThread[]>(threadCount))
	{}


	ThreadPool::~ThreadPool()
	{
		Join();
	}


	void ThreadPool::Join()
	{
		for (int i = 0; i < threadCount; i++)
		{
			mWorkers[i].Join();
		}
	}


	ThreadPool::WorkerThread::WorkerThread()
		: mRunningFlag(true)
		, mThread([this] { Run(); })
	{}


	ThreadPool::WorkerThread::~WorkerThread()
	{
		Join();
	}

	void ThreadPool::WorkerThread::Queue(Job&& job)
	{
		mJobQueue.Lock()->emplace_back(std::move(job));
	}


	void ThreadPool::WorkerThread::Join()
	{
		mRunningFlag = false;
		if (mThread.joinable())
		{
			mThread.join();
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
					auto jobQueueLock = mJobQueue.Lock();
					if (!jobQueueLock->empty())
					{
						auto job = std::move(jobQueueLock->front());
						jobQueueLock->pop_front();
						return Optional(std::move(job));
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
				if (!mRunningFlag)
				{
					return;
				}

				std::this_thread::yield();
			}
		}
	}
}
