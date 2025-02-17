#include "Strawberry/Core/Math/Matrix.hpp"


using namespace Strawberry::Core;
using namespace Strawberry::Core::Math;


int main()
{
    Matrix<int, 2, 2> m(1,
                        2,
                        3,
                        4);
    Assert(m[0][0] == 1);
    Assert(m[1][0] == 3);
    Assert(m[0][1] == 2);
    Assert(m[1][1] == 4);
}