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
			Voronoi voronoi(delaunay);
			voronoi.MakeDual();
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
#if STRAWBERRY_DEBUG
			for (const auto node : mTriangulation.GetGraph().NodeIndices())
			{
				auto cell = GetCell(node);
				Assert(GetContainingCell(GetCellMeanVertex(cell)).first == node);
			}
#endif
		}


		/// Return the dual graph of this delaunay triangulation.
		///
		/// This is also the graph of the voronoi diagram of the points in this triangulation.
		void MakeDual() noexcept
		{
			MakeEdgesFromDelaunayFaces();
			ClipEdgesToBoundingBox();
			CreateOuterEdges();
		}


		void MakeEdgesFromDelaunayFaces()
		{
			for (const auto& face : mTriangulation.Faces())
			{
				auto centroid = mTriangulation.GetFaceCircumcenter(face);
				auto centroidNode = mGraph.AddNode(centroid);
				mTriangleToNodeMapping.emplace(face, centroidNode);

				for (const auto& [otherFace, otherCentroidIndex] : mTriangleToNodeMapping)
				{
					if (face != otherFace && face.SharesEdgeWith(otherFace))
					{
						mGraph.AddEdge({centroidNode, otherCentroidIndex});
					}
				}
			}
		}


		void ClipEdgesToBoundingBox()
		{
			const auto& bounds = mTriangulation.GetBoundingBox();
			const auto boundsOutline = bounds.GetOutline();
			/// Record edges that overlap the bounds of the mTriangulation
			/// and adjust them so that they are clipped by the bounds.
			std::vector<Edge> edgesToRemove;
			std::vector<Edge> edgesToAdd;
			for (auto edge : mGraph.Edges())
			{
				auto a = mGraph.GetValue(edge.A());
				auto b = mGraph.GetValue(edge.B());

				bool containsA = bounds.Contains(a);
				bool containsB = bounds.Contains(b);

				if (containsA && containsB) [[likely]]
				{
					continue;
				}

				if (containsA)
				{
					Vector<T, 2> edgeDir = b - a;
					Ray<T, 2> ray(a, edgeDir);
					auto intersections = ray.Intersection(boundsOutline);
					auto intersection = *std::ranges::min_element(intersections, {}, [] (const auto& x) { return x.rayDistance; });

					if (mGraph.Degree(edge.B()) == 1)
					{
						edgesToRemove.emplace_back(edge);
					}

					auto newNode = mGraph.AddNode(intersection.position);
					edgesToAdd.emplace_back(edge.A(), newNode);
				}
				else if (containsB)
				{
					Vector<T, 2> edgeDir = a - b;
					Ray<T, 2> ray(b, edgeDir);
					auto intersections = ray.Intersection(boundsOutline);
					auto intersection = *std::ranges::min_element(intersections, {}, [] (const auto& x) { return x.rayDistance; });

					if (mGraph.Degree(edge.A()) == 1)
					{
						edgesToRemove.emplace_back(edge);
					}

					auto newNode = mGraph.AddNode(intersection.position);
					edgesToAdd.emplace_back(edge.B(), newNode);
				}
				else
				{
					edgesToRemove.emplace_back(edge);
				}
			}

			for (auto&& edge : edgesToAdd)
			{
				mGraph.AddEdge(edge);
			}

			for (auto&& edge : edgesToRemove)
			{
				mGraph.RemoveEdge(edge);
			}
		}


		void CreateOuterEdges()
		{
			const auto bounds = mTriangulation.GetBoundingBox();
			const auto boundsOutline = bounds.GetOutline();
			// Take each outer edge A and extend an edge B from
			// the circumcentre of A to the boundary of the graph,
			// intersecting the midpoint of edge A.
			//
			// If an edge doesn't have a face at all,
			// then we extend in both directions a ray,
			// from the centre of the edge to the boundary,
			// in the two directions perpendicular to the edge.
			for (const auto edge : mTriangulation.GetOuterEdges())
			{
				auto vEdgeA = mTriangulation.GetGraph().GetValue(edge.A());
				auto vEdgeB = mTriangulation.GetGraph().GetValue(edge.B());

				auto faces = mTriangulation.FindFacesWithEdge(edge);

				if (faces.size() == 0)
				{
					auto vMidPoint = 0.5 * (vEdgeA + vEdgeB);
					auto vEdgeDir = (vEdgeB - vEdgeA).Perpendicular();

					Line<T, 2> line(vMidPoint, vMidPoint + vEdgeDir);
					auto intersections = line.Intersection(boundsOutline);
					AssertEQ(intersections.size(), 2);
					std::array<typename decltype(mGraph)::NodeID, 2> newNodes{
						mGraph.AddNode(intersections[0].position),
						mGraph.AddNode(intersections[1].position)};

					mGraph.AddEdge({newNodes[0], newNodes[1]});
				}
				else if (faces.size() == 1)
				{
					auto face = faces[0];
					auto vMean = mTriangulation.GetFaceAsTriangle(face).GetMean();

					auto centerNode = mTriangleToNodeMapping.at(face);
					auto vFaceCircumcenter = mGraph.GetValue(centerNode);

					// Make sure edges are in CW order, so they all point outwards from the face.
					if ((vEdgeA - vMean).DotPerp(vEdgeB - vMean) > 0.0)
					{
						std::swap(vEdgeA, vEdgeB);
					}

					auto vAlongEdge = vEdgeB - vEdgeA;
					auto vVoronoiEdgeDirection = vAlongEdge.Perpendicular();

					Ray<T, 2> ray(vFaceCircumcenter, vVoronoiEdgeDirection);

					auto intersections = ray.Intersection(boundsOutline);
					if (intersections.size() > 0)
					{
						auto intersection = *std::ranges::max_element(intersections, {}, [] (const auto& x) { return x.rayDistance; });
						auto newNode = mGraph.AddNode(intersection.position);
						mGraph.AddEdge({centerNode, newNode});
					}
				}
			}
		}


		UndirectedVectorGraph<Vector<T, 2>>             mGraph;
		Delaunay                                        mTriangulation;
		std::map<typename Delaunay::Face, unsigned int> mTriangleToNodeMapping;
		std::map<unsigned int, Cell>                    mCellMap;
	};
}
