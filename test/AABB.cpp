#include "Strawberry/Core/Math/Geometry/AABB.hpp"

#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Math/Geometry/ConvexPolygon.hpp"

using namespace Strawberry;
using namespace Core;
using namespace Math;

int main()
{
	AABB<double, 2> aabb({0, 0}, {10, 10});


	Assert(aabb.Contains({5, 5}));
	Assert(aabb.Contains({10, 10}));
	Assert(aabb.Contains({0, 0}));
	Assert(!aabb.Contains({15, 5}));
	Assert(!aabb.Contains({15, 15}));
	Assert(!aabb.Contains({-1, -1}));


	auto corners = aabb.Corners();
	AssertEQ(corners.size(), 4);
	AssertEQ(corners[0], aabb.Min());
	AssertEQ(corners[3], aabb.Max());


	auto outline = aabb.AsPolygon();

	Assert(outline.GetLine(0).A().SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(0).B() - Vector{10., 0.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(1).A() - Vector{10., 0.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(1).B() - Vector{10., 10.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(2).A() - Vector{10., 10.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(2).B() - Vector{0., 10.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(3).A() - Vector{0., 10.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(3).B() - Vector{0., 0.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());

	AABB<double, 2> aabb2({-5, -5}, {5, 5});

	auto intersection = aabb.Intersection(aabb2);
	Assert(intersection.HasValue());
	AssertEQ(intersection->overlap.Min(),Vector<double, 2>(0, 0));
	AssertEQ(intersection->overlap.Max(), Vector<double, 2>(5, 5));

	AABB<double, 2> aabb3({20, 20}, {25, 30});
	auto intersection2 = aabb.Intersection(aabb3);
	Assert(!intersection2.HasValue());

	return 0;
}
