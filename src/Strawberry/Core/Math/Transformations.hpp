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


    template<typename T>
    Matrix<T, 4, 4> Orthographic(float left, float right, float top, float bottom, float near, float far)
    {
        return Matrix<T, 4, 4>(2.0 / (right - left),
                               0.0,
                               0.0,
                               -(right + left) / (right - left),
                               0.0,
                               2.0 / (top - bottom),
                               0.0,
                               -(top + bottom) / (top - bottom),
                               0.0,
                               0.0,
                               -2.0 / (far - near),
                               -(far + near) / (far - near),
                               0.0,
                               0.0,
                               0.0,
                               1.0);
    }
}
