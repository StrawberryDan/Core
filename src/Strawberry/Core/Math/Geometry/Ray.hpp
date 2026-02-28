#pragma once

#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Math/Geometry/Line.hpp"
#include "Strawberry/Core/Math/Geometry/LineSegment.hpp"
#include "Strawberry/Core/Types/Optional.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class Ray
	{
	public:
		Ray() = default;

		Ray(const Vector<T, D>& origin, const Vector<T, D>& direction)
			: mOrigin(origin)
		{
			SetDirection(direction);
		}

		/// Create a Ray starting at line.A(), and going in the same direction.
		explicit Ray(const Line<T, D>& line)
			: mOrigin(line.A())
		{
			SetDirection(line.Direction());
		}

		/// Construct a Ray starting at point A of the line segment, and going in its direction.
		explicit Ray(const LineSegment<T, D>& lineSegment)
			: mOrigin(lineSegment.A())
		{
			SetDirection(lineSegment.Direction());
		}


		Vector<T, D>& Origin() { return mOrigin; }
		const Vector<T, D>& Origin() const { return mOrigin; }


		void SetDirection(const Vector<T, D>& direction) { mDirection = direction.Normalised(); }
		const Vector<T, D>& Direction() const { return mDirection; }


		/// Transforms this Ray into a Line that is parallel.
		Line<T, D> IntoLine() const
		{
			return Line(mOrigin, mOrigin + mDirection);
		}


		Optional<Vector<T, D>> Intersection(const Line<T, D>& other) const noexcept
		{
			// Solve using Cramer's rule
			auto& p1 = this->Origin();
			auto& p2 = other.A();
			auto   c = p2 - p1;
			auto& v1 = this->Direction();
			auto  v2 = other.Direction();

			double determinant = v1[0] * v2[1] - v2[0] * v1[1];
			if (determinant == 0.0)
			{
				return NullOpt;
			}

			auto t1Num = c[0] * v2[1] - v2[0] * c[1];
			auto t1 = t1Num / determinant;
			auto t2Num = -(v1[0] * c[1] - c[0] * v1[1]);
			auto t2 = t2Num / determinant;

			if (t1 < 0.0)
			{
				return NullOpt;
			}

			return p1 + t1 * v1;
		}


		Optional<Vector<T, D>> Intersection(const LineSegment<T, D>& other) const noexcept
		{
			// Solve using Cramer's rule
			auto& p1 = this->Origin();
			auto& p2 = other.A();
			auto   c = p2 - p1;
			auto& v1 = this->Direction();
			auto  v2 = other.Direction();

			double determinant = v1[0] * v2[1] - v2[0] * v1[1];
			if (determinant == 0.0)
			{
				return NullOpt;
			}

			auto t1Num = c[0] * v2[1] - v2[0] * c[1];
			auto t1 = t1Num / determinant;
			auto t2Num = -(v1[0] * c[1] - c[0] * v1[1]);
			auto t2 = t2Num / determinant;

			if (t1 < 0.0 || t2 < 0.0 || t2 > 1.0)
			{
				return NullOpt;
			}

			return p1 + t1 * v1;
		}


		Optional<Vector<T, D>> Intersection(const Ray& other) const noexcept
		{
			// Solve using Cramer's rule
			auto& p1 = this->Origin();
			auto& p2 = other.Origin();
			auto   c = p2 - p1;
			auto& v1 = this->Direction();
			auto& v2 = other.Direction();

			double determinant = v1[0] * v2[1] - v2[0] * v1[1];
			if (determinant == 0.0)
			{
				return NullOpt;
			}

			auto t1Num = c[0] * v2[1] - v2[0] * c[1];
			auto t1 = t1Num / determinant;
			auto t2Num = -(v1[0] * c[1] - c[0] * v1[1]);
			auto t2 = t2Num / determinant;

			if (t1 < 0.0 || t2 < 0.0)
			{
				return NullOpt;
			}

			return p1 + t1 * v1;
		}


		std::string ToString() const noexcept
		{
			return fmt::format("Ray(from {} direction {})", Origin().ToString(), Direction().ToString());
		}


	private:
		Vector<T, D> mOrigin;
		Vector<T, D> mDirection;
	};

}
