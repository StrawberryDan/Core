#pragma once
#include "Matrix.hpp"
#include <cstdint>
#include <random>


namespace Strawberry::Core::Math
{
	// Class representing a signal of white noise interpolated over a given period.
	class LinearNoise
	{
	public:
		// Creates a new noise signal with the given seed and period.
		LinearNoise(uint32_t seed, float period);


		// Returns the value of this noise signal at the given position.
		float operator()(Vec2f position) const noexcept;


	private:
		// Returns a consistent random value in [-1.0, 1.0] for any input coordinate
		float WhiteIntegerNoise(Vec2u position) const;


		// The seed for this signal
		uint32_t mSeed;
		// The orthogonal distance between two white noise values in input space
		float    mPeriod;
	};
}