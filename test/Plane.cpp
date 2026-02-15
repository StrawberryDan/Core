#include "Strawberry/Core/Math/Geometry/Plane.hpp"


using namespace Strawberry::Core;
using namespace Math;

int main()
{
	Plane<double, 2> p1({1, 1}, 0);
	Assert(p1.SignedDistance({1, 1}) > 0.);
	Assert(p1.SignedDistance({-1, -1}) < 0);
	return 0;
}
