#pragma once


#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math::Noise
{
	class Perlin
	{
	public:
		// Creates a new noise signal with the given seed and period.
		Perlin(uint64_t seed, float period);


		float Amplitude() const { return 1.0f; }


		// Returns the value of this noise signal at the given position.
		float operator()(Vec2f position) const noexcept;


	private:
		// Returns a consistent random value in [-1.0, 1.0] for any input coordinate
		float VectorNoise(Vec2f samplePosition, Vec2i gridPosition) const;


		// The seed for this signal
		uint64_t mSeed;
		// The orthogonal distance between two white noise values in input space
		float    mPeriod;
	};

}
