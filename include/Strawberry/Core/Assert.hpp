#pragma once



#include <cstddef>
#include <iostream>
#include <utility>
#include <csignal>


#include "Markers.hpp"



namespace Strawberry::Core
{
	inline constexpr void Assert(bool value)
	{
#if !NDEBUG
		if (!value)
		{
			DebugBreak();
		}
		else
		{
			return;
		}
#endif // !NDEBUG
	}
}