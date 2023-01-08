#include "Standard/Markers.hpp"



#include <cstdlib>



namespace Strawberry::Standard
{
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