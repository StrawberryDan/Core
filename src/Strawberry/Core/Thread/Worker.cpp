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
			std::deque<PackagedTask> tasks = std::move(*mJobQueue.Lock());

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
