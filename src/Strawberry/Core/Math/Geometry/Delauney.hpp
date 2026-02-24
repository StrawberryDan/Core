#pragma once


#include "Strawberry/Core/Math/Geometry/Simplex.hpp"
#include "Strawberry/Core/Math/Geometry/Sphere.hpp"
#include "Strawberry/Core/Math/Graph.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include <algorithm>
#include <map>


namespace Strawberry::Core::Math
{
	template <typename T>
	class Delauney;

	template <typename T>
	class PrunedDelauney;


	template <typename T>
	class Delauney<Vector<T, 2>> : public UndirectedGraph<Vector<T, 2>>
	{
	public:
		using Graph = UndirectedGraph<Vector<T, 2>>;
		using Edge = Graph::Edge;

		template <typename>
		friend class Voronoi;


		/// Struct for faces represented in this graph.
		struct Face
		{
		public:
			Face(unsigned int a, unsigned int b, unsigned int c) noexcept
				: mNodes{a, b, c}
			{
				std::sort(mNodes.begin(), mNodes.end());
				AssertNEQ(a, b, "Attempt to create a degenerate triangle!");
				AssertNEQ(a, c, "Attempt to create a degenerate triangle!");
				AssertNEQ(b, c, "Attempt to create a degenerate triangle!");
			}


			const auto& Node(unsigned int i) const noexcept { return mNodes[i]; }
			const auto& Nodes() const noexcept { return mNodes; }


			bool ContainsNode(unsigned int n) const noexcept
			{
				return std::ranges::contains(mNodes, n);
			}


			bool ContainsEdge(Edge e) const noexcept
			{
				return Edges().contains(e);
			}


			std::set<Edge> Edges() const noexcept
			{
				return {
					Edge(mNodes[0], mNodes[1]),
					Edge(mNodes[1], mNodes[2]),
					Edge(mNodes[2], mNodes[0])
				};
			}


			auto operator<=>(const Face&) const = default;


		private:
			std::array<unsigned int, 3> mNodes;
		};


		/// Constructs a basic Delaunay triangulation with rectangular bounds.
		Delauney(const Vector<T, 2>& min, const Vector<T, 2>& max) noexcept
		{
			this->Graph::AddNode(Vector{min[0], min[1]});
			this->Graph::AddNode(Vector{max[0], min[1]});
			this->Graph::AddNode(Vector{min[0], max[1]});
			this->Graph::AddNode(Vector{max[0], max[1]});

			this->AddEdge(Edge(0, 1));
			this->AddEdge(Edge(0, 2));
			this->AddEdge(Edge(1, 2));
			this->AddEdge(Edge(1, 3));
			this->AddEdge(Edge(2, 3));
			AssertEQ(this->mFaces.size(), 2);
		}


		/// Adds a node to this graph and connects it to the triangulation.
		void AddNode(const Vector<T, 2>& value)
		{
			Core::Assert(InBounds(value), "Attempted to call Delaunay::AddNode() with out of bounds point!");

			auto newNode = Graph::AddNode(value);

			auto conflictingFaces = GetConflictingFaces(newNode);

			auto sharedEdges = GetSharedEdges(conflictingFaces);

			for (auto conflictingFace : conflictingFaces)
			{
				mFaces.erase(conflictingFace);
			}

			for (auto sharedEdge : sharedEdges)
			{
				this->RemoveEdge(sharedEdge);
			}

			std::set<Edge> newEdges;
			for (auto conflictingFace : conflictingFaces)
			{
				for (auto node : conflictingFace.Nodes())
				{
					newEdges.insert(Edge(newNode, node));
				}
			}
			for (auto edge : newEdges)
			{
				this->AddEdge(edge);
			}

			Core::Assert(
				IsPlanar(),
				"Delaunay graph is not planar after call to Delaunay::AddNode!");
		}


		void RemoveNode(unsigned int node)
		{
			Graph::RemoveNode(node);
			auto edgesToRemove = this->Edges()
				| std::views::filter([node] (Edge e) { return e.ContainsNode(node); })
				| std::ranges::to<std::set>();
			auto facesToRemove = this->Faces()
				| std::views::filter([node] (Face f) { return f.ContainsNode(node); })
				| std::ranges::to<std::set>();

			for (auto edge : edgesToRemove)
			{
				Graph::RemoveEdge(edge);
			}

			for (auto face : facesToRemove)
			{
				mFaces.erase(face);
			}
		}


		/// Const accessor to the set of all faces in the graph.
		const auto& Faces() const noexcept
		{
			return mFaces;
		}


		/// Return the minumum point of this graph.
		const Vector<T, 2>& GetMin() const noexcept { return this->GetValue(0); }
		/// Return the maximum point of this graph.
		const Vector<T, 2>& GetMax() const noexcept { return this->GetValue(3); }


		/// Returns this triangulation with the supporting vertices removed.
		///
		/// Since, without the bounding vertices, this graph cannot support further addition to the graph,
		/// it is demoted to a normal graph.
		Graph PruneSupportingVertices() const
		{
			Delauney copy = *this;

			copy.RemoveNode(0);
			copy.RemoveNode(1);
			copy.RemoveNode(2);
			copy.RemoveNode(3);

			return std::move(static_cast<Graph>(copy));
		}


	private:
		/// Adds an edge to this graph, and forms any faces resulting from the addition.
		void AddEdge(Edge edge)
		{
			this->Graph::AddEdge(edge);
			std::set<unsigned int> mutualNeighbours;
			std::ranges::set_intersection(this->GetNeighbourIndices(edge.nodes[0]),
										  this->GetNeighbourIndices(edge.nodes[1]),
										  std::inserter(mutualNeighbours, mutualNeighbours.begin()));

			for (auto neighbour : mutualNeighbours)
			{
				AddFace(Face(edge.nodes[0], edge.nodes[1], neighbour));
			}
		}

		/// Use Remove Edge here and make it private to hide it.
		using Graph::RemoveEdge;

		/// Adds a face to this graph.
		/// Excludes faces which contain other nodes.
		void AddFace(Face face)
		{
			auto triangle = FaceToTriangle(face);
			if (!triangle.GetCircumsphere().HasValue())
			{
				return;
			}

			for (auto n : this->Nodes())
			{
				if (face.ContainsNode(n))
					continue;

				auto v = this->GetValue(n);
				if (triangle.Contains(v))
				{
					return;
				}
			}

			mFaces.insert(face);
		}

		/// Returns whether the point v is within the bounds of this graph.
		bool InBounds(const Vector<T, 2>& v) const
		{
			auto& min = GetMin();
			auto& max = GetMax();

			return v[0] >= min[0] && v[1] >= min[1] && v[0] <= max[0] && v[1] <= max[1];
		}
		/// Return the set of faces that conflict with the node given.
		/// Faces conflict with a node if their circumcircle contains that node.
		std::set<Face> GetConflictingFaces(unsigned int node) const
		{
			std::set<Face> conflicingFaces;
			Vector<T, 2> v = this->GetValue(node);
			for (auto face : mFaces)
			{
				Triangle<double, 2> triangle = FaceToTriangle(face);
				Optional<Sphere<double, 2>> circumcircle = triangle.GetCircumsphere();
				if (circumcircle && circumcircle->Contains(v))
				{
					conflicingFaces.emplace(face);
				}
			}
			return conflicingFaces;
		}

		/// Get the corresponding triangle of the face.
		Triangle<T, 2> FaceToTriangle(Face face) const
		{
			return Triangle<T, 2>{
				this->GetValue(face.Node(0)),
				this->GetValue(face.Node(1)),
				this->GetValue(face.Node(2))
			};
		}

		/// Returns the set of edges that are shared by more than one
		/// of the set of faces given.
		std::set<Edge> GetSharedEdges(const std::set<Face>& faces) const
		{
			std::map<Edge, unsigned int> mVotes;

			for (Face face : faces)
			{
				for (Edge edge : face.Edges())
				{
					mVotes[edge]++;
				}
			}

			return mVotes
				| std::views::filter([] (auto x) { return x.second >= 2; })
				| std::views::transform([] (auto x) { return x.first; })
				| std::ranges::to<std::set>();
		}

		// If this graph is planar, then the following identity will be true.
		// Based on Euler's V - E + F = 2 indentity.
		// That identity includes the mega-face outside the graph. Since we don't include
		// that, we check equality to 1 and not 2.
		bool IsPlanar() const noexcept
		{
			return this->Nodes().size() - this->Edges().size() + this->Faces().size() == 1;
		}


		/// Get the center of the cicumcirle of the triangle represented by this face.
		Vector<T, 2> GetFaceCenter(Face face) const
		{
			auto circumsphere = FaceToTriangle(face).GetCircumsphere();
			Assert(circumsphere.HasValue(), "Attempted to get the center of a degenerate face in Delaunay!");
			return circumsphere->Center();
		}

		/// Return the set of faces that share an edge with this face.
		std::set<Face> GetAdjacentFaces(Face face) const
		{
			std::set<Face> adjacentFaces;

			for (auto edge : face.Edges())
			{
				for (auto otherFace : mFaces)
				{
					if (otherFace != face && otherFace.ContainsEdge(edge))
					{
						adjacentFaces.insert(otherFace);
					}
				}
			}

			return adjacentFaces;
		}


	private:
		std::set<Face> mFaces;
	};
}
