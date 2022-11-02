#include "Standard/Clock.hpp"



namespace Strawberry::Standard
{
	Clock::Clock(bool run)
	    : mIsRunning(run)
	    , mBuffer{}
	    , mStart(std::chrono::system_clock::now())
	    , mEnd(mStart)
	{

	}



	void Clock::Start()
	{
	    if (!mIsRunning)
	    {
	        mStart = std::chrono::system_clock::now();
	    }
	}



	double Clock::Stop()
	{
	    if (mIsRunning)
	    {
	        mEnd = std::chrono::system_clock::now();
	        mBuffer += (mEnd - mStart);
	    }
	    return mBuffer.count();
	}



	double Clock::Read() const
	{
	    if (mIsRunning)
	    {
	        return (std::chrono::duration_cast<Duration>(std::chrono::system_clock::now() - mStart) + mBuffer).count();
	    }
	    else
	    {
	        return mBuffer.count();
	    }
	}



	void Clock::Restart()
	{
	    mBuffer = {};
	    mStart  = std::chrono::system_clock::now();
	}
}