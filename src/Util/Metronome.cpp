//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Util/Metronome.hpp"
#include "Strawberry/Core/Util/Logging.hpp"


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
		double time = *mClock;
		auto result = time >= (mFrequency + mSecondsAhead) - mPreemption;
		return result;
	}


	void Metronome::Tick()
	{
		mSecondsAhead += mFrequency - *mClock;
		mClock.Restart();

		if (mSecondsAhead <= 0.0)
		{
			Core::Logging::Warning("{}:{} Metronome has fallen behind!", __FILE__, __LINE__);
		}
	}


	void Metronome::Restart()
	{
		mSecondsAhead = 0.0;
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
}