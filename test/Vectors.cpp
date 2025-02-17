#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Assert.hpp"


using namespace Strawberry::Core;
using namespace Strawberry::Core::Math;


int main()
{
    Vector a(1, 2, 3);
    Vector b(4, 5, 6);

    auto c = a.Dot(b);
    AssertEQ(c, 32);
    auto d = a.Cross(b);
    AssertEQ(d, Vector(2 * 6 - 3 * 5, - 1 * 6 + 4 * 3, 1 * 5 - 2 * 4));

    double e = d.Magnitude();
    float  f = d.Magnitude();

    auto& [x, y, z] = a;
    x += 1;
    AssertEQ(a, Vector(2, 2, 3));
}