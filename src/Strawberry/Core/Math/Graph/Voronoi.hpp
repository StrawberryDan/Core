#pragma once
// Strawberry Core
#include "Strawberry/Core/Math/Geometry/Plane.hpp"
#include "Strawberry/Core/Math/Geometry/Ray.hpp"
#include "Strawberry/Core/Math/Graph/Delauney.hpp"
#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
// Standard Library
#include <algorithm>


namespace Strawberry::Core::Math
{
	/// Base template.
	template <typename T>
	class Voronoi;


	/// Template specialisation for vectors.
	template <typename T>
	class Voronoi<Vector<T, 2>>
	{
	public:
		/// Alias types.
		using Delaunay = Delaunay<Vector<T, 2>>;
		using Face = Delaunay::Face;
		using Edge = Delaunay::Edge;


		struct Cell
		{

		};


		static Voronoi<Vector<T, 2>> From(const Delaunay& delaunay) noexcept
		{
			Voronoi voronoi;

			voronoi.mGraph = delaunay.GetDual();

			return voronoi;
		}


		const auto& GetGraph() const noexcept { return mGraph; }


	private:
		Voronoi() = default;


		UndirectedGraph<Vector<T, 2>> mGraph;
	};
}
