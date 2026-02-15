#pragma once


#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class Sphere
	{
	public:
		Sphere() = default;
		Sphere(const Vector<T, D>& center, T radius)
			: mCenter(center), mRadius(radius) {}


		      Vector<T, D>& Center()       { return mCenter; }
		const Vector<T, D>& Center() const { return mCenter; }

		double& Radius()       { return mRadius; }
		double  Radius() const { return mRadius; }


		bool Contains(const Vector<T, D>& point) const
		{
			return (point - Center()).SquareMagnitude() < mRadius * mRadius;
		}


	private:
		Vector<T, D> mCenter;
		T            mRadius;
	};
}
