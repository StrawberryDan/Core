#include "Strawberry/Core/Types/Optional.hpp"


using namespace Strawberry;


int main()
{
	Core::Optional<int> a = 0;
	Core::Optional<int> b = 1;
	Core::Optional<int> c = 0;
	Core::Optional<int> d = Core::NullOpt;

	Core::AssertEQ(a, 0);
	Core::AssertEQ(b, 1);
	Core::AssertNEQ(a, 1);
	Core::AssertNEQ(b, 0);

	Core::AssertNEQ(a, b);
	Core::AssertEQ(a, c);
	Core::AssertNEQ(a, d);
}
