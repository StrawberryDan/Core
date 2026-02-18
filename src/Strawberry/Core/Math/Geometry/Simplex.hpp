#pragma once


#include "Strawberry/Core/Math/Geometry/Plane.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include <array>


namespace Strawberry::Core::Math
{
	template <typename T, unsigned Dimension, unsigned Order> requires (Order <= Dimension + 1)
	class Simplex
	{
	public:
		Simplex() = default;
		template <typename... Args>
		Simplex(Args&&... args)
			: mPoints{std::forward<Args>(args)...}
		{
			if constexpr (Dimension == 2 && Order == 3)
			{
				MakeCounterClockwise();
			}
		}


		Vector<T, Dimension>& Point(unsigned index) { return mPoints[index]; }
		const Vector<T, Dimension>& Point(unsigned index) const { return mPoints[index]; }

		bool Contains(const Vector<T, Dimension>& point) const requires (Dimension == 2 && Order == 3)
		{
			if ((Point(1) - Point(0)).Perpendicular().Dot(point - Point(0)) < 0) [[likely]] return false;
			if ((Point(2) - Point(1)).Perpendicular().Dot(point - Point(1)) < 0) [[likely]] return false;
			if ((Point(0) - Point(2)).Perpendicular().Dot(point - Point(2)) < 0) [[likely]] return false;
			return true;
		}


		bool IsCounterClockwise() const requires (Dimension == 2 && Order == 3)
		{
			return (Point(2) - Point(0)).Dot((Point(1) - Point(0)).Perpendicular()) > 0;
		}


		void MakeCounterClockwise() requires (Dimension == 2 && Order == 3)
		{
			if (!IsCounterClockwise())
			{
				std::swap(Point(1), Point(2));
			}

			Core::Assert(IsCounterClockwise());
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
