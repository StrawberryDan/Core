#include "Strawberry/Core/Math/Geometry/Simplex.hpp"
#include "Strawberry/Core/Math/Geometry/Sphere.hpp"
#include "Strawberry/Core/Assert.hpp"
#include <limits>
using namespace Strawberry::Core;
using namespace Math;

int main()
{
	Triangle<double, 2> s1(
			Vector{0., 0.},
			Vector{1., 0.},
			Vector{0., 1.}
		);

	Assert(s1.Contains(Vector{0.25, 0.25}));
	Assert(!s1.Contains(Vector{0.75, 0.75}));
	Assert(!s1.Contains(Vector{-1., -1.}));


	auto sphere = Sphere<double, 2>::Circumcsphere(s1);
	AssertEQ(sphere->Center(), Vector<double, 2>{0.5, 0.5});
	Assert((sphere->Radius() - std::sqrt(2.0) / 2.0 ) < std::numeric_limits<double>::epsilon());

	return 0;
}
