#include "Strawberry/Core/Math/Checked.hpp"
#include "Strawberry/Core/Assert.hpp"
#include <cstdint>


using namespace Strawberry::Core;
using namespace Math;


int main()
{
    auto a = CheckedAddition<uint32_t>(10, 5);
    AssertEQ(a.value, 15);
    AssertEQ(a.overflow, false);

    auto b = CheckedAddition<uint8_t>(255, 1);
    AssertEQ(b.value, 0);
    AssertEQ(b.overflow, true);

    auto c = CheckedAddition<uint8_t>(254, 4);
    AssertEQ(c.value, 2);
    AssertEQ(c.overflow, true);

    auto d = CheckedAddition<uint8_t>(255, 255);
    AssertEQ(d.value, 254);
    AssertEQ(d.overflow, true);


    auto e = CheckedSubtraction<uint8_t>(0, 1);
    AssertEQ(e.value, 255);
    AssertEQ(e.overflow, true);

    auto f = CheckedMultiplication<uint8_t>(255, 2);
    Logging::Info("{}", f.value);    
    AssertEQ(f.value, 254);
    AssertEQ(f.overflow, true);    
    
    return 0;
}

