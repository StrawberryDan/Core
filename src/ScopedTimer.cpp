//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/ScopedTimer.hpp"
// Strawberry Core
#include "Strawberry/Core/Logging.hpp"



namespace Strawberry::Core
{
	ScopedTimer::ScopedTimer(const std::string& name)
		: mName(name)
		, mClock()
	{}


	ScopedTimer::~ScopedTimer()
	{
		Logging::Trace("{} ---- {}", mName, *mClock);
	}
}