#include "Strawberry/Core/Math/Geometry/Line.hpp"


using namespace Strawberry::Core;
using namespace Math;


int main()
{
	Line<double, 2> lineA({0.0, 0.0}, {1.0, 1.0});
	Line<double, 2> lineB({0.0, 0.0}, {1.0, 0.0});
	Line<double, 2> lineC({2.0, 0.0}, {0.0, 2.0});

	Logging::Info("Testing line {} against {}", lineA, lineB);
	auto intA = lineA.Intersection(lineB);
	Assert(intA.HasValue());
	AssertEQ(intA.Value(), Vector{0.0, 0.0});


	Logging::Info("Testing line {} against {}", lineA, lineC);
	auto intB = lineA.Intersection(lineC);
	Assert(intB.HasValue());
	AssertEQ(intB.Value(), Vector{1.0, 1.0});

	return 0;
}
