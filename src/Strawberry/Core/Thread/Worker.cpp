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
