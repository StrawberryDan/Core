// Strawberry Core
#include "Strawberry/Core/Math/Noise/SmoothLinear.hpp"
#include "Strawberry/Core/Math/Math.hpp"
// Standard Library
#include <random>


namespace Strawberry::Core::Math::Noise
{
	SmoothLinear::SmoothLinear(uint64_t seed, float period)
		: mSeed(seed)
		, mPeriod(period)
	{}


	float SmoothLinear::WhiteIntegerNoise(Vec2i position) const
	{
		ZoneScoped;

		// Seed a mersenne twister engine with this pairing combined with the seed.
		std::mt19937_64 engine(mSeed xor position.Embed());
		// Sample the real distribution between -1.0 and 1.0 with this engine.
		std::uniform_real_distribution<float> distribution(-1.0, 1.0);
		float sample = distribution(engine);
		// Return our sample.
		return sample;
	}


	float SmoothLinear::operator()(Vec2f position) const noexcept
	{
		ZoneScoped;

		// Decompose input coordinates.
		const auto& [x, y] = position;
		// Calculate the index of the wave that the input point is inside.
		const float waveX = std::floor(x / mPeriod);
		const float waveY = std::floor(y / mPeriod);
		// Calculate the phase of the input point relative to the period.
		const float phaseX = waveX >= 0 ? std::fmod(x, mPeriod) : mPeriod + std::fmod(x, mPeriod);
		const float phaseY = waveY >= 0 ? std::fmod(y, mPeriod) : mPeriod + std::fmod(y, mPeriod);
		// Calculate the ratio between the phase and the period
		const float phaseRatioX = phaseX / mPeriod;
		const float phaseRatioY = phaseY / mPeriod;
		// Calculate the four noise corner values.
		const float grid[2][2]
			{
				{ WhiteIntegerNoise(Vec2i{waveX + 0, waveY + 0}), WhiteIntegerNoise(Vec2i{waveX + 1, waveY + 0}) },
				{ WhiteIntegerNoise(Vec2i{waveX + 0, waveY + 1}), WhiteIntegerNoise(Vec2i{waveX + 1, waveY + 1}) }
			};
		// Calculate Bilinear interpolation between these points.
		const float a = Smoothstep(grid[0][0], grid[0][1], phaseRatioX);
		const float b = Smoothstep(grid[1][0], grid[1][1], phaseRatioX);
		const float c = Smoothstep(a, b, phaseRatioY);

		return c;
	}



}
