#pragma once


// Core
#include "Strawberry/Core/IO/Logging.hpp"
#include "Strawberry/Core/Markers.hpp"
// Standard Library
#include <csignal>
#include <cstddef>
#ifdef STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE
#include <stacktrace>
#endif


namespace Strawberry::Core
{
	constexpr void Assert([[maybe_unused]] bool value)
	{
#if STRAWBERRY_DEBUG
		if (!value)
		{
#ifdef STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE
			std::string trace =
				std::ranges::fold_left_first(
					std::stacktrace::current()
					| std::views::filter([] (auto&& x) { return !x.source_file().ends_with("Strawberry/Core/Assert.hpp"); })
					| std::views::transform([](auto&& x) { return fmt::format("\t{}:{}", x.source_file(), x.source_line()); }),
					[](auto&& x, auto&& y) { return x + "\n" + y; }).value_or("N/A");
			std::cout << fmt::format("Assertion failed at \n{}!", trace) << std::endl;
#else
			Logging::Error("Assertion failed!");
#endif
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
