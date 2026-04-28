#pragma once
// Strawberry Core
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Math/Graph/Voronoi.hpp"
#include "Strawberry/Core/Math/Graph/Delauney.hpp"
// Standard Library
#include <algorithm>
#include <random>
#include <set>


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class PointSet
	{
	public:
		static PointSet UniformDistribution(unsigned int count, const AABB<T, 2> aabb)
		{
			std::default_random_engine rng;
			rng.seed(std::random_device{}());

			std::array<std::uniform_real_distribution<T>, D> distibutions;
			for (int d  = 0; d < D; d++)
			{
				distibutions[d] = std::uniform_real_distribution<T>(aabb.Min()[d], aabb.Max()[d]);
			}

			PointSet points;
			for (int i = 0; i < count; i++)
			{
				Vector<T, D> v;
				for (int d = 0; d < D; d++)
				{
					v[d] = distibutions[d](rng);
				}
				points.Add(v);
			}
			return points;
		}


		static PointSet UniformDistribution(unsigned int count, const Triangle<T, 2> simplex) requires (D == 2)
		{
			std::default_random_engine rng;
			rng.seed(std::random_device{}());

			PointSet set;
			for (int pointIndex = 0; pointIndex < count; pointIndex++)
			{
				static std::uniform_real_distribution<T> dist(0.0, 1.0);

				double r1 = dist(rng);
				double r2 = dist(rng);

				set.Add(
					(1 - std::sqrt(r1)) * simplex.Point(0) +
					std::sqrt(r1) * (1 - r2) * simplex.Point(1) +
					std::sqrt(r1) * r2 * simplex.Point(2));
			}
			return set;
		}


		PointSet() = default;


		const Vector<T, D> Get(unsigned int i) const { return mPoints[i]; }
		Vector<T, D>& Get(unsigned int i) { return mPoints[i]; }


		auto begin() const noexcept { return mPoints.cbegin(); }
		auto end() const noexcept { return mPoints.cend(); }


		unsigned int Size() const { return mPoints.size(); }


		void Add(const Vector<T, D>& point)
		{
			mPoints.insert(point);
		}


		Vector<T, D> MinExtreme() const
		{
			Vector<T, D> min;
			for (auto point : mPoints)
			{
				for (int d = 0; d < D; d++)
				{
					min[d] = std::min(min[d], point[d]);
				}
			}
			return min;
		}


		Vector<T, D> MaxExtreme() const
		{
			Vector<T, D> max;
			for (auto point : mPoints)
			{
				for (int d = 0; d < D; d++)
				{
					max[d] = std::max(max[d], point[d]);
				}
			}
			return max;
		}


		PointSet Relaxed(const ConvexPolygon<T>& bounds, unsigned int iterationCount = 1, double strength = 1.0)
		{
			PointSet result = *this;
			for (int i = 0; i < iterationCount; i++)
			{
				auto delaunay = typename Delaunay<Vector<T, 2>>::Builder(bounds)
					.WithNodes(result)
					.Build();
				auto voronoi = typename Voronoi<Vector<T, 2>>::Builder(delaunay).Build();

				PointSet next;
				for (auto node : delaunay.GetGraph().NodeIndices())
				{
					auto p0 = delaunay.GetGraph().GetValue(node);
					auto cell = voronoi.GetCell(node);
					auto point = strength * voronoi.GetCellAsPolygon(cell).CenterOfMass()
						+ (1.0 - strength) * p0;
					next.Add(point);
				}
				result = next;
			}
			return result;
		}


		decltype(auto) begin(this auto self)
		{
			return self.mPoints.begin();
		}

		decltype(auto) end(this auto self)
		{
			return self.mPoints.end();
		}


	private:
		std::set<Vector<T, D>> mPoints;
	};
}
