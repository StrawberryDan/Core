#pragma once


#include "Strawberry/Core/Math/Geometry/PointSet.hpp"
#include "Strawberry/Core/Math/Graph.hpp"


namespace Strawberry::Core::Math
{
	template <typename T>
	class Delauney : public GraphTypeUndirected<Vector<T, 2>>
	{
	public:
		static Delauney FromPoints(const PointSet<T, 2>& points)
		{
			Delauney delaunay;

			

			return delaunay;
		}


		Delauney() = default;
	};
}
