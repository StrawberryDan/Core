//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Util/Metronome.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	Metronome::Metronome(double frequency, double preemption)
		: Frequency(frequency)
		, mPreemption(preemption)
	{}


	Metronome::operator bool()
	{
		double time = *mClock;
		auto result = time >= (Frequency + mSecondsAhead) - mPreemption;
		return result;
	}


	void Metronome::Tick()
	{
		mSecondsAhead += Frequency - *mClock;
		mClock.Restart();
	}


	void Metronome::SetPreemption(double preemption)
	{
		mPreemption = preemption;
	}


	void Metronome::SetAllowedSecondsAhead(double seconds)
	{
		Frequency = seconds;
	}
}