#include "Strawberry/Core/Math/Geometry/AABB.hpp"

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


	auto outline = aabb.GetOutline();

	Assert(outline.GetLine(0).A().SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(0).B() - Vector{10., 0.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(1).A() - Vector{10., 0.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(1).B() - Vector{10., 10.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(2).A() - Vector{10., 10.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(2).B() - Vector{0., 10.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(3).A() - Vector{0., 10.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());
	Assert((outline.GetLine(3).B() - Vector{0., 0.}).SquareMagnitude() < std::numeric_limits<double>::epsilon());

	return 0;
}