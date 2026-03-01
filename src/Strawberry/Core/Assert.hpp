#pragma once


// Core
#include "Strawberry/Core/IO/Logging.hpp"
#include "Strawberry/Core/Markers.hpp"
// Libfmt
#include "fmt/base.h"
#include "fmt/format.h"
// Standard Library
#include <csignal>
#include <cstddef>
#ifdef STRAWBERRY_CORE_ENABLE_LOGGING_STACKTRACE
#include <stacktrace>
#endif


namespace Strawberry::Core
{
	constexpr void Assert([[maybe_unused]] bool value, std::string message = "")
	{
#if STRAWBERRY_CORE_ENABLE_ASSERTIONS
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
			if (message.empty())
			{
				Logging::Error("Assertion failed!");
			}
			else
			{
				Logging::Error("Assertion failed: {}", message);
			}
#endif
			DebugBreak();
			std::terminate();
		}
#endif // STRAWBERRY_DEBUG
	}


	template<typename A, typename B>
	constexpr void AssertEQ([[maybe_unused]] A a, [[maybe_unused]] B b, std::string message = "")
	{
#if STRAWBERRY_CORE_ENABLE_ASSERTIONS
		if constexpr (fmt::is_formattable<A>::value && fmt::is_formattable<B>::value)
		{
			if (message.empty())
				Assert(a == b, fmt::format("Expected {} == {}", a, b));
			else
				Assert(a == b, message);
		}
		else Assert(a == b, message);
#endif
	}


	template<typename A, typename B>
	constexpr void AssertNEQ([[maybe_unused]] A a, [[maybe_unused]] B b, std::string message = "")
	{
#if STRAWBERRY_CORE_ENABLE_ASSERTIONS
		if constexpr (fmt::is_formattable<A>::value && fmt::is_formattable<B>::value)
		{
			if (message.empty())
				Assert(a != b, fmt::format("Expected {} != {}", a, b));
			else
				Assert(a != b, message);
		}
		else Assert(a != b, message);
#endif
	}


	constexpr void AssertImplication(bool a, bool b, std::string message = "")
	{
#if STRAWBERRY_CORE_ENABLE_ASSERTIONS
		Assert(!a || b, message);
#endif
	}
} // namespace Strawberry::Core
