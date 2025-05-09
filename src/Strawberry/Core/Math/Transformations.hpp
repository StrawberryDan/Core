#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Matrix.hpp"
#include "Units.hpp"


//======================================================================================================================
//  Function Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	template<typename T, size_t D>
	Matrix<T, D + 1, D + 1> Translate(const Vector<T, D>& translation)
	{
		Matrix<T, D + 1, D + 1> result;
		for (int i       = 0; i < D; i++)
			result[D][i] = translation[i];
		return result;
	}


	template<typename T, typename... Ts>
	Matrix<T, sizeof...(Ts) + 1, sizeof...(Ts) + 1> Translate(Ts... args)
	{
		return Translate(Core::Math::Vector<T, sizeof...(Ts)>(args...));
	}


	template<typename T, size_t D>
	Matrix<T, D + 1, D + 1> Scale(const Vector<T, D>& scale)
	{
		Matrix<T, D + 1, D + 1> result;
		for (int i       = 0; i < D; i++)
			result[i][i] = scale[i];
		return result;
	}


	template<typename T, typename... Ts>
	Matrix<T, sizeof...(Ts) + 1, sizeof...(Ts) + 1> Scale(Ts... args)
	{
		return Scale(Core::Math::Vector<T, sizeof...(Ts)>(args...));
	}


	template <typename T>
	constexpr Matrix<T, 2, 2> Rotate2D(Radians radians)
	{
		const T cosx = std::cos(radians);
		const T sinx = std::sin(radians);
		return Matrix<T, 2, 2>{
			cosx, -sinx,
			sinx, cosx
		};
	}


	template <typename T>
	Matrix<T, 4, 4> RotateX(Radians radians)
	{
		T cosx = std::cos(radians);
		T sinx = std::sin(radians);

		return Matrix<T, 4, 4>{
			1.0f, 0.0f,  0.0f, 0.0f,
			0.0f, cosx, -sinx, 0.0f,
			0.0f, sinx,  cosx, 0.0f,
			0.0f, 0.0f,  0.0f, 1.0f,
		};
	}


	template <typename T>
	Matrix<T, 4, 4> RotateY(Radians radians)
	{
		T cosx = std::cos(radians);
		T sinx = std::sin(radians);

		return Matrix<T, 4, 4>{
			 cosx, 0.0f, sinx, 0.0f,
			 0.0f, 1.0f, 0.0f, 0.0f,
			-sinx, 0.0f, cosx, 0.0f,
			 0.0f, 0.0f, 0.0f, 1.0f,
		};
	}


	template <typename T>
	Matrix<T, 4, 4> RotateZ(Radians radians)
	{
		T cosx = std::cos(radians);
		T sinx = std::sin(radians);

		return Matrix<T, 4, 4>{
			cosx, -sinx, 0.0f, 0.0f,
			sinx, cosx, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}
}
