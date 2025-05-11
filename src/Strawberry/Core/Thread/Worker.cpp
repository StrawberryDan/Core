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
		ZoneScoped;

		Join();
	}


	void Worker::Join()
	{
		ZoneScoped;

		mRunningFlag = false;

		std::unique_lock lock(mTaskQueueMutex);
		mTaskQueueCV.notify_one();
		lock.unlock();

		if (mThread.joinable())
		{
			mThread.join();
		}
	}


	void Worker::Run()
	{
		ZoneScoped;

		while (true)
		{
			std::unique_lock lock(mTaskQueueMutex);
			mTaskQueueCV.wait(lock, [&] { return !mRunningFlag || !mTaskQueue.empty(); });
			std::deque<PackagedTask> tasks = std::move(mTaskQueue);
			lock.unlock();

			for (auto&& task : tasks)
			{
				std::invoke(task);
			}

			if (tasks.empty())
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
