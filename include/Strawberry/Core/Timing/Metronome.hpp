#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Clock.hpp"

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	class Metronome
	{
	public:
		Metronome() = default;
		explicit Metronome(double frequency, double eagerness = 1.0);


		explicit operator bool();
		void Tick();
		void TickWithoutProgress();

		double GetPreemption() const { return mPreemption; }

		double GetFrequency() const { return mFrequency; }

		void SetPreemption(double preemption);
		void SetFrequency(double frequency);


	private:
		double mFrequency;
		Clock  mClock;
		double mSecondsAhead = 0.0;
		double mPreemption   = 0.0;
	};
} // namespace Strawberry::Core