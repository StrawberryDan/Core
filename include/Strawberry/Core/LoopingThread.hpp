#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <atomic>
#include <thread>
#include <functional>



namespace Strawberry::Core
{
	class LoopingThread
	{
	public:
		/// Accepts a function taking no arguments.
		explicit LoopingThread(std::function<void()> function, std::function<void()> startup = {})
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


		// Will pass 'this' as the first argument always.
		explicit LoopingThread(const std::function<void(LoopingThread*)>& function, std::function<void()> startup = {})
				: mShouldRun(true)
				, mStartUp(std::move(startup))
				, mFunction([function, this] { return function(this); })
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


		~LoopingThread()
		{
			mShouldRun = false;
			mThread.join();
		}


		void Stop()
		{
			mShouldRun = false;
		}


		LoopingThread(const LoopingThread& rhs)            = delete;
		LoopingThread& operator=(const LoopingThread& rhs) = delete;
		LoopingThread(LoopingThread&& rhs)                 = delete;
		LoopingThread& operator=(LoopingThread&& rhs)      = delete;


	private:
		std::atomic<bool>           mShouldRun;
		std::thread                 mThread;
		const std::function<void()> mStartUp;
		const std::function<void()> mFunction;
	};
}