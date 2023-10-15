//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Timing/Metronome.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	Metronome::Metronome(double frequency, double preemption)
		: mFrequency(frequency)
		, mPreemption(preemption)
	{}


	Metronome::operator bool()
	{
		double time   = *mClock;
		auto   result = time >= (mFrequency + mSecondsAhead) - mPreemption;
		return result;
	}


	void Metronome::Tick()
	{
		mSecondsAhead += mFrequency - *mClock;
		mClock.Restart();
	}

	void Metronome::TickWithoutProgress()
	{
		mClock.Restart();
	}


	void Metronome::SetPreemption(double preemption)
	{
		mPreemption = preemption;
	}


	void Metronome::SetFrequency(double frequency)
	{
		mFrequency = frequency;
	}
} // namespace Strawberry::Core