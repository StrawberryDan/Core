#pragma once


#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, size_t D>
	class Line
	{
	public:
		Line() = default;
		Line(const Vector<T, D> a, const Vector<T, D>& b)
			: mPoints{a, b} {}


		Vector<T, D>& A() { return mPoints[0]; }
		Vector<T, D>& B() { return mPoints[1]; }


	private:
		Vector<T, D> mPoints[2];
	};


	template <typename T, unsigned D>
	class LineSegment : private Line<T, D>
	{
	public:
		using Line<T, D>::Line;


		using Line<T, D>::A;
		using Line<T, D>::B;
	};
}
