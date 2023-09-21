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
		/// Produced a matrix of all zeros;
		constexpr static Matrix Zeroed()
		{
			Matrix result;
			for (size_t i = 0; i < std::min(W, H); i++)
			{
				result[i, i] = T(0);
			}
			return result;
		}


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

		constexpr bool operator==(const Matrix& b) const
		{
			for (size_t row = 0; row < H; row++)
			{
				for (size_t col = 0; col < W; col++)
				{
					if (mValue[row][col] != b[row, col]) return false;
				}
			}

			return true;
		}

		constexpr bool operator!=(const Matrix& b) const
		{
			for (size_t row = 0; row < H; row++)
			{
				for (size_t col = 0; col < W; col++)
				{
					if (mValue[row][col] == b[row, col]) return false;
				}
			}

			return true;
		}

	private:
		T mValue[H][W];
	};
} // namespace Strawberry::Core::Math