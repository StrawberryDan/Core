#pragma once


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
		auto        Size() const { return Max() - Min(); }
		auto        Center() const { return (Min() + Max()).Piecewise(std::divides{}, 2); }

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


		/// Returns the edges of the AABB as line segments in CCW order.
		std::array<LineSegment<T, D>, 4> AsLineSegments() const noexcept
		{
			return {
				LineSegment<T, D>(mMin, Vector{mMax[0], mMin[1]}),
				LineSegment<T, D>(Vector(mMax[0], mMin[1]), mMax),
				LineSegment<T, D>(mMax, Vector(mMin[0], mMax[1])),
				LineSegment<T, D>(Vector(mMin[0], mMax[1]), mMin)
			};
		}

	private:
		Vector<T, D> mMin;
		Vector<T, D> mMax;
	};


	template <typename T, unsigned int D>
	struct IntersectionTest<AABB<T, D>, Line<T, D>>
	{
		struct Data
			: IntersectionTest<LineSegment<T, D>, Line<T, D>>::Result::Inner
		{
			using IntersectionTest<LineSegment<T, D>, Line<T, D>>::Result::Inner::Inner;


			Data(const IntersectionTest<LineSegment<T, D>, Line<T, D>>::Result::Inner& inner)
				: IntersectionTest<LineSegment<T, D>, Line<T, D>>::Result::Inner(inner)
				, edgeIndex(0)
			{}


			unsigned int edgeIndex;
		};

		using Result = std::vector<Data>;

		constexpr Result operator()(const AABB<T, D>& a, const Line<T, D>& b) const noexcept
		{
			Result result;
			result.reserve(2);

			auto segments = a.AsLineSegments();
			for (int i = 0; i < 4; i++)
			{
				const auto& line = segments[i];

				if (auto intersection = line.Intersection(b))
				{
					Data data(intersection.Unwrap());
					data.edgeIndex = i;
					result.emplace_back(std::move(data));
				}
			}

			return result;
		}
	};


	template <typename T, unsigned int D>
	struct IntersectionTest<AABB<T, D>, Ray<T, D>>
	{
		using Result = std::vector<typename IntersectionTest<LineSegment<T, D>, Ray<T, D>>::Result::Inner>;

		constexpr Result operator()(const AABB<T, D>& a, const Ray<T, D>& b) const noexcept
		{
			Result result;
			result.reserve(2);

			for (const auto& line : a.AsLineSegments())
			{
				if (auto intersection = line.Intersection(b))
				{
					auto pos = std::ranges::lower_bound(result, std::less{}, [] (const auto& x) { return x.rayDistance;});
					result.emplace(pos, std::move(intersection.Unwrap()));
				}
			}

			return result;
		}
	};


	template <typename T, unsigned int D>
	struct IntersectionTest<AABB<T, D>, LineSegment<T, D>>
	{
		using Result = std::vector<typename IntersectionTest<LineSegment<T, D>, LineSegment<T, D>>::Result::Inner>;

		constexpr Result operator()(const AABB<T, D>& a, const LineSegment<T, D>& b) const noexcept
		{
			Result result;
			result.reserve(2);

			for (const auto& line : a.AsLineSegments())
			{
				if (auto intersection = b.Intersection(line))
				{
					auto pos = std::ranges::lower_bound(result, std::less{}, [] (const auto& x) { return x.segmentDistance[0]; });
					result.emplace(pos, std::move(intersection.Unwrap()));
				}
			}

			return result;
		}
	};
}