#pragma once


#include <type_traits>


namespace Strawberry::Standard::Math
{
	template <typename A, typename B>
	auto CeilDiv(A num, B den) requires ( std::is_integral_v<A> && std::is_integral_v<B> )
	{
		return (num + den - 1) / den;
	}



	template<typename A, typename B>
	auto RoundUpToNearestMultiple(A value, B multiple) requires ( std::is_integral_v<A> && std::is_integral_v<B> )
	{
		return multiple * CeilDiv(value, multiple);
	}
}