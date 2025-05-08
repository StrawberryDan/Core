#include "Worker.hpp"
#include "Strawberry/Core/Types/Optional.hpp"

namespace Strawberry::Core
{
	Worker::Worker()
		: mRunningFlag(true)
		, mThread([this] { Run(); })
	{}


	Worker::~Worker()
	{
		Join();
	}

	void Worker::Queue(Task&& job)
	{
		mJobQueue.Lock()->emplace_back(std::move(job));
	}


	void Worker::Join()
	{
		mRunningFlag = false;
		if (mThread.joinable())
		{
			mThread.join();
		}
	}


	void Worker::Run()
	{
		while (true)
		{
			bool wasEmpty = false;

			Optional<Task> job = GetNextJob();

			if (job)
			{
				std::invoke(job.Unwrap());
			}
			else
			{
				wasEmpty = true;
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


	Optional<Task> Worker::GetNextJob()
	{
		Optional<Task> nextJob;

		auto jobQueueLock = mJobQueue.Lock();
		if (!jobQueueLock->empty())
		{
			nextJob = std::move(jobQueueLock->front());
			jobQueueLock->pop_front();
		}

		return nextJob;
	}
}
