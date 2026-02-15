#include "Strawberry/Core/Math/Geometry/Sphere.hpp"
#include "Strawberry/Core/Math/Vector.hpp"

using namespace Strawberry::Core;
using namespace Strawberry::Core::Math;


int main()
{
	Vector<double, 2> point2DA(1., 1.);
	Sphere<double, 2> sphere2DA({0., 0.}, 1.);
	Sphere<double, 2> sphere2DB({1., 1.}, 1.);
	Assert(!sphere2DA.Contains(point2DA));
	Assert(sphere2DB.Contains(point2DA));
	return 0;
}
