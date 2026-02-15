#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Types/Optional.hpp"
#include <atomic>
#include <functional>
#include <memory>
#include <thread>


namespace Strawberry::Core
{
	class RepeatingTask
	{
	public:
		/// Accepts a function taking no arguments.
		explicit RepeatingTask(
			std::function<void()> function,
			Optional<std::function<void()>> startUp = NullOpt,
			Optional<std::function<void()>> shutDown = NullOpt
		)
			: mShouldRun(std::make_unique<std::atomic<bool>>(false))
			, mThread(std::make_unique<std::thread>())
			, mStartUp(std::move(startUp))
			, mShutdown(std::move(shutDown))
			, mFunction(std::move(function))
		{
			Start();
		}


		RepeatingTask(const RepeatingTask&) = delete;
		RepeatingTask& operator=(const RepeatingTask&) = delete;
		RepeatingTask(RepeatingTask&&) = default;
		RepeatingTask& operator=(RepeatingTask&&) = default;


		~RepeatingTask()
		{
			Stop();
		}


		[[nodiscard]] bool IsRunning() const
		{
			return mThread->joinable();
		}


		void Start()
		{
			*mShouldRun = true;
			*mThread    = std::thread([this]
			{
				if (mStartUp)
				{
					std::invoke(mStartUp.Value());
				}

				while (mShouldRun)
				{
					std::invoke(mFunction);
				}

				if (mShutdown)
				{
					std::invoke(mShutdown.Value());
				}
			});
		}


		void Stop()
		{
			if (IsRunning())
			{
				*mShouldRun = false;
				mThread->join();
			}
		}


	private:
		std::unique_ptr<std::atomic<bool>> mShouldRun;
		std::unique_ptr<std::thread>       mThread;
		Optional<std::function<void()>>    mStartUp;
		Optional<std::function<void()>>    mShutdown;
		std::function<void()>              mFunction;
	};
} // namespace Strawberry::Core
