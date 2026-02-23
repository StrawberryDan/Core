#pragma once


#include "Strawberry/Core/Math/Geometry/Delauney.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Math/Graph.hpp"


namespace Strawberry::Core::Math
{
	template <typename T>
	class Voronoi
		: public Graph<Vector<T, 2>>
	{
	public:
		/// Returns the voronoi edge graph for this delaunay triangulation.
		///
		/// This works because delauney triangulations and voronoi diagrams are duals of eachother.
		static Voronoi From(const Delauney<T>& delauney) noexcept
		{
			using Delaunay = std::decay_t<decltype(delauney)>;
			using Face = Delaunay::Face;
			using Edge = Delaunay::Edge;
			Voronoi<T> graph;


			std::map<Face, unsigned int> faceCenterNodeIDs;


			for (auto face : delauney.Faces())
			{
				faceCenterNodeIDs.emplace(face, graph.AddNode(delauney.GetFaceCenter(face)));
			}

			for (auto face : delauney.Faces())
			{
				auto neighbours = delauney.GetAdjacentFaces(face);

				for (auto n : neighbours)
				{
					graph.AddEdge(Edge(faceCenterNodeIDs[face], faceCenterNodeIDs[n]));
				}
			}


			return graph;

		}


		Voronoi() = default;
	};
}
