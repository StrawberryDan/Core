#include "ThreadPool.hpp"


namespace Strawberry::Core
{
	ThreadPool::ThreadPool(const unsigned int threadCount)
		: mThreadCount(threadCount)
		, mWorkers(std::make_unique<Worker[]>(threadCount))
	{}


	ThreadPool::ThreadPool(float percentageOfThreads)
		: mThreadCount(percentageOfThreads * std::thread::hardware_concurrency())
		, mWorkers(std::make_unique<Worker[]>(mThreadCount))
	{}


	ThreadPool::~ThreadPool()
	{
		Join();
	}


	void ThreadPool::Join()
	{
		for (int i = 0; i < mThreadCount; i++)
		{
			mWorkers[i].Join();
		}
	}
}
