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
		struct Cell
		{
			std::vector<unsigned int> nodes;
		};


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
			/// Mapping of triangulation node  handles to the list
			/// of edge graph node handles that make up the cell surrounding this
			/// point.
			std::map<unsigned int, Cell> cellMap;

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

			for (auto node : delauney.Nodes())
			{
				auto containingFaces = delauney.Faces()
					| std::views::filter([&] (Face face) { return face.ContainsNode(node); })
					| std::views::transform([&] (Face face) { return faceCenterNodeIDs.at(face); })
					| std::ranges::to<std::vector>();


				std::ranges::sort(containingFaces, std::less{}, [&] (auto face)
				{
					return (voronoi.GetValue(face) - delauney.GetValue(node)).ATan2();
				});

				cellMap[node] = Cell { .nodes = containingFaces };
			}


			return Voronoi(delauney, std::move(voronoi), std::move(cellMap));
		}


		const auto& Triangulation() const noexcept { return mTriangulation; }
		const auto& Cells() const noexcept { return mVoronoi; }


	private:
		Voronoi(UndirectedGraph<Vector<T, 2>> triangulation,
				UndirectedGraph<Vector<T, 2>> voronoi,
				std::map<unsigned int, Cell> cellMap) noexcept
			: mTriangulation(std::move(triangulation))
			, mVoronoi(std::move(voronoi))
			, mCellMap(std::move(cellMap))
		{
			// Assert(IsPlanar(), "Result of Voronoi constructor was not planar!");
		}


		bool IsPlanar() const noexcept
		{
			return Cells().Nodes().size() - Cells().Edges().size() + mCellMap.size() == 1;
		}


	private:
		UndirectedGraph<Vector<T, 2>> mTriangulation;
		UndirectedGraph<Vector<T, 2>> mVoronoi;
		/// A map of indices of nodes in the triangulation mapped to lists
		/// of cell indices in the voronoi mapping which form the cell conainting
		/// that index.
		std::map<unsigned int, Cell> mCellMap;
	};
}
