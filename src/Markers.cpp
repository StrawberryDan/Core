#include "Core/Markers.hpp"



#include <cstdlib>



namespace Strawberry::Core
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