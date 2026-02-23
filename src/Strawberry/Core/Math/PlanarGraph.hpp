#pragma once


#include "Strawberry/Core/Math/Graph.hpp"
#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename Base>
	class PlanarGraph;

	template <typename T, GraphConfig CONFIG>
	class PlanarGraph<Graph<Vector<T, 2>, CONFIG>>
		: public Graph<Vector<T, 2>, CONFIG>
	{
	public:


	private:
		
	};
}
