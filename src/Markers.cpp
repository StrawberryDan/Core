#include "Standard/Markers.hpp"



namespace Strawberry::Standard
{
	[[noreturn]] void Unreachable()
	{
#if __cpp_lib_unreachable
		std::unreachable();
#elif __clang__
		__builtin_unreachable();
#else
		#error "Unimplemented!"
#endif
}
}