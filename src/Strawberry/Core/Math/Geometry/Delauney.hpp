#pragma once


#include "Strawberry/Core/Markers.hpp"
#include "Strawberry/Core/Math/Geometry/PointSet.hpp"
#include "Strawberry/Core/Math/Geometry/Simplex.hpp"
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
				: mFaces{a, b, c}
			{
				std::sort(mFaces.begin(), mFaces.end());
			}

			auto operator<=>(const Face& other) const = default;

			std::array<unsigned int, 3> mFaces;
		};


		static Delauney FromPoints(const PointSet<T, 2>& points)
		{
			Delauney delaunay;

			auto min = points.MinExtreme();
			auto max = points.MaxExtreme();
			min = min + 0.1 * (min - max);
			max = max + 0.1 * (max - min);

			delaunay.Graph::AddNode(min);
			delaunay.Graph::AddNode(Vector{max[0], min[1]});
			delaunay.Graph::AddNode(Vector{min[0], max[1]});
			delaunay.Graph::AddNode(max);

			delaunay.AddEdge(0, 1);
			delaunay.AddEdge(1, 2);
			delaunay.AddEdge(2, 0);
			delaunay.AddEdge(1, 3);
			delaunay.AddEdge(2, 3);

			for (int i = 0; i < points.Size(); i++)
			{
				delaunay.AddNode(points.Get(i));
			}

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

			Face containingFace = FindContainingFace(value);

			AddEdge(newNode, containingFace.mFaces[0]);
			AddEdge(newNode, containingFace.mFaces[1]);
			AddEdge(newNode, containingFace.mFaces[2]);
		}


		void AddEdge(unsigned int a, unsigned int b)
		{
			AddEdge(Edge(a, b));
		}


		void AddEdge(const Edge& edge)
		{
			Graph::AddEdge(edge);

			auto neighboursA = this->GetNeighbourIndices(edge.nodes[0]);
			auto neighboursB = this->GetNeighbourIndices(edge.nodes[1]);

			std::set<unsigned int> mutualNeighbours;

			std::set_intersection(neighboursA.begin(), neighboursA.end(),
								  neighboursB.begin(), neighboursB.end(),
								  std::inserter(mutualNeighbours, mutualNeighbours.begin()));

			for (auto mn : mutualNeighbours)
			{
				mFaces.emplace(edge.nodes[0], edge.nodes[1], mn);
			}
		}


		Face FindContainingFace(const Vector<T, 2>& value)
		{
			for (auto face : std::views::reverse(mFaces))
			{
				Triangle<T, 2> triangle(this->GetValue(face.mFaces[0]), this->GetValue(face.mFaces[1]), this->GetValue(face.mFaces[2]));
				triangle.MakeCounterClockwise();

				Core::Logging::Trace("Checking point {}, {} against triangle ({}, {}), ({}, {}),.({}, {}) -> {}",
									 value[0], value[1],
									 triangle.Point(0)[0], triangle.Point(0)[1],
									 triangle.Point(1)[0], triangle.Point(1)[1],
									 triangle.Point(2)[0], triangle.Point(2)[1],
									 triangle.Contains(value)
			);

				if (triangle.Contains(value))
					return face;
			}

			Core::Unreachable();
		}


		Delauney() = default;


	private:
		std::set<Face> mFaces;
	};
}
