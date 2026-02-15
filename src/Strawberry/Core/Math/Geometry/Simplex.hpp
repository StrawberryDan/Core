#pragma once


#include "Strawberry/Core/Math/Geometry/Plane.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include <array>


namespace Strawberry::Core::Math
{
	template <typename T, unsigned Dimension, unsigned Order>
	class Simplex
	{
	public:
		Simplex() = default;
		template <typename... Args>
		Simplex(Args&&... args)
			: mPoints{std::forward<Args>(args)...} {}
		Simplex(std::array<Vector<T, Dimension>, Order> points);


		Vector<T, Dimension>& Point(unsigned index) { return mPoints[index]; }
		const Vector<T, Dimension>& Point(unsigned index) const { return mPoints[index]; }

		bool Contains(const Vector<T, Dimension>& point) const requires (Dimension == 2 && Order == 3)
		{
			if ((Point(1) - Point(0)).Perpendicular().Dot(point - Point(0)) < 0) [[likely]] return false;
			if ((Point(2) - Point(1)).Perpendicular().Dot(point - Point(1)) < 0) [[likely]] return false;
			if ((Point(0) - Point(2)).Perpendicular().Dot(point - Point(2)) < 0) [[likely]] return false;
			return true;
		}


		Plane<T, Dimension> IntoPlane() const requires (Order == 3)
		{
			return Plane<T, Dimension>::FromTriangle(mPoints[0], mPoints[1], mPoints[2]);
		}


	private:
		std::array<Vector<T, Dimension>, Order> mPoints;
	};


	template <typename T, unsigned Dimension>
	using Triangle = Simplex<T, Dimension, 3>;
}
