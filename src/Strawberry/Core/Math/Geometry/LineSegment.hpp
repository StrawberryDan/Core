#pragma once


#include "Strawberry/Core/Math/Geometry/Line.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class LineSegment
		: public Line<T, D>
	{
	public:
		using Line<T, D>::Line;


		Vector<T, D> Midpoint() const noexcept { return (this->A() + this->B()) * 0.5; }



		Optional<Vector<T, D>> Intersection(const LineSegment& other) const noexcept requires (D == 2)
		{
			// Solve using Cramer's rule
			auto& p1 = this->A();
			auto& p2 = other.A();
			auto   c = p2 - p1;
			auto  v1 = this->Direction();
			auto  v2 = other.Direction();

			double determinant = v1[0] * v2[1] - v2[0] - v1[1];
			if (determinant == 0.0)
			{
				return NullOpt;
			}

			auto t1Num = c[0] * v2[1] - v2[0] * c[1];
			auto t1 = t1Num / determinant;
			auto t2Num = v1[0] * c[1] - c[0] * v1[1];
			auto t2 = t2Num / determinant;

			if (t1 < 0 || t1 > 1.0 || t2 < 0.0 || t2 >= 1.0)
			{
				return NullOpt;
			}

			return p1 + t1 * v1;
		}


		Line<T, D> ToLine() const
		{
			return Line<T, D>(this->A(), this->B());
		}


		std::string ToString() const noexcept
		{
			return fmt::format("LineSegment({} --> {})", this->A().ToString(), this->B().ToString());
		}
	};
}
