#include "ThreadPool.hpp"


namespace Strawberry::Core
{
	ThreadPool::ThreadPool(const int threadCount)
		: mThreadCount(std::clamp<int>(threadCount >= 0 ? threadCount : std::thread::hardware_concurrency() + threadCount, 0, std::thread::hardware_concurrency()))
		, mWorkers(std::make_unique<Worker[]>(mThreadCount))
	{}


	ThreadPool::ThreadPool(float percentageOfThreads)
		: mThreadCount(percentageOfThreads * std::thread::hardware_concurrency())
		, mWorkers(std::make_unique<Worker[]>(mThreadCount))
	{}


	ThreadPool::~ThreadPool()
	{
		ZoneScoped;

		Join();
	}


	void ThreadPool::Join()
	{
		ZoneScoped;

		for (int i = 0; i < mThreadCount; i++)
		{
			mWorkers[i].Join();
		}
	}
}
