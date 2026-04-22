#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <concepts>
#include <utility>
#include <cmath>


namespace Strawberry::Core::Math
{
	template<std::integral T>
	constexpr T GreatestCommonDivisor(T a, T b)
	{
		std::pair<T, T> pair(a, b);

		while (pair.second != 0)
		{
			pair = {pair.second, pair.first % pair.second};
		}

		return pair.first;
	}


	template<std::integral A, std::integral B>
	constexpr auto CeilDiv(A num, B den)
	{
		return (num + den - 1) / den;
	}


	template<std::integral A, std::integral B>
	auto RoundUpToMultiple(A value, B multiple)
	{
		return multiple * CeilDiv(value, multiple);
	}


	template<std::integral A, std::integral B>
	constexpr auto RoundDownToMultiple(A value, B multiple)
	{
		return multiple * (value / multiple);
	}


	template<std::floating_point A>
	constexpr A RoundToDecimalPoints(A value, unsigned int decimalPoints)
	{
		A magnitude = std::pow<A>(10, decimalPoints);
		return std::round(value * magnitude) / magnitude;
	}


	template <std::floating_point T>
	constexpr T Smoothstep(T x) noexcept
	{
		if (x <= 0.0)
			return 0.0;
		if (x > 1.0)
			return 1.0;
		return 3.0 * x * x - 2.0 * x * x * x;
	}


	template <std::floating_point T>
	constexpr T Smoothstep(T a, T b, T x) noexcept
	{
		auto ratio = Smoothstep(x);
		return b * ratio + a * (1.0 - ratio);
	}

	/// Returns 1 for positive input, -1 for negative and 0 for 0.
	template <typename T> requires (std::signed_integral<T> || std::floating_point<T>)
	constexpr int SigNum(const T& v) noexcept
	{
		if (v > 0.0)
		{
			return 1;
		}
		if (std::abs(v) == 0.0)
		{
			return 0;
		}
		return -1;
	}
} // namespace Strawberry::Core::Math
