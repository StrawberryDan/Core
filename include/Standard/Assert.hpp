#pragma once



#include <cstddef>
#include <iostream>
#include <utility>



namespace Strawberry::Standard
{
	inline constexpr void Assert(bool value)
	{
#if !NDEBUG
		if (!value)
		{
			std::abort();
		}
		else
		{
			return;
		}
#endif // !NDEBUG
	}
}