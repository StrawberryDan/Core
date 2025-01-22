#pragma once


// Core
#include "Strawberry/Core/IO/Logging.hpp"
#include "Strawberry/Core/Markers.hpp"
// Standard Library
#include <csignal>
#include <cstddef>
#include <iostream>
#include <utility>


namespace Strawberry::Core
{
	constexpr void Assert([[maybe_unused]] bool value)
	{
#if STRAWBERRY_DEBUG
		if (!value)
		{
			Logging::Error("Assertion failed!");
			DebugBreak();
			std::terminate();
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


	constexpr void AssertImplication(bool a, bool b)
	{
		Assert(!a || b);
	}
} // namespace Strawberry::Core
