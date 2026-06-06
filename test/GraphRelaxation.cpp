#include "Strawberry/Core/Math/Geometry/PointSet.hpp"

using namespace Strawberry::Core::Math;

int main()
{
	static constexpr unsigned int POINT_COUNT = 3;
	static constexpr unsigned int ITERATION_COUNT = 1024;


	AABB<float, 2> box({-1000, -1000}, {1000, 1000});
	PointSet pointSet = PointSet<float, 2>::UniformDistribution(POINT_COUNT, box);

	PointSet relaxed = pointSet.Relaxed(box.AsPolygon(), ITERATION_COUNT);

	return 0;
}
