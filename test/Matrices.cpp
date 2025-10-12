#include "Strawberry/Core/Math/Matrix.hpp"


using namespace Strawberry::Core;
using namespace Strawberry::Core::Math;


int main()
{
    Matrix<int, 2, 2> m(1, 2,
                        3, 4);
    Assert(m[0][0] == 1);
    Assert(m[1][0] == 2);
    Assert(m[0][1] == 3);
    Assert(m[1][1] == 4);


	Matrix<double, 2, 2> m2(2, 4, 6, 8);
	AssertEQ(m2.Inverse().Unwrap(), Matrix<double, 2, 2>(-1, 1.0 / 2.0, 3.0 / 4.0, - 1.0 / 4.0));
}