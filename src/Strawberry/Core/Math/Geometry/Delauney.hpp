#pragma once


#include "Strawberry/Core/Math/Geometry/PointSet.hpp"


namespace Strawberry::Core::Math
{
	template <typename T>
	class Delauney
	{
	public:
		Delauney() = default;


	private:
		PointSet<T, 2> mPoints;
	};
}
