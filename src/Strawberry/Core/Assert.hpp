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
#if STRAWBERRY_DEBUG
		if (!value)
		{
			DebugBreak();
		}
		else
		{
			return;
		}
#endif // STRAWBERRY_DEBUG
	}


	template<typename A, typename B>
	inline constexpr void AssertEQ([[maybe_unused]] A a, [[maybe_unused]] B b)
	{
		Assert(a == b);
	}


	template<typename A, typename B>
	inline constexpr void AssertNEQ([[maybe_unused]] A a, [[maybe_unused]] B b)
	{
		Assert(a != b);
	}
} // namespace Strawberry::Core
