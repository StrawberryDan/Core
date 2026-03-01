#include "Strawberry/Core/Math/Geometry/Ray.hpp"


using namespace Strawberry::Core;
using namespace Math;


int main()
{
	Ray<double, 2> r1 ({0, 0}, {1, 1});
	Ray<double, 2> r2({0, -1}, {-1, 1});
	Ray<double, 2> r3({1, 0}, {-1, 1});


	LineSegment<double, 2> ls1({10, 0}, {10, 10});

	Line<double, 2> l1({-10, 0}, {-10, 1});

	AssertEQ(r1.Intersection(r3), Vector{0.5, 0.5});
	Assert(!r1.Intersection(r2).HasValue());
	AssertEQ(ls1.Intersection(r1), Vector{10., 10.});
	AssertEQ(r2.Intersection(l1), Vector{-10., 9.});

	return 0;
}
