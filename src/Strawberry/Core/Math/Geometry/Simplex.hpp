#pragma once


#include "Strawberry/Core/Math/Vector.hpp"
#include <array>


namespace Strawberry::Core::Math
{
	template <typename T, unsigned Dimension, unsigned Order>
	class Simplex
	{
		Simplex() = default;
		Simplex(std::array<Vector<T, Dimension>, Order> points);


		Vector<T, Dimension>& Point(unsigned index) { return mPoints[index]; }
		const Vector<T, Dimension>& Point(unsigned index) const { return mPoints[index]; }


	private:
		std::array<Vector<T, Dimension>, Order> mPoints;
	};


	template <typename T, unsigned Dimension>
	using Triangle = Simplex<T, Dimension, 3>;
}
