#include "Strawberry/Core/Math/Geometry/Simplex.hpp"
#include "Strawberry/Core/Assert.hpp"

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

	return 0;
}
