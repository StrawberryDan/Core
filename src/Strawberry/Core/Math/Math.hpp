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
} // namespace Strawberry::Core::Math
