#include "Strawberry/Core/Math/Periodic.hpp"
#include "Strawberry/Core/Assert.hpp"


using namespace Strawberry::Core;
using namespace Strawberry::Core::Math;


int main()
{
    Periodic<unsigned int, 10> unsignedInt = 5;
    AssertEQ(unsignedInt + 10,  5);
    AssertEQ(unsignedInt + 14,  9);
    AssertEQ(unsignedInt - 6,  9);
    AssertEQ(unsignedInt - 10,  5);
    AssertEQ(unsignedInt - 15,  0);
    AssertEQ(unsignedInt * 2,  0);
    AssertEQ(unsignedInt * 3,  5);
    AssertEQ(unsignedInt * 4,  0);
    AssertEQ(unsignedInt / 2,  2);

    DynamicPeriodic<unsigned int> dynamicUnsignedInt(10, 5);
    AssertEQ(dynamicUnsignedInt + 10,  5);
    AssertEQ(dynamicUnsignedInt + 14,  9);
    AssertEQ(dynamicUnsignedInt - 6,  9);
    AssertEQ(dynamicUnsignedInt - 10,  5);
    AssertEQ(dynamicUnsignedInt - 15,  0);
    AssertEQ(dynamicUnsignedInt * 2,  0);
    AssertEQ(dynamicUnsignedInt * 3,  5);
    AssertEQ(dynamicUnsignedInt * 4,  0);
    AssertEQ(dynamicUnsignedInt / 2,  2);

    DynamicPeriodic<double> dynamicDouble(10, 5);
    AssertEQ(dynamicDouble + 10,  5);
    AssertEQ(dynamicDouble + 14,  9);
    AssertEQ(dynamicDouble - 6,  9);
    AssertEQ(dynamicDouble - 10,  5);
    AssertEQ(dynamicDouble - 15,  0);
    AssertEQ(dynamicDouble * 2,  0);
    AssertEQ(dynamicDouble * 3,  5);
    AssertEQ(dynamicDouble * 4,  0);
    AssertEQ(dynamicDouble / 2,  2.5);
}