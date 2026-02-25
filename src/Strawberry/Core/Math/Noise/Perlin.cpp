// Strawberry Core
#include "Strawberry/Core/Math/Noise/Perlin.hpp"
#include "Strawberry/Core/Math/Math.hpp"
// Standard Library
#include <random>


namespace Strawberry::Core::Math::Noise
{
	Perlin::Perlin(uint64_t seed, float period)
		: mSeed(seed)
		, mPeriod(period)
	{}


	float Perlin::operator()(Vec2f position) const noexcept
	{
		ZoneScoped;

		// Decompose input coordinates.
		const auto& [x, y] = position;
		// Calculate the index of the wave that the input point is inside.
		const float waveX = std::floor(x / mPeriod);
		const float waveY = std::floor(y / mPeriod);
		// Calculate the phase of the input point relative to the period.
		const float phaseX = x - waveX * mPeriod;
		const float phaseY = y - waveY * mPeriod;
		// Calculate the ratio between the phase and the period
		const float phaseRatioX = phaseX / mPeriod;
		const float phaseRatioY = phaseY / mPeriod;
		// Calculate the four noise corner values.
		const float grid[2][2]
			{
				{ VectorNoise(position, Vec2i{waveX + 0, waveY + 0}), VectorNoise(position, Vec2i{waveX + 1, waveY + 0}) },
				{ VectorNoise(position, Vec2i{waveX + 0, waveY + 1}), VectorNoise(position, Vec2i{waveX + 1, waveY + 1}) }
			};
		// Calculate Bilinear interpolation between these points.
		const float a = Smoothstep(grid[0][0], grid[0][1], phaseRatioX);
		const float b = Smoothstep(grid[1][0], grid[1][1], phaseRatioX);
		const float c = Smoothstep(a, b, phaseRatioY);

		return std::isfinite(c) ? c : 0.0f;
	}

	float Perlin::VectorNoise(Vec2f samplePosition, Vec2i gridPosition) const
	{
		ZoneScoped;

		// Seed engine with embedding
		std::mt19937_64 engine(mSeed xor gridPosition.Embed());
		// Sample the real distribution between 0 and 2.0 * M_PI with this engine.
		std::uniform_real_distribution<float> distribution(0, 2.0 * M_PI);
		// Sample orientation
		float orientation = distribution(engine);
		// Create vector
		Vec2f gridVector(std::sin(orientation), std::cos(orientation));
		// Calculate offset of sample point
		Vec2f offset = samplePosition - gridPosition.AsType<float>();
		// Calculate normalised dot product
		float result = gridVector.Dot(offset.Normalised());
		return std::isfinite(result) ? result : 0.0f;
	}

}
