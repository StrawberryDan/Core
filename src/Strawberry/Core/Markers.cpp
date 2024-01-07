#include "Strawberry/Core/Markers.hpp"


#include <csignal>


#if defined(_WIN32)
#include <debugapi.h>
#endif


namespace Strawberry::Core
{
	void DebugBreak()
	{
#ifndef STRAWBERRY_RELEASE
#if defined(_WIN32)
		__debugbreak();
#elif defined(__UNIX) || defined(__APPLE__)
		std::raise(SIGTRAP);
#else
#warning "Debug Break not defined on this target!";
#endif
#endif
	}


	[[noreturn]] void Unreachable()
	{
		DebugBreak();

#if __cpp_lib_unreachable
		std::unreachable();
#elif __clang__
		__builtin_unreachable();
#else
		std::abort();
#endif
	}
} // namespace Strawberry::Core