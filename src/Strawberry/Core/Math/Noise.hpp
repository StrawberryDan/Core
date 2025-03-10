#pragma once
#include "Matrix.hpp"
#include <cstdint>
#include <random>


namespace Strawberry::Core::Math
{
	class LinearNoise
	{
	public:
		LinearNoise(uint32_t seed, float period);

	private:
		float Noise(Core::Math::Vec2u position) const;

	public:
		float operator()(Vec2f position) const noexcept;


	private:
		uint32_t mSeed;
		float    mPeriod;
	};
}