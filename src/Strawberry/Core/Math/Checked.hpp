#pragma once
// Standard Library
#include <concepts>
#include <limits>


namespace Strawberry::Core::Math
{
	template <std::integral T>
	struct CheckedResult
	{
		bool overflow;
		T	 value;

		explicit operator bool() const noexcept { return overflow; };
		T operator*() const noexcept { return value; }
	};


	template <std::integral T>
	constexpr CheckedResult<T> CheckedAddition(T a, T b) noexcept
	{
#if STRAWBERRY_COMPILER_GCC || STRAWBERRY_COMPILER_CLANG
		CheckedResult<T> result;
		result.overflow = __builtin_add_overflow(a, b, &result.value);
		return result;
#else
#error "No implementation for CheckedAddition for this compiler"
#endif
	}

	template <std::integral T>
	constexpr CheckedResult<T> CheckedSubtraction(T a, T b) noexcept
	{
#if STRAWBERRY_COMPILER_GCC || STRAWBERRY_COMPILER_CLANG
		CheckedResult<T> result;
		result.overflow = __builtin_sub_overflow(a, b, &result.value);
		return result;
#else
#error "No implementation for CheckedSubtraction for this compiler"
#endif
	}

	template <std::integral T>
	constexpr CheckedResult<T> CheckedMultiplication(T a, T b) noexcept
	{
#if STRAWBERRY_COMPILER_GCC || STRAWBERRY_COMPILER_CLANG
		CheckedResult<T> result;
		result.overflow = __builtin_mul_overflow(a, b, &result.value);
		return result;
#else
#error "No implementation for CheckedMultiplication for this compiler"
#endif
	}

	template <std::integral T>
	constexpr T SaturatingAddition(T a, T b) noexcept
	{
		CheckedResult<T> result = CheckedAddition(a, b);
		if (result.overflow)
		{
			if constexpr (std::unsigned_integral<T>)
			{
				return std::numeric_limits<T>::max();
			}
			else
			{
				return (b > 0)
					? std::numeric_limits<T>::max()
					: std::numeric_limits<T>::lowest();
			}
		}
		else
		{
			return result.value;
		}
	}

	template <std::integral T>
	constexpr T SaturatingSubtraction(T a, T b) noexcept
	{
		CheckedResult<T> result = CheckedSubtraction<T>(a, b);
		if (result.overflow)
		{
			if constexpr (std::unsigned_integral<T>)
			{
				return 0;
			}
			else
			{
				return (b > 0)
					? std::numeric_limits<T>::lowest()
					: std::numeric_limits<T>::max();
			}
		}
		else
		{
			return result.value;
		}
	}

	template <std::integral T>
	constexpr T SaturatingMultiplication(T a, T b) noexcept
	{
		CheckedResult<T> result = CheckedMultiplication<T>(a, b);
		if (result.overflow)
		{
			if constexpr (std::unsigned_integral<T>)
			{
				return std::numeric_limits<T>::max();
			}
			else
			{
				return (b > 0)
					? std::numeric_limits<T>::max()
					: std::numeric_limits<T>::lowest();
			}
		}
		else
		{
			return result.value;
		}
	}
}
