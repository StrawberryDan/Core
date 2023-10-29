#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Matrix.hpp"


//======================================================================================================================
//  Function Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	template <typename T, size_t D>
	Matrix<T, D + 1, D + 1> Translation(const Vector<T, D>& translation)
	{
		Matrix<T, D + 1, D + 1> result;
		for (int i = 0; i < D; i++)
			result[D][i] = translation[i];
		return result;
	}


	template <typename T, size_t D>
	Matrix<T, D + 1, D + 1> Scale(const Vector<T, D>& scale)
	{
		Matrix<T, D + 1, D + 1> result;
		for (int i = 0; i < D; i++)
			result[i][i] = scale[i];
		return result;
	}
}
