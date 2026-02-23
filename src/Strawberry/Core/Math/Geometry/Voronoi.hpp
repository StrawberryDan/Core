#pragma once


#include "Strawberry/Core/Math/Geometry/Delauney.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Math/Graph.hpp"


namespace Strawberry::Core::Math
{
	template <typename T>
	class Voronoi
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

			UndirectedGraph<Vector<T, 2>> voronoi;


			std::map<Face, unsigned int> faceCenterNodeIDs;


			for (auto face : delauney.Faces())
			{
				faceCenterNodeIDs.emplace(face, voronoi.AddNode(delauney.GetFaceCenter(face)));
			}

			for (auto face : delauney.Faces())
			{
				auto neighbours = delauney.GetAdjacentFaces(face);

				for (auto n : neighbours)
				{
					voronoi.AddEdge(Edge(faceCenterNodeIDs[face], faceCenterNodeIDs[n]));
				}
			}


			return Voronoi(delauney.PruneSupportingVertices(), std::move(voronoi));
		}


		const auto& Triangulation() const noexcept { return mTriangulation; }
		const auto& Cells() const noexcept { return mVoronoi; }


	private:
		Voronoi(UndirectedGraph<Vector<T, 2>> triangulation, UndirectedGraph<Vector<T, 2>> voronoi) noexcept
			: mTriangulation(std::move(triangulation))
			, mVoronoi(std::move(voronoi)) {}


	private:
		UndirectedGraph<Vector<T, 2>> mTriangulation;
		UndirectedGraph<Vector<T, 2>> mVoronoi;
	};
}
