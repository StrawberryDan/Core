#include "Strawberry/Core/Math/Noise.hpp"

int main()
{
	Strawberry::Core::Math::Noise::Linear noise(0, 10);

	auto a = noise({0, 0});
	auto b = noise({-1, 0});
	auto c = noise({-0, 0});
	auto d = noise({10, 0});

	return 0;
}
