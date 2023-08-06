#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <atomic>
#include <thread>
#include <functional>



namespace Strawberry::Core
{
	class RepeatingTask
	{
	public:
		/// Accepts a function taking no arguments.
		explicit RepeatingTask(std::function<void()> function)
				: mShouldRun(true)
				  , mStartUp()
				  , mFunction(std::move(function))
		{
			mThread = std::thread([this] ()
								  {
									  if (mStartUp) mStartUp();

									  while (mShouldRun)
									  {
										  mFunction();
									  }
								  });
		}


		/// Accepts a function taking no arguments.
		explicit RepeatingTask(std::function<void()> function, std::function<void()> startup)
			: mShouldRun(true)
			, mStartUp(std::move(startup))
			, mFunction(std::move(function))
		{
			mThread = std::thread([this] ()
			{
				if (mStartUp) mStartUp();

				while (mShouldRun)
				{
					mFunction();
				}
			});
		}


		/// Will pass 'this' as the first argument always.
		explicit RepeatingTask(const std::function<void(RepeatingTask *)>& function)
				: mShouldRun(true)
				, mStartUp()
				, mFunction([function, this] { function(this); })
		{
			mThread = std::thread([this] ()
			{
				if (mStartUp) mStartUp();

				while (mShouldRun)
				{
					mFunction();
				}
			});
		}


		/// Accepts a function taking no arguments.
		explicit RepeatingTask(std::function<void(RepeatingTask *)> startup, std::function<void()> function)
				: mShouldRun(true)
				  , mStartUp([startup, this] { startup(this); })
				  , mFunction(std::move(function))
		{
			mThread = std::thread([this] ()
			{
				if (mStartUp) mStartUp();

				while (mShouldRun)
				{
					mFunction();
				}
			});
		}


		/// Will pass 'this' as the first argument always.
		explicit RepeatingTask(std::function<void()> startup, const std::function<void(RepeatingTask *)>& function)
				: mShouldRun(true)
				, mStartUp(std::move(startup))
				, mFunction([function, this] { function(this); })
		{
			mThread = std::thread([this] ()
			{
				if (mStartUp) mStartUp();

				while (mShouldRun)
				{
					mFunction();
				}
			});
		}


		/// Will pass 'this' as the first argument always.
		explicit RepeatingTask(std::function<void(RepeatingTask *)> startup, const std::function<void(RepeatingTask *)>& function)
				: mShouldRun(true)
				, mStartUp([startup, this] { startup(this); })
				, mFunction([function, this] { function(this); })
		{
			mThread = std::thread([this] ()
			{
				if (mStartUp) mStartUp();

				while (mShouldRun)
				{
					 mFunction();
				}
			});
		}


		~RepeatingTask()
		{
			mShouldRun = false;
			mThread.join();
		}


		bool IsRunning() const
		{
			return mShouldRun;
		}


		void Stop()
		{
			mShouldRun = false;
		}


		RepeatingTask(const RepeatingTask& rhs)            = delete;
		RepeatingTask& operator=(const RepeatingTask& rhs) = delete;
		RepeatingTask(RepeatingTask&& rhs)                 = delete;
		RepeatingTask& operator=(RepeatingTask&& rhs)      = delete;


	private:
		std::atomic<bool>           mShouldRun;
		std::thread                 mThread;
		const std::function<void()> mStartUp;
		const std::function<void()> mFunction;
	};
}