#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Vector.hpp"
// Standard Library
#include <algorithm>
#include <array>
#include <concepts>

//======================================================================================================================
//  Class Definition
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	template <typename T, size_t H, size_t W>
		requires (std::signed_integral<T> || std::floating_point<T>)
	class Matrix
	{
	public:
		/// Identity Matrix Constructor
		constexpr Matrix()
			: mValue{T(0)}
		{
			for (size_t i = 0; i < std::min(H, W); i++) mValue[i][i] = T(1);
		}

		template <typename... Args>
			requires (sizeof...(Args) == H * W && (std::same_as<T, Args> && ...))
		constexpr Matrix(Args... args)
		{
			std::array<T, H * W> values{args...};
			for (size_t i = 0; i < values.size(); i++) mValue[i / W][i % W] = values[i];
		}

		constexpr T& operator[](size_t row, size_t col) { return mValue[row][col]; }

		constexpr T& operator[](size_t row, size_t col) const { return mValue[row][col]; }

		const T& operator[](size_t row, size_t col) const { return mValue[row][col]; }

	private:
		T mValue[H][W];
	};
} // namespace Strawberry::Core::Math