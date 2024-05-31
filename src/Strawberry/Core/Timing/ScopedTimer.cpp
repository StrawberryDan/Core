//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <utility>

#include "Strawberry/Core/Timing/ScopedTimer.hpp"
// Strawberry Core
#include "Strawberry/Core/IO/Logging.hpp"


namespace Strawberry::Core
{
    ScopedTimer::ScopedTimer(std::string name)
        : mName(std::move(name))
        , mClock() {}


    ScopedTimer::~ScopedTimer()
    {
        Logging::Trace("{} ---- {}", mName, *mClock);
    }
} // namespace Strawberry::Core
