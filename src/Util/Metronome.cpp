//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Util/Metronome.hpp"


//======================================================================================================================
//  Class Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	Metronome::Metronome(double frequency, double eagerness)
		: Frequency(frequency)
		, mEagerness(1.0 / eagerness)
	{
		Core::Assert(!std::isinf(mEagerness));
		Core::Assert(!std::isnan(mEagerness));
	}


	Metronome::operator bool()
	{
		double time = *mClock;
		auto result = time >= (Frequency + mSecondsAhead) * mEagerness;
		return result;
	}


	void Metronome::Tick()
	{
		mSecondsAhead += Frequency - *mClock;
		mClock.Restart();
	}


	void Metronome::SetEagerness(double eagerness)
	{
		mEagerness = 1.0 / eagerness;
		Core::Assert(mEagerness > 0.0);
	}


	void Metronome::SetAllowedSecondsAhead(double seconds)
	{
		Frequency = seconds;
	}
}