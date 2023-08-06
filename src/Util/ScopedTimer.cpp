//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Util/ScopedTimer.hpp"
// Strawberry Core
#include "Strawberry/Core/Util/Logging.hpp"



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