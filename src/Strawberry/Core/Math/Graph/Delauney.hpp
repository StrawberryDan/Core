#pragma once
// Strawberry Core
#include "Strawberry/Core/Math/Geometry/AABB.hpp"
#include "Strawberry/Core/Math/Geometry/PointSet.hpp"
#include "Strawberry/Core/Math/Geometry/Simplex.hpp"
#include "Strawberry/Core/Math/Geometry/Sphere.hpp"
#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Graph/GraphWalker.hpp"
#include "Strawberry/Core/Math/Math.hpp"
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
		using Graph = UndirectedVectorGraph<Vector<T, 2>>;
		using Value = Graph::Value;
		using Edge = Graph::Edge;
		using NodeID = Graph::NodeID;


		/// Struct for the triangular faces represented in this graph.
		struct Face;


		/// Builder type and friend declaration.
		class Builder;
		friend class Builder;


		/// Constructs a delaunay triangulation from a set of points.
		static Delaunay From(const PointSet<T, 2>& points, Vector<T, 2> padding)
		{
			Builder builder(points.MinExtreme() - padding
							, points.MaxExtreme() + padding);
			for (const auto& point : points)
			{
				builder.AddNode(point);
			}
			return builder.Build();
		}

		const auto& GetBoundingBox() const noexcept { return mBounds; }

		/// Accessor for the edge of the voronoi cell boundaries.
		const auto& GetGraph() const noexcept { return mGraph; }

		/// Const accessor to the set of faces.
		const auto& Faces() const { return mFaces; }

		/// Gets the number of triangular faces in the graph.
		[[nodiscard]] unsigned int FaceCount() const { return mFaces.size(); }

		/// Returns the selected face as a Triangle with the values as vertices.
		Triangle<T, 2> GetFaceAsTriangle(const Face& face) const noexcept
		{
			Assert(mFaces.contains(face),
				   "Delaunay::GetFaceAsTriangle called with face not "
				   "already in the triangulation!");
			return Triangle<T, 2>{{
					GetGraph().GetValue(face.Node(0)),
					GetGraph().GetValue(face.Node(1)),
					GetGraph().GetValue(face.Node(2))
				}};
		}

		/// Shorthand for getting the circumcircle centre for the result of GetFaceASTriangle().
		Vector<T, 2> GetFaceCircumcenter(const Face& face) const noexcept
		{
			return GetFaceAsTriangle(face).GetCircumsphere().Unwrap().Center();
		}

		/// Returns the faces that are adjacent to the given face.
		std::set<Face> GetAdjacentFaces(const Face& face) const noexcept
		{
			return mFaces
				| std::views::filter([face] (const Face& otherFace) {
					return face != otherFace && face.SharesEdgeWith(otherFace); })
				| std::ranges::to<std::set>();
		}

		/// Looks for the faces on either side of this edge.
		std::vector<Face> FindFacesWithEdge(Edge edge) const
		{
			// Type for vector path walkers.
			using WalkerType = VectorGraphWalker<PathGraphWalker<BasicGraphWalker<Graph>>>;
			// Create two walkers on each side of the edge.
			WalkerType walkers[2] {
				WalkerType(mGraph, edge.A()),
				WalkerType(mGraph, edge.B())
			};

			// Have the walkers swap places so that they
			// end up going in opposite directions.
			walkers[0].WalkTo(edge.B());
			walkers[1].WalkTo(edge.A());

			std::vector<Face> faces;
			// Do for each walker
			for (auto& walker : walkers)
			{
				// Keep trying to walk counter-clockwise until we can't.
				while (walker.TryWalkCCW())
				{
					// We never let the path exceed 4 nodes.
					Assert(walker.PathLength() <= 4);
					// If the path is at the length to encapsulate a triangle.
					if (walker.PathLength() == 4)
					{
						// If our first and current node are the same, we've walked a triangle.
						if (walker.CurrentNode() == walker.GetPreviousNode(walker.PathLength() - 1))
						{
							/// Create the new face.
							Face face(
								walker.GetPreviousNode(0),
								walker.GetPreviousNode(1),
								walker.GetPreviousNode(2));
							/// Put the face in our set.
							faces.emplace_back(face);
						}
						// No need to keep going now.
						break;
					}
				}
			}

			// A valid triangulation edge cannot be shared by more than 2 faces.
			Assert(faces.size() <= 2);

			// It's possible to have the same face twice so we check here.
			if (faces.size() == 2 && faces[0] == faces[1])
			{
				faces.pop_back();
			}

			return faces;
		}

		/// Get the edges on the outside of the triangulation;
		std::set<Edge> GetOuterEdges() const noexcept
		{
			std::set<Edge> outerEdges;
			for (const auto& edge : mGraph.Edges())
			{
				unsigned int containingFaces = 0;
				for (const auto& face : mFaces)
				{
					if (face.ContainsEdge(edge))
					{
						containingFaces++;
					}
				}

				if (containingFaces <= 1)
				{
					outerEdges.emplace(edge);
				}
			}
			return outerEdges;
		}


	private:
		/// Private constructor for use by Builder().
		Delaunay() = default;


		/// The bounding box of this graph.
		AABB<T, 2> mBounds;
		/// The graph of the Delaunay triangulation.
		UndirectedVectorGraph<Vector<T, 2>> mGraph;
		/// The set of triangular faces contained in this graph.
		std::set<Face> mFaces;
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
			std::ranges::sort(mNodes);
			AssertNEQ(a, b, "Attempt to create a degenerate face!");
			AssertNEQ(a, c, "Attempt to create a degenerate face!");
			AssertNEQ(b, c, "Attempt to create a degenerate face!");
		}


		/// Node Index Getter.
		const auto& Node(unsigned int i) const noexcept { return mNodes[i]; }
		/// Const access to Node Array.
		const auto& Nodes() const noexcept { return mNodes; }


		/// Check if this node is a part of this face.
		[[nodiscard]] bool ContainsNode(unsigned int n) const noexcept
		{
			return std::ranges::contains(mNodes, n);
		}


		/// Check if this face includes the given edge.
		bool ContainsEdge(const Edge& e) const noexcept
		{
			return Edges().contains(e);
		}


		// Returns whether this face has another edge in common with this other face.
		bool SharesEdgeWith(const Face& other) const noexcept
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


		/// Default comparison for use in sorted data structures.
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
			: mBoundingBox(min, max)
		{
			auto span = max - min;

			// Store min and max;
			mResult.mBounds = mBoundingBox;
			// Create the supporting nodes.
			mResult.mGraph.AddNode(min);
			mResult.mGraph.AddNode(Vector{2 * span[0], min[1]});
			mResult.mGraph.AddNode(Vector{min[0], 2 * span[1]});
			// Create the edges between supporting nodes.
			mResult.mGraph.AddEdge({0, 1});
			mResult.mGraph.AddEdge({1, 2});
			mResult.mGraph.AddEdge({2, 0});
			// Create the faces between supporting nodes.
			mResult.mFaces.emplace(Face{0, 1, 2});
			Validate();
		}


		/// Adds a node to this triangulation.
		Builder&& AddNode(const Vector<T, 2>& value)
		{
			ZoneScoped;

			Assert(mBoundingBox.Contains(value),
				   "Attempted to add an out-of-bounds node to Delaunay graph.");
			Assert(!mResult.mGraph.ContainsValue(value),
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
			unsigned int newNodeHandle = mResult.mGraph.AddNode(value);

			/// Erase the conflicting faces.
			for (Face face : conflictingFaces)
			{
				mResult.mFaces.erase(face);
				mTriangleCache.erase(face);
				mCircumsphereCache.erase(face);
			}

			/// Erase the innerEdges fo the conflicting faces.
			for (Edge innerEdge : innerEdges)
			{
				mResult.mGraph.RemoveEdge(innerEdge);
			}

			/// Join the new node into the hole by joining it to
			/// the outer points.
			for (unsigned int outerNode : outerNodes)
			{
				/// Add edge to the graph.
				Edge newEdge(outerNode, newNodeHandle);
				mResult.mGraph.AddEdge(newEdge);
				/// Look for new triangular faces.
				auto faces = mResult.FindFacesWithEdge(newEdge);
				mResult.mFaces.insert_range(faces);
			}

			Validate();

			return std::move(*this);
		}


		/// Return the graph with the supporting nodes removed.
		Delaunay Build() const noexcept
		{
			// Return the copy.
			return Prune();
		}


	private:
		Delaunay Prune() const noexcept
		{
			// If we're pruning supporting vertices, we remove all nodes
			// from the supporting rectangle and all faces using those nodes.
			auto copy = mResult;

			copy.mGraph.RemoveNode(0);
			copy.mGraph.RemoveNode(1);
			copy.mGraph.RemoveNode(2);
			copy.mFaces = mResult.mFaces
				| std::views::filter([] (Face face) {
					return !(face.ContainsNode(0) ||
							 face.ContainsNode(1) ||
							 face.ContainsNode(2));
				})
				| std::ranges::to<std::set>();
			Validate(&copy);
			return copy;
		}


		/// Returns the set of faces that conflict with the point being added 'value'.
		///
		/// Edges are defined to be conflictingly if their circumspheres
		std::set<Face> GetConflictingFaces(const Vector<T, 2>& value) const
		{
			std::set<Face> conflictingFaces;
			for (const auto& face : mResult.mFaces)
			{
				if (GetCircumcircle(face).Contains(value))
				{
					conflictingFaces.emplace(face);
				}
			}
			return conflictingFaces;
		}


		/// Returns the set of nodes that outline the set of conflicting faces.
		std::set<unsigned int> GetOuterNodes(const std::set<Face>& faces) const
		{
			// Count how often each edge occurs in these faces.
			std::map<Edge, unsigned int> edgeOccurrences;
			for (const auto& face : faces)
			{
				for (const auto& edge : face.Edges())
				{
					edgeOccurrences[edge] += 1;
				}
			}

			// Filter for the edges that occur twice, meaning that they are inner edges.
			std::set<Delaunay<Vector<T, 2>>::Edge> outerEdges;
			for (const auto& [edge, count] : edgeOccurrences)
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
		std::set<Edge> GetInnerEdges(const std::set<Delaunay<Vector<T, 2>>::Face>& faces) const
		{
			// Count how often each edge occurs in these faces.
			std::map<Edge, unsigned int> edgeOccurrences;
			for (const auto& face : faces)
			{
				for (const auto& edge : face.Edges())
				{
					edgeOccurrences[edge] += 1;
				}
			}

			// Filter for the edges that occur twice, meaning that they are inner edges.
			std::set<Delaunay<Vector<T, 2>>::Edge> innerEdges;
			for (const auto& [edge, count] : edgeOccurrences)
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
			auto iter = mTriangleCache.find(face);
			if (iter == mTriangleCache.end())
			{
				Triangle<T, 2> triangle({
						mResult.GetGraph().GetValue(face.Node(0)),
						mResult.GetGraph().GetValue(face.Node(1)),
						mResult.GetGraph().GetValue(face.Node(2))
					});

				iter = mTriangleCache.emplace(face, triangle).first;
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
			auto a = mResult.GetGraph().GetValue(face.Node(0));
			auto b = mResult.GetGraph().GetValue(face.Node(1));
			auto c = mResult.GetGraph().GetValue(face.Node(2));

			auto v1 = b - a;
			auto v2 = c - a;

			return 0.5 * std::abs(v1[0] * v2[1] - v1[1] * v2[0]);
		}


	private:
		void Validate(const Delaunay* graph = nullptr) const
		{
#ifdef STRAWBERRY_DEBUG
			if (graph == nullptr) graph = &mResult;
			std::map<Edge, unsigned int> edgeOccurrences;
			for (auto face : graph->Faces())
			{
				for (auto edge : face.Edges())
				{
					edgeOccurrences[edge] += 1;
				}
			}

			for (const auto& [edge, count] : edgeOccurrences)
			{
				Core::Assert(count > 0);
				Core::Assert(count < 3);
			}

			AssertEQ(graph->GetGraph().NodeCount() - graph->GetGraph().EdgeCount() + graph->FaceCount(), 1);
#endif
		}

		/// Bounds
		AABB<T, 2> mBoundingBox;
		/// The delaunay graph we are creating.
		Delaunay<Vector<T, 2>>                 mResult;
		/// A cache of the triangle representation of faces from our delaunay.
		mutable std::map<Face, Triangle<T, 2>> mTriangleCache;
		/// A cache of the circumspheres of the faces from our delaunay.
		mutable std::map<Face, Sphere<T, 2>>   mCircumsphereCache;
		/// Bool for whether the resulting graph should be pruned.
		bool mShouldPrune = true;
	};
}
