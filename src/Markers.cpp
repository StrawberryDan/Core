#include "Strawberry/Core/Markers.hpp"



#include <cstdlib>
#include <csignal>

#if defined(_WIN32)
	#include <debugapi.h>
#endif



namespace Strawberry::Core
{
	void DebugBreak()
	{
#if defined(_WIN32)

#elif defined(__UNIX) || defined(__APPLE__)
		std::raise(SIGTRAP);
#else
		#warning "Debug Break not defined on this target!";
#endif
	}



	[[noreturn]] void Unreachable()
	{
#if __cpp_lib_unreachable
		std::unreachable();
#elif __clang__
		__builtin_unreachable();
#else
		std::abort();
#endif
	}
}