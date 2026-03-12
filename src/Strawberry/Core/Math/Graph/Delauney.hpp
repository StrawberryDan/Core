#pragma once
// Strawberry Core
#include "Strawberry/Core/Math/Geometry/Line.hpp"
#include "Strawberry/Core/Math/Geometry/LineSegment.hpp"
#include "Strawberry/Core/Math/Geometry/Simplex.hpp"
#include "Strawberry/Core/Math/Geometry/Sphere.hpp"
#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Graph/Walker.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
// Standard library
#include <algorithm>
#include <map>
#include <ranges>


namespace Strawberry::Core::Math
{
	/// Base Template.
	template <typename T>
	class Delaunay;


	/// Graph representing a delaunay triangulation of a set of points.
	template <typename T>
	class Delaunay<Vector<T, 2>>
	{
	public:
		using Graph = UndirectedGraph<Vector<T, 2>>;
		using Value = Graph::Value;
		using Edge = Graph::Edge;


		/// Struct for the triangular faces represented in this graph.
		struct Face;


		/// Builder type and friend declaration.
		class Builder;
		friend class Builder;


		const Graph::Value& GetValue(unsigned int nodeID) const noexcept
		{
			return mGraph.GetValue(nodeID);
		}


		const auto& GetGraph() const noexcept { return mGraph; }


		/// Const accessor to the set of faces.
		const auto& Faces() const { return mFaces; }

		/// Returns the selected face as a Triangle with the values as vertices..
		Triangle<T, 2> GetFaceAsTriangle(const Face& face) const noexcept
		{
			Assert(mFaces.contains(face),
				   "Delaunay::GetFaceAsTriangle called with face not "
				   "already in the triangulation!");
			return Triangle<T, 2>{{
					GetValue(face.Node(0)),
					GetValue(face.Node(1)),
					GetValue(face.Node(2))
				}};
		}

		/// Shorthand for getting the circumcircle center for the result of GetFaceASTriangle().
		Vector<T, 2> GetFaceCircumcenter(const Face& face) const noexcept
		{
			return GetFaceAsTriangle(face).GetCircumsphere().Unwrap().Center();
		}


		std::set<Face> GetAdjacentFaces(const Face& face) const noexcept
		{
			return mFaces
				| std::views::filter([face] (const Face& otherFace) {
					return face != otherFace && face.SharesEdgeWith(otherFace);
				})
				| std::ranges::to<std::set>();
		}


		std::set<Edge> GetOuterEdges() const noexcept
		{
			std::map<Edge, unsigned int> votes;

			for (auto face : mFaces)
			{
				for (auto edge : face.Edges())
				{
					votes[edge]++;
				}
			}

			return votes
				| std::views::filter([] (const auto& x) { return x.second == 1; })
				| std::views::keys
				| std::ranges::to<std::set>();
		}


		Vector<T, 2> GetCenter() const noexcept
		{
			return 0.5 * (mMin + mMax);
		}


		std::set<LineSegment<T, 2>> GetBoundingBox() const noexcept
		{
			auto xmax = Vector{mMax[0], mMin[1]};
			auto ymax = Vector{mMin[0], mMax[1]};
			return {
				LineSegment<T, 2>(mMin, xmax),
				LineSegment<T, 2>(xmax, mMax),
				LineSegment<T, 2>(mMax, ymax),
				LineSegment<T, 2>(ymax, mMin)
			};
		}


		Graph GetDual() const noexcept
		{
			Graph dual;

			std::map<Face, unsigned int> faceNodeMapping;

			for (const auto& face : mFaces)
			{
				auto centroid = GetFaceCircumcenter(face);
				auto centroidNode = dual.AddNode(centroid);
				faceNodeMapping.emplace(face, centroidNode);

				for (const auto& [otherFace, otherCentroidIndex] : faceNodeMapping)
				{
					if (face != otherFace && face.SharesEdgeWith(otherFace))
					{
						dual.AddEdge({centroidNode, otherCentroidIndex});
					}
				}
			}

			return dual;
		}


	private:
		/// Private constructor for use by Builder().
		Delaunay() = default;


		UndirectedGraph<Vector<T, 2>> mGraph;
		/// The set of triangular faces contained in this graph.
		std::set<Face> mFaces;

		Vector<T, 2> mMin;
		Vector<T, 2> mMax;
	};


	/// Class Defining a Triangular face on the graph of a Delaunay Triangulation.
	template <typename T>
	struct Delaunay<Vector<T, 2>>::Face
	{
	public:
		/// Construct a fae from the given set of node indices.
		Face(unsigned int a, unsigned int b, unsigned int c) noexcept
			: mNodes{a, b, c}
		{
			/// Nodes are stored in sorted order for normalisation.
			std::sort(mNodes.begin(), mNodes.end());
			AssertNEQ(a, b, "Attempt to create a degenerate face!");
			AssertNEQ(a, c, "Attempt to create a degenerate face!");
			AssertNEQ(b, c, "Attempt to create a degenerate face!");
		}


		/// Node Index Getter.
		const auto& Node(unsigned int i) const noexcept { return mNodes[i]; }
		/// Const access to Node Array.
		const auto& Nodes() const noexcept { return mNodes; }


		/// Check if this node is a part of this face.
		bool ContainsNode(unsigned int n) const noexcept
		{
			return std::ranges::contains(mNodes, n);
		}


		/// Check if this face includes the given edge.
		bool ContainsEdge(Edge e) const noexcept
		{
			return Edges().contains(e);
		}


		bool SharesEdgeWith(Face other) const noexcept
		{
			std::set<unsigned int> u;
			std::ranges::set_intersection(Nodes(), other.Nodes(), std::inserter(u, u.begin()));
			Assert(u.size() < 3);
			return u.size() == 2;
		}


		/// Returns the set of edges in this face.
		std::set<Edge> Edges() const noexcept
		{
			return {
				Edge(mNodes[0], mNodes[1]),
				Edge(mNodes[1], mNodes[2]),
				Edge(mNodes[2], mNodes[0])
			};
		}


		/// Default comparision for use in sorted data structures.
		auto operator<=>(const Face&) const = default;


	private:
		/// The set of 3 nodes in this face.
		std::array<unsigned int, 3> mNodes;
	};



	/// Class for creating Delaunay Triangulations.
	template <typename T>
	class Delaunay<Vector<T, 2>>::Builder
	{
	public:
		/// Create a builder with the given AABB extent.
		Builder(const Vector<T, 2>& min, const Vector<T, 2>& max)
		{
			// Store min and max;
			mGraph.mMin = min;
			mGraph.mMax = max;

			// Other two orthogonal extreme points.
			Vector<T, 2> xMax(max[0], min[1]);
			Vector<T, 2> yMax(min[0], max[1]);
			// Create the supporting nodes.
			mGraph.mGraph.AddNode(min);
			mGraph.mGraph.AddNode(xMax);
			mGraph.mGraph.AddNode(yMax);
			mGraph.mGraph.AddNode(max);
			// Create the edges between supporting nodes.
			mGraph.mGraph.AddEdge({0, 1});
			mGraph.mGraph.AddEdge({0, 2});
			mGraph.mGraph.AddEdge({0, 3});
			mGraph.mGraph.AddEdge({1, 3});
			mGraph.mGraph.AddEdge({2, 3});
			// Create the faces between supporting nodes.
			mGraph.mFaces.emplace(Face{0, 2, 3});
			mGraph.mFaces.emplace(Face{0, 1, 3});
		}


		/// Adds a node to this triangulaiton.
		void AddNode(const Vector<T, 2>& value)
		{
			ZoneScoped;

			Assert(NodeInBounds(value),
				   "Attempted to add an out-of-bounds node to Delaunay graph.");
			Assert(!mGraph.mGraph.ContainsValue(value),
				   "Attempted to add duplicate node to Delaunay graph.");

			/// Get the set of faces that conflic with the value being added.
			std::set conflictingFaces = GetConflictingFaces(value);
			/// Get the set of edges that are shared by 2 of these conflicting faces.
			std::set innerEdges       = GetInnerEdges(conflictingFaces);
			/// The set of edges nodes defining the face of the created by
			/// the deletion of the inner edges.
			/// I.e. the hole torn in the graph.
			std::set outerNodes       = GetOuterNodes(conflictingFaces);

			/// Add the new node to the graph.
			unsigned int newNodeHandle = mGraph.mGraph.AddNode(value);

			/// Erase the conflicting faces.
			for (Face face : conflictingFaces)
			{
				mGraph.mFaces.erase(face);
				mFaceCache.erase(face);
				mCircumsphereCache.erase(face);
			}

			/// Erase the innerEdges fo the conflicting faces.
			for (Edge innerEdge : innerEdges)
			{
				mGraph.mGraph.RemoveEdge(innerEdge);
			}

			/// Join the new node into the hole by joining it to
			/// the outer points.
			for (unsigned int outerNode : outerNodes)
			{
				/// Add edge to the graph.
				Edge newEdge(outerNode, newNodeHandle);
				mGraph.mGraph.AddEdge(newEdge);
				/// Look for new triangular faces.
				DiscoverFaces(newEdge);
			}
		}


		/// Return the graph with the supporting nodes removed.
		Delaunay<Vector<T, 2>> Build(bool pruneSupportVertices = false) const noexcept
		{
			auto copy = mGraph;
			if (pruneSupportVertices)
			{
				copy.mGraph.RemoveNode(0);
				copy.mGraph.RemoveNode(1);
				copy.mGraph.RemoveNode(2);
				copy.mGraph.RemoveNode(3);
				copy.mFaces = mGraph.mFaces
					| std::views::filter([] (Face face) {
						return !(face.ContainsNode(0) ||
								 face.ContainsNode(1) ||
								 face.ContainsNode(2) ||
								 face.ContainsNode(3));
					})
					| std::ranges::to<std::set>();
			}
			return copy;
		}


	private:
		/// Returns if a node is contained by the AABB box bounding this graph.
		bool NodeInBounds(const Vector<T, 2>& value)
		{
			Vector<T, 2> min = mGraph.GetValue(0);
			Vector<T, 2> max = mGraph.GetValue(3);

			const bool xBounds = min[0] < value[0] && value[0] < max[0];
			const bool yBounds = min[1] < value[1] && value[1] < max[1];
			return xBounds && yBounds;
		}


		/// Returns the set of faces that conflict with the point being added 'value'.
		///
		/// Edges are defined to be conflicinging if their circumspheres
		std::set<Delaunay<Vector<T, 2>>::Face> GetConflictingFaces(const Vector<T, 2>& value) const
		{
			std::set<Face> conflictingFaces;
			for (const auto& face : mGraph.mFaces)
			{
				if (GetCircumcircle(face).Contains(value))
				{
					conflictingFaces.emplace(face);
				}
			}
			return conflictingFaces;
		}


		/// Returns the set of nodes that outline the set of conflicing faces.
		std::set<unsigned int> GetOuterNodes(const std::set<Face>& faces) const
		{
			// Count how often each edge occurs in these faces.
			std::map<Edge, unsigned int> edgeOccurences;
			for (const auto& face : faces)
			{
				for (const auto& edge : face.Edges())
				{
					edgeOccurences[edge] += 1;
				}
			}

			// Filter for the edges that occur twice, meaning that they are inner edges.
			std::set<Delaunay<Vector<T, 2>>::Edge> outerEdges;
			for (const auto& [edge, count] : edgeOccurences)
			{
				Core::Assert(count == 1 || count == 2);
				if (count == 1)
				{
					outerEdges.emplace(edge);
				}
			}

			std::set<unsigned int> outerNodes;
			for (const auto& edge : outerEdges)
			{
				outerNodes.emplace(edge.A());
				outerNodes.emplace(edge.B());
			}

			return outerNodes;
		}


		/// Returns the set of edges that are shared amongst the input faces.
		std::set<Delaunay<Vector<T, 2>>::Edge> GetInnerEdges(const std::set<Delaunay<Vector<T, 2>>::Face>& faces) const
		{
			// Count how often each edge occurs in these faces.
			std::map<Delaunay<Vector<T, 2>>::Edge, unsigned int> edgeOccurences;
			for (const auto& face : faces)
			{
				for (const auto& edge : face.Edges())
				{
					edgeOccurences[edge] += 1;
				}
			}

			// Filter for the edges that occur twice, meaning that they are inner edges.
			std::set<Delaunay<Vector<T, 2>>::Edge> innerEdges;
			for (const auto& [edge, count] : edgeOccurences)
			{
				Core::Assert(count == 1 || count == 2);
				if (count == 2)
				{
					innerEdges.emplace(edge);
				}
			}

			return innerEdges;
		}


		/// Gets the given face in the form of a triangle.
		Triangle<T, 2> GetFaceAsTriangle(Face face) const noexcept
		{
			auto iter = mFaceCache.find(face);
			if (iter == mFaceCache.end())
			{
				Triangle<T, 2> triangle({
						mGraph.GetValue(face.Node(0)),
						mGraph.GetValue(face.Node(1)),
						mGraph.GetValue(face.Node(2))
					});

				iter = mFaceCache.emplace(face, triangle).first;
			}

			return iter->second;
		}


		/// Gets the given face's circumcircle.
		Sphere<T, 2> GetCircumcircle(Face face) const noexcept
		{
			auto iter = mCircumsphereCache.find(face);
			if (iter == mCircumsphereCache.end())
			{
				auto sphere = GetFaceAsTriangle(face).GetCircumsphere().Unwrap();
				iter = mCircumsphereCache.emplace(face, sphere).first;
			}

			return iter->second;
		}


		/// Returns the area of the face.
		double GetFaceArea(Face face) const noexcept
		{
			auto a = mGraph.GetValue(face.Node(0));
			auto b = mGraph.GetValue(face.Node(1));
			auto c = mGraph.GetValue(face.Node(2));

			auto v1 = b - a;
			auto v2 = c - a;

			return 0.5 * std::abs(v1[0] * v2[1] - v1[1] * v2[0]);
		}


		/// Looks for a new face on either side of this edge.
		void DiscoverFaces(Edge edge)
		{
			VectorGraphWalker<decltype(mGraph.mGraph)> walkers[2]
				{
					{mGraph.mGraph, edge.A()},
					{mGraph.mGraph, edge.B()}
				};

			walkers[0].WalkTo(edge.B());
			walkers[1].WalkTo(edge.A());


			for (auto& walker : walkers)
			{
				while (walker.TryWalkCCW() && walker.PathLength() <= 4)
				{
					if (walker.CurrentNode() == walker.GetPreviousNode(walker.PathLength() - 1))
					{
						Assert(walker.PathLength() == 4);
						Face face(
							walker.GetPreviousNode(0),
							walker.GetPreviousNode(1),
							walker.GetPreviousNode(2));
						mGraph.mFaces.emplace(face);
					}
				}
			}
		}


	private:
		Delaunay<Vector<T, 2>>                 mGraph;
		mutable std::map<Face, Triangle<T, 2>> mFaceCache;
		mutable std::map<Face, Sphere<T, 2>>   mCircumsphereCache;
	};
}
