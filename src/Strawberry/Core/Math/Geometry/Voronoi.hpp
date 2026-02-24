#pragma once


#include "Strawberry/Core/Math/Geometry/Delauney.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Math/Graph.hpp"
#include <algorithm>


namespace Strawberry::Core::Math
{
	template <typename T>
	class Voronoi;


	template <typename T>
	class Voronoi<Vector<T, 2>>
	{
	public:
		using Delauney = Delauney<Vector<T, 2>>;
		using Face = Delauney::Face;
		using Edge = Delauney::Edge;


		struct Cell
		{
			std::vector<unsigned int> nodes;
			std::set<Edge> edges;
		};


		/// Returns the voronoi edge graph for this delaunay triangulation.
		///
		/// This works because delauney triangulations and voronoi diagrams are duals of eachother.
		static Voronoi From(const Delauney& delauney) noexcept
		{
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
				auto connectedFaces = delauney.Faces()
					| std::views::filter([&] (Face face) { return face.ContainsNode(node); });
				auto edgeNodes = connectedFaces
					| std::views::transform([&] (Face face) { return faceCenterNodeIDs.at(face); })
					| std::ranges::to<std::vector>();
				auto edges =
					connectedFaces
					| std::views::transform([] (Face face) { return face.Edges(); })
					| std::views::join
					| std::views::filter([node] (Edge e) { return !e.ContainsNode(node); })
					| std::ranges::to<std::set>();


				cellMap[node] = Cell { .nodes = edgeNodes, .edges = edges };
			}


			return Voronoi(delauney, std::move(voronoi), std::move(cellMap));
		}


		const auto& Triangulation() const noexcept { return mTriangulation; }
		const auto& Edges() const noexcept { return mVoronoi; }
		const auto& Cells() const noexcept { return mCellMap; }


		Vector<T, 2> GetCellCentroid(const Cell& cell) const noexcept
		{
			auto cellVertices = cell.nodes
				| std::views::transform([this] (unsigned int i) { this->Edges().GetValue(i); })
				| std::ranges::to<std::vector>();
			return std::ranges::fold_left(cellVertices, {0, 0}, std::plus{}) * (1.0 / (T) cellVertices.size());
		}


		bool CellContains(const Cell& cell, const Vector<T, 2>& point) const noexcept
		{
			auto ls = GetCellAsLineSegments(cell);

			for (const auto& l : ls)
			{
				T dot = (point - l.A()).Dot(l.Direction().Perpendicular());
				if (dot < 0.0)
				{
					return false;
				}
			}

			return true;
		}


		std::set<LineSegment<T, 2>> GetCellAsLineSegments(const Cell& cell) const
		{
			auto centroid = GetCellCentroid(cell);
			for (auto edge : cell.edges)
			{
				LineSegment<T, 2> l(GetValue(edge.nodes[0]), GetValue(edge.nodes[1]));
				if ((centroid - l.A()).Dot(l.Direction()) < 0.0)
				{
					l.Swap();
				}
			}
		}



	private:
		Voronoi(Delauney triangulation,
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
			return Edges().Nodes().size() - Edges().Edges().size() + mCellMap.size() == 1;
		}


	private:
		Delauney mTriangulation;
		UndirectedGraph<Vector<T, 2>> mVoronoi;
		/// A map of indices of nodes in the triangulation mapped to lists
		/// of cell indices in the voronoi mapping which form the cell conainting
		/// that index.
		std::map<unsigned int, Cell> mCellMap;
	};
}
