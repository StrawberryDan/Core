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


    AssertEQ(Vec2i(0, 0).Embed(), 0);
    AssertEQ(Vec2i(1, 0).Embed(), 1);
    AssertEQ(Vec2i(1, 1).Embed(), 2);
    AssertEQ(Vec2i(0, 1).Embed(), 3);
    AssertEQ(Vec2i(-1, 1).Embed(), 4);
    AssertEQ(Vec2i(-1, 0).Embed(), 5);
    AssertEQ(Vec2i(-1, -1).Embed(), 6);
    AssertEQ(Vec2i(0, -1).Embed(), 7);
    AssertEQ(Vec2i(1, -1).Embed(), 8);
    AssertEQ(Vec2i(2, 0).Embed(), 9);
    AssertEQ(Vec2i(2, 1).Embed(), 10);
    AssertEQ(Vec2i(2, 2).Embed(), 11);
    AssertEQ(Vec2i(1, 2).Embed(), 12);
    AssertEQ(Vec2i(0, 2).Embed(), 13);
    AssertEQ(Vec2i(-1, 2).Embed(), 14);
    AssertEQ(Vec2i(-2, 2).Embed(), 15);
    AssertEQ(Vec2i(-2, 1).Embed(), 16);
    AssertEQ(Vec2i(-2, 0).Embed(), 17);
    AssertEQ(Vec2i(-2, -1).Embed(), 18);
    AssertEQ(Vec2i(-2, -2).Embed(), 19);
    AssertEQ(Vec2i(-1, -2).Embed(), 20);
    AssertEQ(Vec2i(-0, -2).Embed(), 21);
    AssertEQ(Vec2i(1, -2).Embed(), 22);
    AssertEQ(Vec2i(2, -2).Embed(), 23);
    AssertEQ(Vec2i(2, -1).Embed(), 24);
}