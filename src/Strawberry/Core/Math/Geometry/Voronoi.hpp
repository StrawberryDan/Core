#pragma once


#include "Strawberry/Core/Math/Geometry/PointSet.hpp"


namespace Strawberry::Core::Math
{
	template <typename T>
	class Voronoi
	{
	public:
		Voronoi() = default;


		void AddPoint(const Vector<T, 2>& point)
		{
			mPoints.Add(point);
		}


	private:
		PointSet<T, 2> mPoints;
	};
}
