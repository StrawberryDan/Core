#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Vector.hpp"
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <algorithm>
#include <array>
#include <concepts>

//======================================================================================================================
//  Class Definition
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	template<typename T, size_t W, size_t H> requires (std::signed_integral<T> || std::floating_point<T>)
	class Matrix
	{
	public:
		/// Produced a matrix of all zeros;
		constexpr static Matrix Zeroed() noexcept
		{
			Matrix result;
			for (size_t i = 0; i < std::min(W, H); i++)
			{
				result[i][i] = T(0);
			}
			return result;
		}


		/// Identity Matrix Constructor
		constexpr Matrix() noexcept
			: mValue{T(0)}
		{
			for (size_t i = 0; i < std::min(H, W); i++) mValue[i][i] = T(1);
		}


		template<typename... Args> requires (sizeof...(Args) == H * W && (std::convertible_to<T, Args> && ...))
		constexpr explicit Matrix(Args... args) noexcept
		{
			std::array<T, H * W> values{static_cast<T>(args)...};
			for (size_t i = 0; i < values.size(); i++) mValue[i % W][i / W] = values[i];
		}


		constexpr Matrix(const Vector<T, H>& v) noexcept requires (W == 1)
		{
			for (int i = 0; i < H; i++)
			{
				(*this)[0][i] = v[i];
			}
		}


		constexpr operator Vector<T, H>() const noexcept requires (W == 1)
		{
			Vector<T, H> vector;
			for (int i = 0; i < H; i++)
			{
				vector[i] = (*this)[0][i];
			}
			return vector;
		}


		constexpr T* operator[](size_t col) noexcept
		{
			Core::Assert(col < W);
			return mValue[col];
		}


		constexpr const T* operator[](size_t col) const noexcept
		{
			Core::Assert(col < W);
			return mValue[col];
		}


		constexpr bool operator==(const Matrix& b) const noexcept
		{
			for (size_t row = 0; row < H; row++)
			{
				for (size_t col = 0; col < W; col++)
				{
					if (mValue[col][row] != b[col][row]) return false;
				}
			}

			return true;
		}


		constexpr bool operator!=(const Matrix& b) const noexcept
		{
			for (size_t row = 0; row < H; row++)
			{
				for (size_t col = 0; col < W; col++)
				{
					if (mValue[col][row] == b[col][row]) return false;
				}
			}

			return true;
		}


		constexpr Matrix operator+(const Matrix& b) const noexcept
		{
			Matrix result;
			for (size_t row = 0; row < H; row++)
			{
				for (size_t col = 0; col < W; col++)
				{
					result[col][row] = (*this)[col][row] + b[col][row];
				}
			}
			return result;
		}


		constexpr Matrix operator-(const Matrix& b) const noexcept
		{
			Matrix result;
			for (size_t row = 0; row < H; row++)
			{
				for (size_t col = 0; col < W; col++)
				{
					result[col][row] = (*this)[col][row] - b[col][row];
				}
			}
			return result;
		}


		template <size_t W2, size_t H2>
		constexpr Matrix<T, W2, H> operator*(const Matrix<T, W2, H2>& b) const noexcept requires (W == H2)
		{
			Matrix<T, W2, H> result = Matrix<T, W2, H>::Zeroed();
			for (size_t col = 0; col < W2; col++)
			{
				for (size_t row = 0; row < H; row++)
				{
					for (size_t k = 0; k < W; k++)
					{
						result[col][row] += (*this)[k][row] * b[col][k];
					}
				}
			}
			return result;
		}


		constexpr Vector<T, W> operator*(const Vector<T, W>& vector) const noexcept
		{
			return Vector((*this) * Matrix<T, 1, W>(vector));
		}


		Matrix Transposed() const
		{
			Matrix result;
			for (int x = 0; x < W; x++)
				for (int y       = 0; y < H; y++)
					result[x][y] = mValue[y][x];
			return result;
		}

	private:
		T mValue[H][W];
	};


	//======================================================================================================================
	//  Type Aliases
	//----------------------------------------------------------------------------------------------------------------------
	using Mat2  = Matrix<double, 2, 2>;
	using Mat3  = Matrix<double, 3, 3>;
	using Mat4  = Matrix<double, 4, 4>;
	using Mat2f = Matrix<float, 2, 2>;
	using Mat3f = Matrix<float, 3, 3>;
	using Mat4f = Matrix<float, 4, 4>;
	using Mat2i = Matrix<int, 2, 2>;
	using Mat3i = Matrix<int, 3, 3>;
	using Mat4i = Matrix<int, 4, 4>;


	// Deduction Guides
	template <typename T, size_t D>
	Matrix(Vector<T, D>) -> Matrix<T, 1, D>;

	// Deduction Guides
	template <typename T, size_t D>
	Vector(Matrix<T, 1, D>) -> Vector<T, D>;
} // namespace Strawberry::Core::Math
