#pragma once


#include "Strawberry/Core/Markers.hpp"
#include "Strawberry/Core/Math/Geometry/PointSet.hpp"
#include "Strawberry/Core/Math/Geometry/Simplex.hpp"
#include "Strawberry/Core/Math/Geometry/Sphere.hpp"
#include "Strawberry/Core/Math/Graph.hpp"
#include <algorithm>
#include <ranges>


namespace Strawberry::Core::Math
{
	template <typename T>
	class Delauney : public UndirectedGraph<Vector<T, 2>>
	{
	public:
		using Graph = UndirectedGraph<Vector<T, 2>>;
		using Edge = typename Graph::Edge;


		struct Face
		{
			Face(unsigned int a, unsigned int b, unsigned int c)
				: nodes{a, b, c}
			{
				std::sort(nodes.begin(), nodes.end());
			}

			auto operator<=>(const Face& other) const = default;

			bool ContainsEdge(Edge edge) const noexcept
			{
				return GetEdges().contains(edge);
			}

			std::set<Edge> GetEdges() const noexcept
			{
				return { Edge(nodes[0], nodes[1]),
						 Edge(nodes[1], nodes[2]),
						 Edge(nodes[2], nodes[0])};
			}

			std::array<unsigned int, 3> nodes;
		};


		static Delauney FromPoints(const PointSet<T, 2>& points)
		{
			Delauney delaunay;

			auto min = points.MinExtreme();
			auto max = points.MaxExtreme();
			min = (min + 0.1 * (min - max)).Offset(-50, -50);
			max = (max + 0.1 * (max - min)).Offset(50, 50);

			delaunay.Graph::AddNode(min);
			delaunay.Graph::AddNode(Vector{max[0], min[1]});
			delaunay.Graph::AddNode(Vector{min[0], max[1]});
			delaunay.Graph::AddNode(max);

			delaunay.AddEdge(0, 1);
			delaunay.AddEdge(1, 2);
			delaunay.AddEdge(2, 0);
			delaunay.AddEdge(1, 3);
			delaunay.AddEdge(2, 3);
			delaunay.AddEdge(3, 1);
			Core::AssertEQ(delaunay.mFaces.size(), 2);

			for (int i = 0; i < points.Size(); i++)
			{
				delaunay.AddNode(points.Get(i));
			}

			AssertEQ(delaunay.GetNodes().size() - delaunay.GetEdges().size() + delaunay.GetFaces().size(), 2,
					 "Delaunay graph was not planar!");

			return delaunay;
		}


		Vector<T, 2> GetMinPoint() const
		{
			return Graph::GetValue(0);
		}


		Vector<T, 2> GetMaxPoint() const
		{
			return Graph::GetValue(3);
		}


		void AddNode(const Vector<T, 2>& value)
		{
			const auto& min = GetMinPoint();
			const auto& max = GetMaxPoint();
			Core::Assert(min[0] <= value[0] && value[0] <= max[0]);
			Core::Assert(min[1] <= value[1] && value[1] <= max[1]);

			unsigned int newNode = Graph::AddNode(value);

			auto conflictingFaces = mFaces
				| std::views::filter([this, value](Face face)
				{
					Triangle<T, 2> triangle(
						this->GetValue(face.nodes[0]),
						this->GetValue(face.nodes[1]),
						this->GetValue(face.nodes[2]));

					Optional<Sphere<T, 2>> circumcircle = Sphere<T, 2>::Circumcsphere(triangle);
					bool circumcircleTest = !circumcircle || circumcircle.Value().Contains(value);
					return circumcircleTest;
				}) | std::ranges::to<std::vector>();

			for (auto face : conflictingFaces)
			{
				EraseFace(face);
			}

			for (auto face : conflictingFaces)
			{
				AddEdge(face.nodes[0], newNode);
				AddEdge(face.nodes[1], newNode);
				AddEdge(face.nodes[2], newNode);
			}
		}


		void AddEdge(unsigned int a, unsigned int b)
		{
			AddEdge(Edge(a, b));
		}


		void AddEdge(const Edge& edge)
		{
			Graph::AddEdge(edge);

			// Get the neighbours.
			auto neighboursA = this->GetNeighbourIndices(edge.nodes[0]);
			auto neighboursB = this->GetNeighbourIndices(edge.nodes[1]);
			// Find mutual neighbours
			std::set<unsigned int> mutualNeighbours;
			std::set_intersection(neighboursA.begin(), neighboursA.end(),
								  neighboursB.begin(), neighboursB.end(),
								  std::inserter(mutualNeighbours, mutualNeighbours.begin()));
			// Create face for each mutual neighbour.
			for (auto mn : mutualNeighbours)
			{
				mFaces.emplace(edge.nodes[0], edge.nodes[1], mn);
			}
		}


		const std::set<Face>& GetFaces() const noexcept
		{
			return mFaces;
		}


		Face FindContainingFace(const Vector<T, 2>& value)
		{
			// Check all faces to find containing one.
			for (auto face : std::views::reverse(mFaces))
			{
				Triangle<T, 2> triangle(this->GetValue(face.nodes[0]), this->GetValue(face.nodes[1]), this->GetValue(face.nodes[2]));
				triangle.MakeCounterClockwise();

				if (triangle.Contains(value))
					return face;
			}

			Core::Unreachable();
		}

		void EraseFace(const Face& face)
		{
			mFaces.erase(face);

			for (auto edge : face.GetEdges())
			{
				// Do not remove bounding edges
				auto isBoundingEdge = [] (Graph::Edge e)
				{
					return e == Edge(0, 1) || e == Edge(2, 0) || e == Edge(2, 3) || e == Edge(1, 3);
				};
				if (isBoundingEdge(edge)) continue;

				// Check if any other face contains this edge
				bool otherFaceHas = std::ranges::any_of(
					mFaces,
					[edge] (Face f)
					{
						return f.ContainsEdge(edge);
					});
				// If no other face has thie edge, then remove it.
				if (!otherFaceHas)
				{
					this->RemoveEdge(edge);
				}
			}
		}


	private:
		Delauney() = default;


	private:
		std::set<Face> mFaces;
	};
}
