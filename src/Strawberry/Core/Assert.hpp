#pragma once


#include <csignal>
#include <cstddef>
#include <iostream>
#include <utility>


#include "Markers.hpp"


namespace Strawberry::Core
{
	constexpr void Assert([[maybe_unused]] bool value)
	{
		static_assert(STRAWBERRY_DEBUG);
#if STRAWBERRY_DEBUG
		if (!value)
		{
			DebugBreak();
		}
#endif // STRAWBERRY_DEBUG
	}


	template<typename A, typename B>
	constexpr void AssertEQ([[maybe_unused]] A a, [[maybe_unused]] B b)
	{
		Assert(a == b);
	}


	template<typename A, typename B>
	constexpr void AssertNEQ([[maybe_unused]] A a, [[maybe_unused]] B b)
	{
		Assert(a != b);
	}
} // namespace Strawberry::Core
