#include "Noise.hpp"


namespace Strawberry::Core::Math
{
	LinearNoise::LinearNoise(uint32_t seed, float period)
			: mSeed(seed)
			, mPeriod(period)
	{}


	float LinearNoise::WhiteIntegerNoise(Vec2u position) const
	{
		// Decompose input parameter
		const auto& [x, y] = position;
		// Calculate Cantor Pairing of these integers.
		uint32_t cantor = (x + y) * (x + y + 1) / 2 + y;
		// Seed a mersenne twister engine with this pairing combined with the seed.
		std::mt19937 engine(mSeed xor cantor);
		// Sample the real distribution between -1.0 and 1.0 with this engine.
		std::uniform_real_distribution<float> distribution(-1.0, 1.0);
		float sample = distribution(engine);
		// Return our sample.
		return sample;
	}


	float LinearNoise::operator()(Vec2f position) const noexcept
	{
		// Decompose input coordinates.
		const auto& [x, y] = position;
		// Calculate the index of the wave that the input point is inside.
		const float waveX = std::floor(x / mPeriod);
		const float waveY = std::floor(y / mPeriod);
		// Calculate the phase of the input point relative to the period.
		const float phaseX = std::fmod(x, mPeriod);
		const float phaseY = std::fmod(y, mPeriod);
		// Calculate the ratio between the phase and the period
		const float phaseRatioX = phaseX / mPeriod;
		const float phaseRatioY = phaseY / mPeriod;
		// Calculate the four noise corner values.
		const float grid[2][2]
		{
			{ WhiteIntegerNoise({waveX + 0, waveY + 0}), WhiteIntegerNoise({waveX + 1, waveY + 0}) },
			{ WhiteIntegerNoise({waveX + 0, waveY + 1}), WhiteIntegerNoise({waveX + 1, waveY + 1}) }
		};
		// Calculate Bilinear interpolation between these points.
 		const float a = std::lerp(grid[0][0], grid[0][1], phaseRatioX);
		const float b = std::lerp(grid[1][0], grid[1][1], phaseRatioX);
		const float c = std::lerp(a, b, phaseRatioY);

		return c;
	}
}