#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Types/Optional.hpp"
#include <atomic>
#include <functional>
#include <thread>


namespace Strawberry::Core
{
	class RepeatingTask
	{
	public:
		/// Accepts a function taking no arguments.
		explicit RepeatingTask(std::function<void()> function)
			: mShouldRun(false)
			, mStartUp()
			, mFunction(std::move(function))
		{
			Start();
		}


		/// Accepts a function taking no arguments.
		explicit RepeatingTask(
			std::function<void()> function,
			Optional<std::function<void>> startUp = NullOpt,
			Optional<std::function<void>> shutDown = NullOpt
		)
			: mShouldRun(false)
			, mStartUp(std::move(startup))
			, mShutdown(std::move(shutDown))
			, mFunction(std::move(function))
		{
			Start();
		}


		~RepeatingTask()
		{
			Stop();
		}


		[[nodiscard]] bool IsRunning() const
		{
			return mThread.joinable();
		}


		void Start()
		{
			mShouldRun = true;
			mThread    = std::thread([this]
			{
				if (mStartUp)
				{
					std::invoke(mStartUp.Value());
				}

				while (self->mShouldRun)
				{
					self->mFunction();
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
				mShouldRun = false;
				mThread.join();
			}
		}


		RepeatingTask(const RepeatingTask& rhs)            = delete;
		RepeatingTask& operator=(const RepeatingTask& rhs) = delete;
		RepeatingTask(RepeatingTask&& rhs) = delete;
		RepeatingTask& operator=(RepeatingTask&& rhs) = delete;


	private:
		std::atomic<bool>               mShouldRun;
		std::thread                     mThread;
		Optional<std::function<void()>> mStartUp;
		Optional<std::function<void()>> mShutdown;
		std::function<void()>           mFunction;
	};
} // namespace Strawberry::Core
