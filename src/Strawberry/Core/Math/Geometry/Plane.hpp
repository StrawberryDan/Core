#pragma once


#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Math/Geometry/Simplex.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class Plane
	{
	public:
		/// Returns the plane corresponding to the triangle formed by 3 counter-clockwise points.
		static Plane FromTriangle(const Triangle<T, D>& triangle)
		{
			return FromTriangle(triangle.Point(0), triangle.Point(1), triangle.Point(2));
		}


		static Plane FromTriangle(const Vector<T, D>& a, const Vector<T, D>& b, const Vector<T, D>& c)
		{
			Vector<T, D> normal = (b - a).Cross(c - a).Normalised();
			T dotProduct = normal.Dot(a);
			return Plane(normal, dotProduct);
		}


		Plane(const Vector<T, D>& normal, T dotProduct)
			: mNormal(normal) , mDotProduct(dotProduct) {}


	private:
		Vector<T, D> mNormal;
		T            mDotProduct;
	};
}
