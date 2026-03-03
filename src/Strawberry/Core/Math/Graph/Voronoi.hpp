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
	template <typename T>
	class Voronoi;


	template <typename T>
	class Voronoi<Vector<T, 2>>
	{
	public:
		using Delauney = Delauney<Vector<T, 2>>;
		using PrunedDelauney = PrunedDelauney<Vector<T, 2>>;
		using Face = Delauney::Face;
		using Edge = Delauney::Edge;


		struct Cell
		{
			/// Centroid of the face.
			Vector<T, 2>             centroid;
			/// The set of node indices of this cell.
			std::set<unsigned int>   nodes;
			/// The set of edges comprising this cell.
			std::set<Edge>           edges;
			/// The set of planes defining this cell.
			std::vector<Plane<T, 2>> boundary;
		};


		/// Prunes the given graph before generating a voronoi from it.
		static Voronoi From(const Delauney& delauney) noexcept
		{
			return From(delauney.Pruned());
		}


		/// Returns the voronoi edge graph for this delaunay triangulation.
		///
		/// This works because delauney triangulations and voronoi diagrams are duals of eachother.
		static Voronoi From(const PrunedDelauney& delauney) noexcept
		{
			UndirectedGraph<Vector<T, 2>> voronoi;

			/// Mapping of the Delaunay graphs faces to the corresponding node
			/// in the voronoi diagram.
			std::map<Face, unsigned int> faceToNodeMap;
			/// Mapping of triangulation node handles to corresponding cell info.
			std::map<unsigned int, Cell> cellMap;

			/// For each face in the delaunay, take it's center and add it to the voronoi graph.
			/// Keep track of the association between that face and this new node.
			for (auto face : delauney.Faces())
			{
				unsigned int newNode = voronoi.AddNode(delauney.GetFaceCenter(face));
				faceToNodeMap.emplace(face, newNode);
			}

			/// Join up the voronoi nodes corresponding to faces that are connected in the
			/// delauney.
			for (auto face : delauney.Faces())
			{
				auto neighbours = delauney.GetAdjacentFaces(face);

				for (auto n : neighbours)
				{
					voronoi.AddEdge(Edge(faceToNodeMap[face], faceToNodeMap[n]));
				}
			}

			/// Record the details of each voronoi cell for each delaunay vertex.
			for (auto node : delauney.NodeIndices())
			{
				/// Get all the faces that contain this node.
				auto connectedFaces = delauney.Faces()
					| std::views::filter([&] (Face face) { return face.ContainsNode(node); });
				/// Get the nodes that are part of this cell's boundary.
				auto edgeNodes = connectedFaces
					| std::views::transform([&] (Face face) { return faceToNodeMap.at(face); })
					| std::ranges::to<std::set>();
				/// Calculate the centroid of the cell.
				Vector<T, 2> centroid;
				for (auto n : edgeNodes)
				{
					centroid += voronoi.GetValue(n);
				}
				centroid = (1.0 / edgeNodes.size()) * centroid;
				/// Get the edges involved in this cell by taking all the edges of the connected faces,
				/// and removing all of the edges to/from the current node.
				auto edges =
					connectedFaces
					| std::views::transform([] (Face face) { return face.Edges(); })
					| std::views::join
					| std::views::filter([node] (Edge e) { return !e.ContainsNode(node); })
					| std::ranges::to<std::set>();
				/// Map edges onto linesegments.
				auto boundary = edges
					| std::views::transform([] (Edge e) { return LineSegment<T, 2>(); })
					| std::ranges::to<std::vector>();
				/// Sort boundary into ccw order.
				std::ranges::sort(
					boundary, std::less(),
					[centroid] (LineSegment<T, 2> l)
					{
						return (l.Midpoint() - centroid).ATan2();
					});
				/// Orient each part of the boundary to be CCW.
				for (auto& l : boundary)
				{
					auto toCentroid = centroid - l.Midpoint();
					auto perp = l.Direction().Perpendicular();
					if (toCentroid.Dot(perp) < 0.0)
					{
						l.Swap();
					}
				}
				/// Transform the boundary edges into planes.
				auto boundaryPlanes = boundary
					| std::views::transform([] (const LineSegment<T, 2>& l)
					{
						return Plane<T, 2>::FromNormalAndPoint(l.Direction().Perpendicular(), l.A());
					})
					| std::ranges::to<std::vector>();

				/// Store this information.
				cellMap[node] = Cell {
					.centroid = centroid,
					.nodes = std::move(edgeNodes),
					.edges = std::move(edges),
					.boundary = std::move(boundaryPlanes)
				};
			}


			/// Create the edges bisecting the outer edges of the delauney graph.
			/// We need these edges to partition 2D space completely.
			for (auto outerEdge : delauney.GetOuterEdges())
			{
				/// Find the face which contains this outer edge.
				auto outerFace = std::ranges::find_if(
					delauney.Faces(),
					[outerEdge] (auto f) { return f.ContainsEdge(outerEdge); });
				AssertNEQ(outerFace, delauney.Faces().end(), "Failed to find face for outer edge in Voronoi::From");

				/// Find the voronoi node for this face.
				auto voronoiNode = faceToNodeMap.at(*outerFace);
				/// Get the position of this node.
				auto voronoiNodeLocation = voronoi.GetValue(voronoiNode);

				// Get this edge as a line segment.
				LineSegment<T, 2> outerEdgeLine(
					delauney.GetValue(outerEdge.A()),
					delauney.GetValue(outerEdge.B()));

				/// Get the perpendicular direction for this edge.
				Vector<T, 2> perpendicular = outerEdgeLine.Direction().Perpendicular();
				/// Invert the perpendicular so that it extends away from the center of the graph.
				Vector<T, 2> center = delauney.GetCenter();
				if (perpendicular.Dot(center - outerEdgeLine.A()) > 0.0)
				{
					perpendicular = perpendicular * -1;
				}

				/// Create a ray from the voronoiNode following the perpendicular.
				Ray<T, 2> perpendicularRay(
					voronoiNodeLocation,
					perpendicular
				);

				/// Find where this ray intersects with any of the graphs bounding planes.
				Core::Optional<Vector<T, 2>> perpendicularEndpoint;
				for (auto line : delauney.GetBoundingBox())
				{
					auto intersection = perpendicularRay.Intersection(line);
					if (intersection)
					{
						// Overwrite endpoint if either there is no endpoint yet, or we've found a closer one.
						if (!perpendicularEndpoint)
						{
							perpendicularEndpoint = intersection.Value();
						}
						else
						{
							float oldDist = (perpendicularEndpoint.Value() - perpendicularRay.Origin()).SquareMagnitude();
							float newDist = (intersection.Value() - perpendicularRay.Origin()).SquareMagnitude();
							if (newDist < oldDist)
							{
								perpendicularEndpoint = intersection.Value();
							}
						}
					}
				}

				// If we found no intersection (unlikely),
				// then just extend the ray by the unit length.
				// This is because the length of the ray doesn't really matter
				// for the sake of checking if a point is in a cell.
				if (!perpendicularEndpoint) [[unlikely]]
				{
					perpendicularEndpoint = perpendicularRay.Origin() + perpendicularRay.Direction();
				}

				/// Add the new node to the graph, and add the edge.
				unsigned int perpendicularEndpointIndex = voronoi.AddNode(perpendicularEndpoint.Unwrap());
				auto newEdge = Edge(voronoiNode, perpendicularEndpointIndex);
				voronoi.AddEdge(newEdge);

				cellMap.at(outerEdge.A()).nodes.insert(perpendicularEndpointIndex);
				cellMap.at(outerEdge.B()).nodes.insert(perpendicularEndpointIndex);
				cellMap.at(outerEdge.A()).edges.insert(newEdge);
				cellMap.at(outerEdge.B()).edges.insert(newEdge);
			}

			return Voronoi(delauney, std::move(voronoi), std::move(cellMap));
		}


		const auto& Triangulation() const noexcept { return mTriangulation; }
		const auto& Edges() const noexcept { return mVoronoi; }
		const auto& Cells() const noexcept { return mCellMap; }


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
