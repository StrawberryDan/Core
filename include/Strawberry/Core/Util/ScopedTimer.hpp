#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Code
#include "Clock.hpp"
// C++ Standard
#include <string>


namespace Strawberry::Core
{
	class ScopedTimer
	{
	public:
		ScopedTimer(const std::string& name);
		ScopedTimer(const ScopedTimer& rhs) = delete;
		ScopedTimer& operator=(const ScopedTimer& rhs) = delete;
		ScopedTimer(ScopedTimer&& rhs) = delete;
		ScopedTimer& operator=(ScopedTimer&& rhs) = delete;
		~ScopedTimer();


	private:
		std::string mName;
		Clock mClock;
	};
}