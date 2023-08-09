#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Util/Clock.hpp"

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	class Metronome
	{
	public:
		Metronome() = default;
		Metronome(double frequency, double eagerness = 1.0);


		explicit operator bool();
		void Tick();
		void Restart();


		void SetPreemption(double preemption);
		void SetFrequency(double seconds);


	private:
		double Frequency;
		Clock  mClock;
		double mSecondsAhead     = 0.0;
		double mPreemption       = 0.0;
	};
}