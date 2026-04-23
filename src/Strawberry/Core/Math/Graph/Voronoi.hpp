#pragma once
// Strawberry Core
#include "Strawberry/Core/Math/Geometry/Plane.hpp"
#include "Strawberry/Core/Math/Geometry/Ray.hpp"
#include "Strawberry/Core/Math/Graph/Delauney.hpp"
#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Graph/GraphWalker.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
// Standard Library
#include <algorithm>
#include <ranges>
#include <tuple>


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
		using CellID = Delaunay::NodeID;
		using DirectedEdge = DirectedGraph<Vector<T, 2>>::Edge;
		using Edge = UndirectedGraph<Vector<T, 2>>::Edge;


		/// Structure representing a cell in a voronoi diagram.
		struct Cell
		{
			std::map<DirectedEdge, Optional<unsigned int>> edges;


			/// Gets the set of nodes in counter-clockwise order.
			auto Nodes() const
			{
				/// Get the ordered set of all nodes.
				auto edgeNodes = edges
					| std::views::keys
					| std::views::transform([] (DirectedEdge edge) { return edge.Nodes(); })
					| std::views::join
					| std::ranges::to<std::vector>();

				/// Remove duplicates from the list whilst preserving the order
				/// of first appearence
				for (int i = 0; i < edges.size(); i++)
				{
					auto v = edgeNodes[i];
					for (int j = edgeNodes.size() - 1; j > i; j--)
					{
						if (edgeNodes[i] == edgeNodes[j])
						{
							edgeNodes.erase(edgeNodes.begin() + j);
						}
					}
				}

				return edgeNodes;
			}


			/// Returns the IDs for the neighbouring cells.
			std::set<unsigned int> Neighbours() const noexcept
			{
				return edges
					| std::views::values
					| std::views::filter([] (const auto& x) { return static_cast<bool>(x); })
					| std::views::transform([] (const auto& x) { return x.Value(); })
					| std::ranges::to<std::set>();
			};


			/// Default comparisons for use in ordered structures.
			auto operator<=>(const Cell& other) const = default;
		};

		/// Constructs Voronoi from a set of points.
		static Voronoi<Vector<T, 2>> From(const PointSet<T, 2>& points) noexcept
		{
			return From(Delaunay::From(points));
		}

		/// Creates the corresponding voronoi diagram for a delaunay triangulation.
		static Voronoi<Vector<T, 2>> From(const Delaunay& delaunay) noexcept
		{
			const auto [voronoiEdges, faceNodeMapping] = delaunay.GetDual();

			Voronoi voronoi(delaunay);
			voronoi.mGraph = voronoiEdges;
			voronoi.mTriangleToNodeMapping = faceNodeMapping;
			voronoi.GenerateCellMap();
			return voronoi;
		}

		/// Returns the graph containing the boundaries of the voronoi.
		const auto& GetGraph() const noexcept { return mGraph; }

		/// Returns the cell for a given point.
		const Cell& GetCell(CellID triangulationPointID)
		{
			return mCellMap.at(triangulationPointID);
		}


		/// Returns a range over all the CellIDs.
		decltype(auto) CellIDs() const { return mCellMap | std::views::keys | std::ranges::to<std::set>(); }

		/// Returns a range over the cells in this diagram.
		decltype(auto) Cells() const { return mCellMap | std::views::values | std::ranges::to<std::set>(); }

		/// Gets the set of vertices for this Cell in CCW order.
		std::vector<Vector<T, 2>> CellVertices(Cell cell) const noexcept
		{
			return cell.Nodes()
				| std::views::transform([this] (unsigned int x) { return mGraph.GetValue(x); })
				| std::ranges::to<std::vector>();
		}

		/// Returns the mean vertex position of the vertices in this cell.
		Vector<T, 2> GetCellMeanVertex(Cell cell) const noexcept
		{
			Vector<T, 2> center;
			auto values = CellVertices(cell);
			for (auto v : values)
			{
				center = center + v;
			}

			return center * (1.0 / values.size());
		}

		/// Returns whether the given point is contained by the given cell.
		bool CellContainsPoint(const Cell& cell, Vector<T, 2> point) const
		{
			for (auto edge : cell.edges | std::views::keys)
			{
				LineSegment<T, 2> asLine(
					mGraph.GetValue(edge.A()),
					mGraph.GetValue(edge.B())
				);

				if (asLine.Direction().DotPerp(point - asLine.A()) < 0.0)
				{
					return false;
				}
			}

			return true;
		}

		/// Finds the cell that contains given point.
		std::pair<typename Delaunay::NodeID, Cell> GetContainingCell(const Vector<T, 2>& point)
		{
			typename Delaunay::NodeID currentNode = mCellMap.begin()->first;
			Cell currentCell = GetCell(currentNode);

			while (!CellContainsPoint(currentCell, point))
			{
				auto neighbouringCells = currentCell.Neighbours();
				currentNode = *std::ranges::min_element(neighbouringCells, std::less{},
												[this, point] (Delaunay::NodeID node)
												{
													return (mTriangulation.GetGraph().GetValue(node) - point).SquareMagnitude();
												});
				currentCell = GetCell(currentNode);
			}

			return {currentNode, currentCell};
		}


	private:
		Voronoi(const Delaunay& delaunay)
			: mTriangulation(delaunay)
		{}

		void GenerateCellMap()
		{
			Assert(mCellMap.empty(), "Second call to GenerateCellMap in Voronoi when mCellMap already populated");
			/// Add the cell for each node
			for (const auto node : mTriangulation.GetGraph().NodeIndices())
			{
				mCellMap.emplace(node, CalculateNodeVoronoiCell(node));
			}
#if STRAWBERRY_DEBUG
			for (const auto node : mTriangulation.GetGraph().NodeIndices())
			{
				auto cell = GetCell(node);
				Assert(GetContainingCell(GetCellMeanVertex(cell)).first == node);
			}
#endif
		}

		/// Calculates the voronoi cell at the given index.
		Cell CalculateNodeVoronoiCell(Delaunay::NodeID triangulationNode) const noexcept
		{
			Vector<T, 2> triangulationNodeValue = mTriangulation.GetGraph().GetValue(triangulationNode);

			/// Table of edges mapped to the neighbour with which it shares that edge.
			std::map<DirectedEdge, Optional<unsigned int>> edges;
			/// Get this cell's neighbours in CCW order.
			auto neighbours = VectorGraphWalker(PathGraphWalker(BasicGraphWalker(mTriangulation.GetGraph(), triangulationNode)))
				.GetNeighboursCCW();

			/// For each neighbour, find the shared edge.
			for (auto neighbour : neighbours)
			{
				Vector<T, 2> neighbourValue = mTriangulation.GetGraph().GetValue(neighbour);

				typename Delaunay::Edge edge (triangulationNode, neighbour);
				auto faces = mTriangulation.FindFacesWithEdge(edge) | std::ranges::to<std::vector>();
				Assert(faces.size() <= 2);

				if (faces.size() == 2)
				{
					/// Get voronoi vertices.
					DirectedEdge voronoiEdge {
						mTriangleToNodeMapping.at(faces[0]),
						mTriangleToNodeMapping.at(faces[1]) };

					Vector<T, 2> voronoiEdgeStart = mTriangulation.GetFaceCircumcenter(faces[0]);
					Vector<T, 2> voronoiEdgeEnd = mTriangulation.GetFaceCircumcenter(faces[1]);

					// Make sure edges are all CCW.
					if ((neighbourValue - triangulationNodeValue).DotPerp(voronoiEdgeStart - triangulationNodeValue) >
						(neighbourValue - triangulationNodeValue).DotPerp(voronoiEdgeEnd   - triangulationNodeValue))
					{
						voronoiEdge.Reverse();
					}

					// Store
					edges.emplace(voronoiEdge, neighbour);
				}
			}


			Cell cell;
			cell.edges = edges;
			Assert(cell.edges.size() > 0);

			Assert(CellContainsPoint(cell, GetCellMeanVertex(cell)));
			return cell;
		}


		UndirectedVectorGraph<Vector<T, 2>>             mGraph;
		Delaunay                                        mTriangulation;
		std::map<typename Delaunay::Face, unsigned int> mTriangleToNodeMapping;
		std::map<unsigned int, Cell>                    mCellMap;
	};
}
