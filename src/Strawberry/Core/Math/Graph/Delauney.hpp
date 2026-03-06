#pragma once
// Strawberry Core
#include "Strawberry/Core/Math/Geometry/Line.hpp"
#include "Strawberry/Core/Math/Geometry/Simplex.hpp"
#include "Strawberry/Core/Math/Geometry/Sphere.hpp"
#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
// Standard library
#include <algorithm>
#include <map>
#include <ranges>


namespace Strawberry::Core::Math
{
	template <typename T>
	class Delaunay;


	template <typename T>
	class Delaunay<Vector<T, 2>>
		: public UndirectedGraph<Vector<T, 2>>
	{
	public:
		using Graph = UndirectedGraph<Vector<T, 2>>;
		using Edge = Graph::Edge;


		/// Struct for the triangular faces represented in this graph.
		struct Face;


		class Builder;
		friend class Builder;


		const auto& Faces() const { return mFaces; }

		Triangle<T, 2> GetFaceAsTriangle(const Face& face) const noexcept
		{
			return Triangle<T, 2>{
				GetValue(face.A()),
				GetValue(face.B()),
				GetValue(face.C())
			};
		}

		Vector<T, 2> GetFaceCircumcenter(const Face& face) const noexcept
		{
			return GetFaceAsTriangle(face).GetCircumsphere().Center();
		}

	private:
		Delaunay(UndirectedGraph<Vector<T, 2>> graph, std::set<Face> faces)
			: UndirectedGraph<Vector<T, 2>>(std::move(graph))
			, mFaces(std::move(faces))
		{}


		std::set<Face> mFaces;
	};


	template <typename T>
	struct Delaunay<Vector<T, 2>>::Face
		{
		public:
			Face(unsigned int a, unsigned int b, unsigned int c) noexcept
				: mNodes{a, b, c}
			{
				std::sort(mNodes.begin(), mNodes.end());
				AssertNEQ(a, b, "Attempt to create a degenerate face!");
				AssertNEQ(a, c, "Attempt to create a degenerate face!");
				AssertNEQ(b, c, "Attempt to create a degenerate face!");
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



	template <typename T>
	class Delaunay<Vector<T, 2>>::Builder
	{
	public:
		Builder(const Vector<T, 2>& min, const Vector<T, 2>& max)
		{
			// Other two orthogonal extreme points.
			Vector<T, 2> xMax(max[0], min[1]);
			Vector<T, 2> yMax(min[0], max[1]);
			// Create the supporting nodes.
			mGraph.AddNode(min);
			mGraph.AddNode(xMax);
			mGraph.AddNode(yMax);
			mGraph.AddNode(max);
			// Create the edges between supporting nodes.
			mGraph.AddEdge({0, 1});
			mGraph.AddEdge({0, 2});
			mGraph.AddEdge({0, 3});
			mGraph.AddEdge({1, 3});
			mGraph.AddEdge({2, 3});
			// Create the faces between supporting nodes.
			mFaces.emplace(Face{0, 2, 3});
			mFaces.emplace(Face{0, 1, 3});
		}


		void AddNode(const Vector<T, 2>& value)
		{
			Assert(NodeInBounds(value),
				   "Attempted to add an out-of-bounds node to Delaunay graph.");
			Assert(!mGraph.ContainsValue(value),
				   "Attempted to add duplicate node to Delaunay graph.");

			std::set conflictingFaces = GetConflictingFaces(value);
			Logging::Info("Found {} conflicing faces", conflictingFaces.size());
			std::set innerEdges       = GetInnerEdges(conflictingFaces);
			Logging::Info("Found {} inner edges", innerEdges.size());
			std::set outerNodes       = GetOuterNodes(conflictingFaces);
			Logging::Info("Found {} outer nodes", outerNodes.size());

			unsigned int newNodeHandle = mGraph.AddNode(value);

			for (Face face : conflictingFaces)
			{
				mFaces.erase(face);
			}

			for (Edge innerEdge : innerEdges)
			{
				mGraph.RemoveEdge(innerEdge);
			}

			for (unsigned int outerNode : outerNodes)
			{
				Edge newEdge(outerNode, newNodeHandle);
				mGraph.AddEdge(newEdge);
				DiscoverFaces(newEdge);
			}
		}


		Delaunay<Vector<T, 2>> Build() const noexcept
		{
			auto copy = mGraph;
			copy.RemoveNode(0);
			copy.RemoveNode(1);
			copy.RemoveNode(2);
			copy.RemoveNode(3);
			return {copy, mFaces};
		}


	private:
		bool NodeInBounds(const Vector<T, 2>& value)
		{
			Vector<T, 2> min = mGraph.GetValue(0);
			Vector<T, 2> max = mGraph.GetValue(3);

			const bool xBounds = min[0] < value[0] && value[0] < max[0];
			const bool yBounds = min[1] < value[1] && value[1] < max[1];
			return xBounds && yBounds;
		}


		Triangle<T, 2> FaceAsTriangle(const Delaunay<Vector<T, 2>>::Face& face) const
		{
			return Triangle<T, 2>(
				mGraph.GetValue(face.Node(0)),
				mGraph.GetValue(face.Node(1)),
				mGraph.GetValue(face.Node(2))
			);
		}


		Sphere<T, 2> FaceAsCircumcircle(const Delaunay<Vector<T, 2>>::Face& face) const
		{
			return FaceAsTriangle(face).GetCircumsphere().Unwrap();
		}


		std::set<Delaunay<Vector<T, 2>>::Face> GetConflictingFaces(const Vector<T, 2>& value) const
		{
			std::set<Face> conflictingFaces;
			for (const auto& face : mFaces)
			{
				if (FaceAsCircumcircle(face).Contains(value))
				{
					conflictingFaces.emplace(face);
				}
			}
			return conflictingFaces;
		}


		std::set<unsigned int> GetOuterNodes(const std::set<Delaunay<Vector<T, 2>>::Face>& faces) const
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


		double GetFaceArea(Face face) const noexcept
		{
			auto a = mGraph.GetValue(face.Node(0));
			auto b = mGraph.GetValue(face.Node(1));
			auto c = mGraph.GetValue(face.Node(2));

			auto v1 = b - a;
			auto v2 = c - a;

			return 0.5 * std::abs(v1[0] * v2[1] - v1[1] * v2[0]);
		}


		void DiscoverFaces(Edge edge)
		{
			std::set<unsigned int> neighboursA = mGraph.GetNeighbourIndices(edge.A());
			std::set<unsigned int> neighboursB = mGraph.GetNeighbourIndices(edge.B());

			std::set<unsigned int> commonNeighbours;
			std::ranges::set_intersection(
				neighboursA, neighboursB,
				std::inserter(commonNeighbours, commonNeighbours.begin())
			);

			Vector<T, 2> edgeVector = mGraph.GetValue(edge.B()) - mGraph.GetValue(edge.A());


			auto faceAreaComparison = [this] (const Face& a, const Face& b)
			{
				return GetFaceArea(a) < GetFaceArea(b);
			};

			// Sort the common nodes into the corresponding faces,
			// divided between those to the left, and to the right of the new edge.
			std::vector<Face> tentativeFaces[2];
			for (auto n : commonNeighbours)
			{
				Face face(n, edge.A(), edge.B());
				Vector toNode = mGraph.GetValue(n) - mGraph.GetValue(edge.A());
				if (edgeVector.DotPerp(toNode) > 0.0f)
				{
					tentativeFaces[0].push_back(face);
				}
				else
				{
					tentativeFaces[1].push_back(face);
				}
			}


			std::array<Optional<Face>, 2> faces {
				tentativeFaces[0].empty() ? Optional<Face>(NullOpt) : *std::ranges::min_element(tentativeFaces[0], faceAreaComparison),
				tentativeFaces[1].empty() ? Optional<Face>(NullOpt) : *std::ranges::min_element(tentativeFaces[1], faceAreaComparison)
			};

			for (auto& face : faces)
			{
				if (face.HasValue())
				{
					Triangle tri = FaceAsTriangle(face.Value());
					bool containsAnyPoint = false;
					for (auto [node, point] : mGraph.Nodes())
					{
						if (face.Value().ContainsNode(node))
						{
							continue;
						}

						if (tri.Contains(point))
						{
							containsAnyPoint = true;
							break;
						}
					}

					if (!containsAnyPoint)
					{
						Logging::Info("Discovered face");
						mFaces.emplace(face.Value());
					}
				}
			}
		}

	private:
		UndirectedGraph<Vector<T, 2>>          mGraph;
		std::set<Delaunay<Vector<T, 2>>::Face> mFaces;
	};
}
