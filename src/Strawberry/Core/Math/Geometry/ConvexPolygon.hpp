#pragma once


#include <concepts>
#include <vector>
#include "Ray.hpp"
#include "Intersection.hpp"
#include "Simplex.hpp"
#include "AABB.hpp"
#include "PointSet.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned int D>
	class PointSet;

	template <typename T>
	class ConvexPolygon
		: public Intersectable
	{
	public:
		ConvexPolygon() = default;


		template <std::ranges::range Range> requires (std::convertible_to<std::ranges::range_value_t<Range>, Vector<T, 2>>)
		static ConvexPolygon From(Range&& range) noexcept
		{
			auto outline = range | std::views::transform([] (const auto& x) { return Vector<T, 2>(x); });
			ConvexPolygon polygon;
			polygon.mPoints = outline | std::ranges::to<std::vector>();
			return polygon;
		}


		Vector<T, 2> GetPoint(unsigned int index) const noexcept
		{
			Assert(index < mPoints.size());
			return mPoints[index];
		}

		decltype(auto) Points() const
		{
			return mPoints | std::views::all;
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


		Vector<T, 2> CenterOfMass() const
		{
			std::vector<std::pair<double, Vector<T, 2>>> components;

			auto mean = Mean();
			for (int i = 0; i < LineCount(); i++)
			{
				auto           line = GetLine(i);
				Triangle<T, 2> tri({line.A(), line.B(), mean});
				components.emplace_back(tri.Area(), tri.GetMean());
			}

			Vector<T, 2> center;
			double totalWeight = 0.0;
			for (const auto& [weight, triMean] : components)
			{
				center = center + (weight * triMean);
				totalWeight += weight;
			}
			return (1.0 / totalWeight) * center;
		}


		Vector<T, 2> Mean() const
		{
			Vector<T, 2> mean;

			for (int i = 0; i < PointCount(); i++)
			{
				mean = mean + GetPoint(i);
			}

			return mean * (1.0 / PointCount());
		}


		[[nodiscard]] AABB<T, 2> GetBoundingBox() const noexcept
		{
			PointSet<T, 2> points;
			for (auto p : mPoints)
			{
				points.Add(p);
			}

			return AABB<T, 2>{points.MinExtreme(), points.MaxExtreme()};
		}


		bool Contains(const Vector<T, 2>& v) const noexcept
		{
			auto boundingBox = GetBoundingBox();
			if (!boundingBox.Contains(v))
			{
				return false;
			}

			for (int i = 0; i < LineCount(); i++)
			{
				auto line = GetLine(i);
				if ((line.B() - line.A()).DotPerp(v - line.A()) < 0.0)
				{
					return false;
				}
			}

			return true;
		}


	private:


		bool IsConvex() const noexcept
		{
			for (int i = 0; i < mPoints.size(); i++)
			{
				const auto& a = mPoints[(i + 0) % mPoints.size()];
				const auto& b = mPoints[(i + 1) % mPoints.size()];
				const auto& c = mPoints[(i + 2) % mPoints.size()];

				if ((b - a).DotPerp(c - a) < 0.0)
				{
					return false;
				}
			}

			return true;
		}


		std::vector<Vector<T, 2>> mPoints;
	};


	template <typename T>
	struct IntersectionTest<ConvexPolygon<T>, Line<T, 2>>
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

		constexpr Result operator()(const ConvexPolygon<T>& a, const Line<T, 2>& b) const noexcept
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
	struct IntersectionTest<ConvexPolygon<T>, Ray<T, 2>>
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

		constexpr Result operator()(const ConvexPolygon<T>& a, const Ray<T, 2>& b) const noexcept
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
	struct IntersectionTest<ConvexPolygon<T>, LineSegment<T, 2>>
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

		constexpr Result operator()(const ConvexPolygon<T>& a, const LineSegment<T, 2>& b) const noexcept
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
