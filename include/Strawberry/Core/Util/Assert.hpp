#pragma once


#include <csignal>
#include <cstddef>
#include <iostream>
#include <utility>


#include "Markers.hpp"


namespace Strawberry::Core
{
	inline constexpr void Assert([[maybe_unused]] bool value)
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
#endif// !NDEBUG
	}
}// namespace Strawberry::Core