#pragma once


#include "Strawberry/Core/Math/Graph.hpp"
#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename T>
	class Delauney : public UndirectedGraph<Vector<T, 2>>
	{
	public:
		Delauney() = default;

	private:
		
	};
}
