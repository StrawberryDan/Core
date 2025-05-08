#include "ThreadPool.hpp"


namespace Strawberry::Core
{
	ThreadPool::ThreadPool(const unsigned int threadCount)
		: threadCount(threadCount)
		, mWorkers(std::make_unique<Worker[]>(threadCount))
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
}
