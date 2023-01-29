#pragma once



#include <cstddef>
#include <iostream>
#include <utility>
#include <csignal>



namespace Strawberry::Standard
{
	inline constexpr void Assert(bool value)
	{
#if !NDEBUG
		if (!value)
		{
#if __APPLE__
			raise(SIGTRAP);
#endif // __APPLE__
			std::abort();
		}
		else
		{
			return;
		}
#endif // !NDEBUG
	}
}