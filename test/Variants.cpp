#include "Strawberry/Core/Types/Variant.hpp"


using namespace Strawberry::Core;


int main()
{
    using V1 = Variant<std::string, int, uint8_t>;
    V1 a = std::string();
    V1 b = int(2);
    V1 c = uint8_t(2);

    Assert(a.IsType<std::string>());
    Assert(b.IsType<int>());
    Assert(c.IsType<uint8_t>());

    Assert(a == "");
    Assert(b == 2);
    Assert(c == 2);

    Assert(b == c);
    // Assert(c == b);
    // Assert(a != b);
}