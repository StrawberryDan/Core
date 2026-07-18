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
		/// Returns a set of 'count' points over a random uniform distibution within the given bounding box.
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


		/// Returns a set of 'count' points over a random uniform distribution within the given simplex.
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


		/// Constructs an empty point set.
		PointSet() = default;


		/// Returns a copy of the ith point in this set.
		const Vector<T, D> Get(unsigned int i) const { return *std::next(mPoints.begin(), i); }


		/// Returns the number of points in this set.
		unsigned int Size() const { return mPoints.size(); }


		/// Adds a point to this point set.
		void Add(const Vector<T, D>& point)
		{
			mPoints.insert(point);
		}


		/// Removes the given point from the set.
		void Remove(const Vector<T, D>& point)
		{
			mPoints.erase(point);
		}


		/// Returns whether the given pointis contained in this set.
		bool Contains(const Vector<T, D>& point) const
		{
			return mPoints.contains(point);
		}


		/// Return the minimum point of the minimum bounding box of these points.
		Vector<T, D> BoundingBoxMin() const
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


		/// Return the maximum point of the minimum bounding box of these points.
		Vector<T, D> BoundingBoxMax() const
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


		// Return the minimum bounding box for this pointset.
		AABB<T, D> BoundingBox() const
		{
			return AABB<T, D>(BoundingBoxMin(), BoundingBoxMax());
		}


		/// Applies Lloyd Relaxation to the given set of points.
		///
		/// Lloyd Relaxation repeatedly transforms the point set into
		/// it's voronoi diagram. Each point is them moved towards its
		/// containing cell's center of mass (relative to the strength parameter).
		/// Increasing the iteration count will result in more uniform distributions
		/// of points. The extreme result of this is a set of evenly spaces points.
		PointSet Relaxed(const AABB<T, 2>& bounds, unsigned int iterationCount = 1, double strength = 1.0) requires (D == 2)
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
					auto cell = voronoi.GetCell(node).Unwrap();

					auto p0 = delaunay.GetGraph().GetValue(node);
					auto p1 = voronoi.GetCellAsPolygon(cell).CenterOfMass();

					auto point = strength * p1
						+ (1.0 - strength) * p0;

					// If we've gone out of bounds, use the intersection of the
					// ray defined by p0-p1 and the bounding box.
					if (!bounds.Contains(point)) [[unlikely]]
					{
						Ray<T, 2> ray(p0, p1 - p0);
						auto intersections = ray.Intersection(bounds.AsPolygon());
						point = intersections[0].position;
					}

					next.Add(point);
				}
				result = next;
			}
			return result;
		}

		/// Begin method to make class iterable.
		decltype(auto) begin(this auto&& self)
		{
			if constexpr (std::is_const_v<decltype(self)>)
			{
				return self.mPoints.cbegin();
			}
			else
			{
				return self.mPoints.begin();
			}
		}

		/// End method to make class iterable.
		decltype(auto) end(this auto&& self)
		{
			if constexpr (std::is_const_v<decltype(self)>)
			{
				return self.mPoints.cend();
			}
			else
			{
				return self.mPoints.end();
			}
		}


	private:
		std::set<Vector<T, D>> mPoints;
	};
}
