#pragma once


#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class Ray
	{
	public:
		Ray() = default;
		Ray(const Vector<T, D>& origin, const Vector<T, D>& direction)
			: mOrigin(origin), mDirection(direction) {}


		Vector<T, D>& Origin() { return mOrigin; }
		const Vector<T, D>& Origin() const { return mOrigin; }


		Vector<T, D>& Direction() { return mDirection; }
		const Vector<T, D>& Direction() const { return mDirection; }


	private:
		Vector<T, D> mOrigin;
		Vector<T, D> mDirection;
	};
}
