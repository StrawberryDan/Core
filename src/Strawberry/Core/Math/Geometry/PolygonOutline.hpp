#pragma once
#include "Strawberry/Core/Math/Geometry/LineSegment.hpp"
#include "Strawberry/Core/Math/Geometry/Line.hpp"
#include "Strawberry/Core/Math/Geometry/Ray.hpp"
#include <ranges>
#include <vector>



namespace Strawberry::Core::Math
{
	template <typename T>
	class PolygonOutline
		: public Intersectable
	{
	public:
		template <std::ranges::range Range>
		static PolygonOutline From(Range&& range) requires (std::convertible_to<std::ranges::range_value_t<Range>, Vector<T, 2>>)
		{
			PolygonOutline outline;
			outline.mPoints = std::forward<Range>(range) | std::ranges::to<std::vector>();
			return outline;
		}

		Vector<T, 2> GetPoint(unsigned int index) const noexcept
		{
			Assert(index < mPoints.size());
			return mPoints[index];
		}

		decltype(auto) PointCount() const noexcept
		{
			return mPoints.size();
		}

		const LineSegment<T, 2> GetLine(unsigned int index) const
		{
			Assert(index < mPoints.size());
			return LineSegment<T, 2>(mPoints[index], mPoints[(index + 1) % mPoints.size()]);
		}


		decltype(auto) LineCount() const
		{
			return mPoints.size();
		}


	private:
		PolygonOutline() = default;


		std::vector<Vector<T, 2>> mPoints;
	};


	template <typename T>
	struct IntersectionTest<PolygonOutline<T>, Line<T, 2>>
	{
		using Base = IntersectionTest<LineSegment<T, 2>, Line<T, 2>>::Result::Inner;
		struct Data : Base
		{
			Data(const Base& base)
				: Base(base)
			{}


			unsigned int edgeIndex = 0;
		};

		using Result = std::vector<Data>;

		constexpr Result operator()(const PolygonOutline<T>& a, const Line<T, 2>& b) const noexcept
		{
			Result result;
			result.reserve(2);

			for (int i = 0; i < a.LineCount(); i++)
			{
				const auto& line = a.GetLine(i);

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


	template <typename T>
	struct IntersectionTest<PolygonOutline<T>, Ray<T, 2>>
	{
		using Base = IntersectionTest<LineSegment<T, 2>, Ray<T, 2>>::Result::Inner;
		struct Data : Base
		{
			Data(const Base& base)
				: Base(base)
			{}

			unsigned int edgeIndex = 0;
		};

		using Result = std::vector<Data>;

		constexpr Result operator()(const PolygonOutline<T>& a, const Ray<T, 2>& b) const noexcept
		{
			Result result;
			result.reserve(2);

			for (int i = 0; i < a.LineCount(); i++)
			{
				const auto& line = a.GetLine(i);
				if (auto intersection = line.Intersection(b))
				{
					Data data(std::move(intersection.Unwrap()));
					data.edgeIndex = i;
					result.emplace_back(std::move(data));
				}
			}

			return result;
		}
	};


	template <typename T>
	struct IntersectionTest<PolygonOutline<T>, LineSegment<T, 2>>
	{
		using Base = IntersectionTest<LineSegment<T, 2>, LineSegment<T, 2>>::Result::Inner;

		struct Data : Base
		{
			Data(const Base& base)
				: Base(base)
			{}

			unsigned int edgeIndex = 0;
		};

		using Result = std::vector<Data>;

		constexpr Result operator()(const PolygonOutline<T>& a, const LineSegment<T, 2>& b) const noexcept
		{
			Result result;
			result.reserve(2);

			for (int i = 0; i < a.LineCount(); i++)
			{
				const auto& line = a.GetLine(i);
				if (auto intersection = line.Intersection(b))
				{
					Data data(std::move(intersection.Unwrap()));
					data.edgeIndex = i;
					result.emplace_back(std::move(data));
				}
			}

			return result;
		}
	};
}
