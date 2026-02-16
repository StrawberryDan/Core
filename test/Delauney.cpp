#include "Strawberry/Core/Math/Geometry/Delauney.hpp"


using namespace Strawberry::Core;
using namespace Math;

int main()
{
	PointSet<double, 2> pointSet;

	auto delauney = Delauney<double>::FromPoints(pointSet);

	return 0;
}
