#pragma once
// Strawberry Core
#include "Strawberry/Core/Math/Geometry/Ray.hpp"
#include "Strawberry/Core/Math/Graph/Delauney.hpp"
#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
// Standard Library
#include <algorithm>
#include <ranges>
#include <tuple>

#include "Strawberry/Core/Math/Geometry/ConvexPolygon.hpp"
#include "Strawberry/Core/Types/Variant.hpp"


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
		using CellNodeID = UndirectedGraph<Vector<T, 2>>::NodeID;
		using DirectedEdge = DirectedGraph<Vector<T, 2>>::Edge;
		using Edge = UndirectedGraph<Vector<T, 2>>::Edge;


		class Builder;


		/// Structure representing a cell in a voronoi diagram.
		struct Cell
		{
			/// Default comparisons for use in ordered structures.
			auto operator<=>(const Cell& other) const = default;

			std::vector<DirectedEdge> Edges() const
			{
				std::vector<DirectedEdge> edges;
				for (int i = 0; mNodeIDs.size() >= 2 && i < mNodeIDs.size(); i++)
				{
					edges.emplace_back(DirectedEdge{mNodeIDs[i], mNodeIDs[(i + 1) % mNodeIDs.size()]});
				}
				return edges;
			}

			std::vector<CellNodeID> mNodeIDs;
		};

		/// Returns the graph containing the boundaries of the voronoi.
		const auto& GetGraph() const noexcept { return mGraph; }


		const Cell& GetCell(CellID id) const
		{
			return mCellMap.at(id);
		}


		decltype(auto) Cells(this auto&& self)
		{
			return self.mCellMap | std::views::values;
		}

		ConvexPolygon<T> GetCellAsPolygon(const Cell& cell)
		{
			auto points = cell.mNodeIDs | std::views::transform(
				[&] (const auto& x) { return mGraph.GetValue(x); });
			return ConvexPolygon<T>::From(std::move(points));
		}

	private:
		Voronoi(const Delaunay& delaunay)
			: mTriangulation(delaunay)
		{}


		UndirectedVectorGraph<Vector<T, 2>>             mGraph;
		Delaunay                                        mTriangulation;
		std::map<typename Delaunay::Face, unsigned int> mTriangleToNodeMapping;
		/// Maps the IDs of nodes from the triangulation to their respective cells.
		std::map<CellID, Cell>                          mCellMap;
	};


	template <typename T>
	class Voronoi<Vector<T, 2>>::Builder
	{
	public:
		Builder (const Delaunay& triangulation)
			: mResult(triangulation)
		{}

		Voronoi Build()
		{
			MakeDual();
			Normalise();
			return mResult;
		}



	private:
		/// Return the dual graph of this delaunay triangulation.
		///
		/// This is also the graph of the voronoi diagram of the points in this triangulation.
		void MakeDual() 
		{
			MakeEdgesFromDelaunayFaces();
			ClipEdgesToBoundingBox();
			CreateOuterEdges();
		}


		void MakeEdgesFromDelaunayFaces()
		{
			for (const auto& face : mResult.mTriangulation.Faces())
			{
				auto centroid = mResult.mTriangulation.GetFaceCircumcenter(face);
				auto centroidNode = mResult.mGraph.AddNode(centroid);
				mResult.mTriangleToNodeMapping.emplace(face, centroidNode);

				for (const auto& [otherFace, otherCentroidIndex] : mResult.mTriangleToNodeMapping)
				{
					if (face != otherFace && face.SharesEdgeWith(otherFace))
					{
						Edge edge{centroidNode, otherCentroidIndex};
						mResult.mGraph.AddEdge(edge);

						auto sharedEdge = face.GetSharedEdge(otherFace).Value();
						mEdgeOwnership[edge].emplace(sharedEdge.A());
						mEdgeOwnership[edge].emplace(sharedEdge.B());
					}
				}
			}
		}


		void ClipEdgesToBoundingBox()
		{
			const auto& bounds = mResult.mTriangulation.GetOutline();
			/// Record edges that overlap the bounds of the mTriangulation
			/// and adjust them so that they are clipped by the bounds.
			std::vector<Edge> edgesToRemove;
			std::vector<Edge> edgesToAdd;
			for (auto edge : mResult.mGraph.Edges())
			{
				auto a = mResult.mGraph.GetValue(edge.A());
				auto b = mResult.mGraph.GetValue(edge.B());

				bool containsA = bounds.Contains(a);
				bool containsB = bounds.Contains(b);

				// If both points of an edge are contained by the bounds,
				// then nothing needs to be done
				if (containsA && containsB) [[likely]]
				{
					continue;
				}


				if (containsA)
				{
					Assert(!containsB);

					Vector<T, 2> edgeDir = b - a;
					Ray<T, 2> ray(a, edgeDir);
					auto intersections = ray.Intersection(bounds);
					auto intersection = *std::ranges::min_element(intersections, {}, [] (const auto& x) { return x.rayDistance; });

					AssertEQ(mResult.mGraph.Degree(edge.B()), 1);
					edgesToRemove.emplace_back(edge);

					auto newNode = mResult.mGraph.AddNode(intersection.position);
					Edge newEdge(edge.A(), newNode);
					edgesToAdd.emplace_back(edge.A(), newNode);
					mEdgeOwnership[newEdge] = mEdgeOwnership[edge];
				}
				else if (containsB)
				{
					Assert(!containsA);

					Vector<T, 2> edgeDir = a - b;
					Ray<T, 2> ray(b, edgeDir);
					auto intersections = ray.Intersection(bounds);
					auto intersection = *std::ranges::min_element(intersections, {}, [] (const auto& x) { return x.rayDistance; });

					AssertEQ(mResult.mGraph.Degree(edge.A()), 1);
					edgesToRemove.emplace_back(edge);

					auto newNode = mResult.mGraph.AddNode(intersection.position);
					Edge newEdge(edge.B(), newNode);
					edgesToAdd.emplace_back(edge.B(), newNode);
					mEdgeOwnership[newEdge] = mEdgeOwnership[edge];
				}
				// Edges where neither point is inside the bounds are removed.
				else
				{
					edgesToRemove.emplace_back(edge);
				}
			}

			for (auto&& edge : edgesToAdd)
			{
				mResult.mGraph.AddEdge(edge);
			}

			for (auto&& edge : edgesToRemove)
			{
				mResult.mGraph.RemoveEdge(edge);
				mEdgeOwnership.erase(edge);
			}
		}


		// Take each outer edge A and extend an edge B from
		// the circumcentre of A to the boundary of the graph,
		// intersecting the midpoint of edge A.
		//
		// If an edge doesn't have a face at all,
		// then we extend in both directions a ray,
		// from the centre of the edge to the boundary,
		// in the two directions perpendicular to the edge.
		void CreateOuterEdges()
		{
			const auto bounds = mResult.mTriangulation.GetOutline();

			for (const auto edge : mResult.mTriangulation.GetOuterEdges())
			{
				auto vEdgeA = mResult.mTriangulation.GetGraph().GetValue(edge.A());
				auto vEdgeB = mResult.mTriangulation.GetGraph().GetValue(edge.B());

				auto faces = mResult.mTriangulation.FindFacesWithEdge(edge);

				if (faces.size() == 0)
				{
					auto vMidPoint = 0.5 * (vEdgeA + vEdgeB);
					auto vEdgeDir = (vEdgeB - vEdgeA).Perpendicular();

					Line<T, 2> line(vMidPoint, vMidPoint + vEdgeDir);
					auto intersections = line.Intersection(bounds);
					AssertEQ(intersections.size(), 2);
					std::array<typename decltype(mGraph)::NodeID, 2> newNodes{
						mResult.mGraph.AddNode(intersections[0].position),
						mResult.mGraph.AddNode(intersections[1].position)};

					Edge newEdge{newNodes[0], newNodes[1]};
					mResult.mGraph.AddEdge(newEdge);
					mEdgeOwnership[newEdge].emplace(edge.A());
					mEdgeOwnership[newEdge].emplace(edge.B());
				}
				else if (faces.size() == 1)
				{
					auto face = faces[0];
					auto vMean = mResult.mTriangulation.GetFaceAsTriangle(face).GetMean();

					auto centerNode = mResult.mTriangleToNodeMapping.at(face);
					auto vFaceCircumcenter = mResult.mGraph.GetValue(centerNode);

					// Make sure edges are in CW order, so they all point outwards from the face.
					if ((vEdgeA - vMean).DotPerp(vEdgeB - vMean) > 0.0)
					{
						std::swap(vEdgeA, vEdgeB);
					}

					auto vAlongEdge = vEdgeB - vEdgeA;
					auto vVoronoiEdgeDirection = vAlongEdge.Perpendicular();

					Ray<T, 2> ray(vFaceCircumcenter, vVoronoiEdgeDirection);

					auto intersections = ray.Intersection(bounds);
					if (intersections.size() > 0)
					{
						auto intersection = *std::ranges::max_element(intersections, {}, [] (const auto& x) { return x.rayDistance; });
						auto newNode = mResult.mGraph.AddNode(intersection.position);
						Edge newEdge{centerNode, newNode};
						mResult.mGraph.AddEdge(newEdge);

						mEdgeOwnership[newEdge].emplace(edge.A());
						mEdgeOwnership[newEdge].emplace(edge.B());
					}
				}
			}
		}


		void Normalise()
		{
			std::map<CellID, std::set<CellNodeID>> cellSets;
			for (auto& [edge, cells] : mEdgeOwnership)
			{
				AssertEQ(cells.size(), 2);
				for (auto& cell : cells)
				{
					cellSets[cell].emplace(edge.A());
					cellSets[cell].emplace(edge.B());
				}
			}

			for (auto& [id, cellSet] : cellSets)
			{
				std::vector<CellNodeID> cellList = cellSet | std::ranges::to<std::vector>();

				Vector<T, 2> mean;
				for (const auto& node : cellList)
				{
					mean = mean + mResult.mGraph.GetValue(node);
				}
				mean = (1.0 / cellList.size()) * mean;

				std::ranges::sort(cellList, std::greater{}, [&] (const auto& x) { return (mResult.mGraph.GetValue(x) - mean).ATan2(); });

				mResult.mCellMap.emplace(id, std::move(cellList));
			}

			for (auto corner : mResult.mTriangulation.GetOutline().Points())
			{
				auto nearest = *std::ranges::min_element(
					mResult.mTriangulation.GetGraph().NodeIndices(),
					std::less{},
					[&] (const auto& v) { return (corner - mResult.mTriangulation.GetGraph().GetValue(v)).SquareMagnitude(); });

				auto cornerNode = mResult.mGraph.AddNode(corner);
				mResult.mCellMap[nearest].mNodeIDs.emplace_back(cornerNode);

				Vector<T, 2> mean;
				for (const auto& node : mResult.mCellMap[nearest].mNodeIDs)
				{
					mean = mean + mResult.mGraph.GetValue(node);
				}
				mean = (1.0 / mResult.mCellMap[nearest].mNodeIDs.size()) * mean;

				std::ranges::sort(
					mResult.mCellMap[nearest].mNodeIDs,
					std::greater{},
					[&] (const auto& x) { return (mResult.mGraph.GetValue(x) - mean).ATan2(); });
			}

			for (auto& [id, cell] : mResult.mCellMap)
			{
				for (auto& edge : cell.Edges())
				{
					if (!mResult.mGraph.IsConnected(edge.A(), edge.B()))
					{
						mResult.mGraph.AddEdge({edge.A(), edge.B()});
					}
				}
			}
		}


		Voronoi mResult;
		std::map<Edge, std::set<CellID>> mEdgeOwnership;
	};
}
