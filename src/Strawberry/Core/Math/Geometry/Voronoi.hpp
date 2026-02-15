#pragma once


#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Math/Graph.hpp"


namespace Strawberry::Core::Math
{
	template <typename T>
	class Voronoi
		: protected Graph<Vector<T, 2>>
	{
	public:
		Voronoi() = default;
	};
}
