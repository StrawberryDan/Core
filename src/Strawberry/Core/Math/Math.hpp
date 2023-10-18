#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <concepts>
#include <type_traits>
#include <utility>


namespace Strawberry::Core::Math
{
	template <std::integral T>
	T GreatestCommonDivisor(T a, T b)
	{
		std::pair<T, T> pair(a, b);

		while (pair.second != 0) { pair = {pair.second, pair.first % pair.second}; }

		return pair.first;
	}


	template <std::integral A, std::integral B>
	auto CeilDiv(A num, B den)
	{
		return (num + den - 1) / den;
	}


	template <std::integral A, std::integral B>
	auto RoundUpToMultiple(A value, B multiple)
	{
		return multiple * CeilDiv(value, multiple);
	}

	template <std::integral A, std::integral B>
	auto RoundDownToMultiple(A value, B multiple)
	{
		return multiple * (value / multiple);
	}
} // namespace Strawberry::Core::Math
