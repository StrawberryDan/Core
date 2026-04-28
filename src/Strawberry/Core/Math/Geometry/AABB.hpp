#pragma once


#include "ConvexPolygon.hpp"
#include "Ray.hpp"
#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned int D>
	class AABB
		: public Intersectable
	{
	public:
		static AABB FromOriginAndExtent(const Vector<T, D>& origin, const Vector<T, D>& extent)
		{
			return AABB(origin, origin + extent);
		}

		AABB(const Vector<T, D>& min, const Vector<T, D>& max)
			: mMin(min)
			, mMax(max)
		{}


		AABB() = default;


		const auto&  Min() const { return mMin; }
		const auto&  Max() const { return mMax; }
		auto        Size() const { Assert(IsNormal()); return Max() - Min(); }
		auto        Center() const { return (Min() + Max()).Piecewise(std::divides{}, 2); }
		auto        Span() const { Assert(IsNormal()); return Max() - Min();}

		std::vector<Vector<T, D>> Corners() const
		{
			size_t c = 1;
			for (unsigned int d = 0; d < D; d++) c *= 2;

			std::vector<Vector<T, D>> corners;
			corners.reserve(c);

			for (size_t i = 0; i < c; i++)
			{
				auto& corner = corners.emplace_back(mMin);
				for (size_t d = 0; d < D; d++)
				{
					if (i & 1 << d)
					{
						corner[d] = mMax[d];
					}
				}
			}

			return corners;
		}

		void SetMin(const Vector<T, D>& min) noexcept { mMin = min; Assert(IsNormal()); }
		void SetMax(const Vector<T, D>& max) noexcept { mMax = max; Assert(IsNormal());  }


		[[nodiscard]] bool IsNormal() const
		{
			for (unsigned int i = 0; i < D; i++)
			{
				if (mMin[i] > mMax[i])
				{
					return false;
				}
			}
			return true;
		}


		void Normalise()
		{
			for (int i = 0; i < D; i++)
			{
				if (mMin[i] > mMax[i]) [[unlikely]] std::swap(mMin[i], mMax[i]);
			}
		}


		bool Contains(const Vector<T, D>& point) const noexcept
		{
			for (unsigned int i = 0; i < D; i++)
			{
				if (point[i] < mMin[i] || point[i] > mMax[i])
				{
					return false;
				}
			}

			return true;
		}


		/// Returns the edges of the AABB as line segments in CCW order,
		/// starting from the bottom left.
		ConvexPolygon<T> AsPolygon() const noexcept
		{
			Assert(IsNormal());
			return ConvexPolygon<T>::From(std::array{
				mMin,
				Vector{mMax[0], mMin[1]},
				mMax,
				Vector(mMin[0], mMax[1]),
			});
		}

	private:
		Vector<T, D> mMin;
		Vector<T, D> mMax;
	};
}