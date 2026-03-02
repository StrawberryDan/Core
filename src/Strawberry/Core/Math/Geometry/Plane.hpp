#pragma once


#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class Plane
	{
	public:
		/// Returns the plane corresponding to the triangle formed by 3 counter-clockwise points.
		static Plane FromTriangle(const Vector<T, D>& a, const Vector<T, D>& b, const Vector<T, D>& c)
		{
			Vector<T, D> normal = (b - a).Cross(c - a).Normalised();
			T dotProduct = normal.Dot(a);
			return Plane(normal, dotProduct);
		}

		/// Returns the plane defined by a point and a normal vector.
		static Plane FromNormalAndPoint(const Vector<T, D>& normal, const Vector<T, D>& point)
		{
			normal = normal.Normalised();
			return Plane(normal, normal.Dot(point));
		}


		Plane(const Vector<T, D>& normal, T dotProduct)
			: mNormal(normal.Normalised())
			, mDotProduct(dotProduct) {}


		T SignedDistance(const Vector<T, D>& point) const requires (std::floating_point<T>)
		{
			return mNormal.Dot(point) - mDotProduct;
		}


	private:
		Vector<T, D> mNormal;
		T            mDotProduct;
	};
}
