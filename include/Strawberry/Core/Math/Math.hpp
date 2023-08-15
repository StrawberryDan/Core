#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <concepts>
#include <type_traits>
#include <utility>


namespace Strawberry::Core::Math
{
	template<std::integral T>
	T GreatestCommonDivisor(T a, T b)
	{
		std::pair<T, T> pair(a, b);

		while (pair.second != 0)
		{
			pair = {pair.second, pair.first % pair.second};
		}

		return pair.first;
	}


	template<typename A, typename B>
	auto CeilDiv(A num, B den)
	requires ( std::is_integral_v<A> && std::is_integral_v<B> )
	{
		return (num + den - 1) / den;
	}


	template<typename A, typename B>
	auto RoundUpToNearestMultiple(A value, B multiple)
	requires ( std::is_integral_v<A> && std::is_integral_v<B> )
	{
		return multiple * CeilDiv(value, multiple);
	}
}