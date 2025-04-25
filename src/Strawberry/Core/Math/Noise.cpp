#include "Noise.hpp"

#include "Math.hpp"


namespace Strawberry::Core::Math::Noise
{
	Linear::Linear(int seed, float period)
			: mSeed(seed)
			, mPeriod(period)
	{}


	float Linear::WhiteIntegerNoise(Vec2i position) const
	{
		// Seed a mersenne twister engine with this pairing combined with the seed.
		std::mt19937_64 engine(mSeed xor position.Embed());
		// Sample the real distribution between -1.0 and 1.0 with this engine.
		std::uniform_real_distribution<float> distribution(-1.0, 1.0);
		float sample = distribution(engine);
		// Return our sample.
		return sample;
	}


	float Linear::operator()(Vec2f position) const noexcept
	{
		// Decompose input coordinates.
		const auto& [x, y] = position;
		// Calculate the index of the wave that the input point is inside.
		const float waveX = std::floor(x / mPeriod);
		const float waveY = std::floor(y / mPeriod);
		// Calculate the phase of the input point relative to the period.
		const float phaseX = x - (waveX * mPeriod);
		const float phaseY = y - (waveY * mPeriod);
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
 		const float a = std::lerp(grid[0][0], grid[0][1], phaseRatioX);
		const float b = std::lerp(grid[1][0], grid[1][1], phaseRatioX);
		const float c = std::lerp(a, b, phaseRatioY);

		return c;
	}


	SmoothLinear::SmoothLinear(int seed, float period)
			: mSeed(seed)
			, mPeriod(period)
	{}


	float SmoothLinear::WhiteIntegerNoise(Vec2i position) const
	{
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


	Perlin::Perlin(int seed, float period)
		: mSeed(seed)
		, mPeriod(period)
	{}

	float Perlin::operator()(Vec2f position) const noexcept
	{
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
			{ VectorNoise(position, Vec2i{waveX + 0, waveY + 0}), VectorNoise(position, Vec2i{waveX + 1, waveY + 0}) },
			{ VectorNoise(position, Vec2i{waveX + 0, waveY + 1}), VectorNoise(position, Vec2i{waveX + 1, waveY + 1}) }
		};
		// Calculate Bilinear interpolation between these points.
		const float a = Smoothstep(grid[0][0], grid[0][1], phaseRatioX);
		const float b = Smoothstep(grid[1][0], grid[1][1], phaseRatioX);
		const float c = Smoothstep(a, b, phaseRatioY);

		Core::Assert(std::isfinite(c));
		return c;
	}

	float Perlin::VectorNoise(Vec2f samplePosition, Vec2i gridPosition) const
	{
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
		return gridVector.Dot(offset.Normalised());
	}
}
