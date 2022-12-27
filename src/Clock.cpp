#include "Standard/Clock.hpp"



namespace Strawberry::Standard
{
	Clock::Clock(bool run)
	    : mBuffer{}
	    , mStartTime()
	{

	}



	void Clock::Start()
	{
	    if (!mStartTime)
	    {
	        mStartTime = std::chrono::system_clock::now();
	    }
	}



	double Clock::Stop()
	{
	    if (mStartTime)
	    {
	        auto now = std::chrono::system_clock::now();
	        mBuffer += (now - *mStartTime);
	    }
	    return mBuffer.count();
	}



	double Clock::Read() const
	{
	    if (mStartTime)
	    {
			auto now = std::chrono::system_clock::now();
			auto currentRun = std::chrono::duration_cast<Duration>(now - *mStartTime);
	        return (currentRun + mBuffer).count();
	    }
	    else
	    {
	        return mBuffer.count();
	    }
	}



	void Clock::Restart()
	{
	    mBuffer = {};
	    mStartTime  = std::chrono::system_clock::now();
	}
}