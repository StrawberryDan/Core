#include "Strawberry/Core/Math/Clamped.hpp"
#include "Strawberry/Core/Assert.hpp"


using namespace Strawberry::Core;
using namespace Strawberry::Core::Math;


void ClampedNumbers()
{
	// Test clamped number addition
	{
		Clamped<int> a(0, 10, 5);
		Clamped<int> b(0, 10, 5);
		Clamped<int> c = a + b;
		AssertEQ(c, 10);
	}

	// Test addition which overflows bounds
	{
		Clamped<int> a(0, 10, 5);
		Clamped<int> b(0, 10, 6);
		Clamped<int> c = a + b;
		AssertEQ(c, 10);
	}

	// Test subtraction
	{
		Clamped<int> a(0, 10, 5);
		Clamped<int> b(0, 10, 5);
		Clamped<int> c = a - b;
		AssertEQ(c, 0);
	}

	// Test overflowing subtraction
	{
		Clamped<int> a(0, 10, 5);
		Clamped<int> b(0, 10, 6);
		Clamped<int> c = a - b;
		AssertEQ(c, 0);
	}


	// Test overflowing multiplication
	{
		Clamped<int> a(0, 10, 5);
		Clamped<int> b(0, 10, 3);
		Clamped<int> c = a * b;
		AssertEQ(c, 10);
	}

	// Test underflowing division
	{
		Clamped<int> a(3, 10, 5);
		Clamped<int> b(3, 10, 3);
		Clamped<int> c = a / b;
		AssertEQ(c, 3);
	}
}


void StaticClampedNumbers()
{
	// Test clamped number addition
	{
		StaticClamped<int, 0, 10> a(5);
		StaticClamped<int, 0, 10> b(5);
		StaticClamped<int, 0, 10> c = a + b;
		AssertEQ(c, 10);
	}

	// Test addition which overflows bounds
	{
		StaticClamped<int, 0, 10> a(5);
		StaticClamped<int, 0, 10> b(6);
		StaticClamped<int, 0, 10> c = a + b;
		AssertEQ(c, 10);
	}

	// Test subtraction
	{
		StaticClamped<int, 0, 10> a(5);
		StaticClamped<int, 0, 10> b(5);
		StaticClamped<int, 0, 10> c = a - b;
		AssertEQ(c, 0);
	}

	// Test overflowing subtraction
	{
		StaticClamped<int, 0, 10> a(5);
		StaticClamped<int, 0, 10> b(6);
		StaticClamped<int, 0, 10> c = a - b;
		AssertEQ(c, 0);
	}


	// Test overflowing multiplication
	{
		StaticClamped<int, 0, 10> a(5);
		StaticClamped<int, 0, 10> b(3);
		StaticClamped<int, 0, 10> c = a * b;
		AssertEQ(c, 10);
	}

	// Test underflowing division
	{
		StaticClamped<int, 3, 10> a(5);
		StaticClamped<int, 3, 10> b(3);
		StaticClamped<int, 3, 10> c = a / b;
		AssertEQ(c, 3);
	}
}


int main()
{
	ClampedNumbers();
	StaticClampedNumbers();
}