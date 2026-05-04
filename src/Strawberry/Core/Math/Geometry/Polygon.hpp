#pragma once
#include <vector>

#include "Strawberry/Core/Math/Geometry/ConvexPolygon.hpp"


namespace Strawberry::Core::Math
{
	template <typename T>
	class Polygon
	{
	public:

	private:
		std::vector<ConvexPolygon<T>> mConvexPolygons;
	};
}
