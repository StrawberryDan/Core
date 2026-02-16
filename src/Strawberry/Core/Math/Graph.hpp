#pragma once

#include "Strawberry/Core/IO/Logging.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
#include "fmt/ranges.h"
#include <algorithm>
#include <array>
#include <deque>
#include <iterator>
#include <map>
#include <type_traits>
#include <vector>
#include <set>
#include <ranges>


namespace Strawberry::Core::Math
{
	struct GraphTypeUndirected
	{
		using Directed = std::false_type;
	};

	struct GraphTypeDirected
	{
		using Directed = std::true_type;
	};


	template <typename T>
	concept GraphConfig = requires(T t)
	{
		std::same_as<bool, decltype(T::Directed::value)>;
	};


	template <typename Payload, GraphConfig Config = GraphTypeUndirected>
	class Graph
	{
	public:
		struct Edge
		{
			Edge(unsigned int a, unsigned int b) : nodes{ a, b }
			{
				// Puts edges in normalised order for undirected graphs
				if constexpr (!Config::Directed::value)
				{
					if (nodes[0] > nodes[1]) std::swap(nodes[0], nodes[1]);
				}
			}


			std::string ToString() const
			{
				if constexpr (Config::Directed::value)
				{
					return fmt::format("{} -> {}", nodes[0], nodes[1]);
				}
				else
				{
					return fmt::format("{} <-> {}", nodes[0], nodes[1]);
				}
			}


			bool ContainsNode(unsigned int node) const
			{
				return nodes[0] == node || nodes[1] == node;
			}


			auto operator<=>(const Edge& other) const = default;


			std::array<unsigned int, 2> nodes;
		};


		struct Face
		{
			Face(unsigned int a, unsigned int b, unsigned int c)
				: nodes{a, b, c}
			{
				if constexpr (!Config::Directed::value)
				{
					std::sort(nodes.begin(), nodes.end());
				}
			}


			bool ContainsNode(unsigned int node)
			{
				return nodes[0] == node || nodes[1] == node || nodes[2] == node;
			}


			Optional<unsigned int> IndexOf(unsigned int node) const
			{
				if (nodes[0] == node) return 0;
				if (nodes[1] == node) return 1;
				if (nodes[2] == node) return 2;
				return NullOpt;
			}


			Edge GetEdge(unsigned int i) const
			{
				return Edge(nodes[(0 + i) % 3], nodes[(1 + i) % 3]);
			}


			bool ContainsEdge(Edge edge) const
			{
				if constexpr (Config::Directed::value)
				{
					return ContainsNode(edge.nodes[0]) && ContainsNode(edge.nodes[1]);
				}
				else
				{
					unsigned int base = edge.nodes[0];
					Optional<unsigned int> baseIndex = IndexOf(base);
					if (!baseIndex) return false;

					Face rotated = Rotate(-*baseIndex);
					return
						edge.nodes[0] == rotated.nodes[0] && edge.nodes[1] == rotated.nodes[1] ||
						edge.nodes[1] == rotated.nodes[1] && edge.nodes[2] == rotated.nodes[2];
				}
			}

			Face Rotate(int rotation) const
			{
				Face face(
					nodes[(0 + rotation) % 3],
					nodes[(1 + rotation) % 3],
					nodes[(2 + rotation) % 3]);
				return face;
			}

			auto operator<=>(const Face& other) const = default;

			std::array<unsigned int, 3> nodes;
		};


		Payload& GetValue(unsigned int nodeIndex)
		{
			return mNodes[nodeIndex];
		}


		const Payload& GetValue(unsigned int nodeIndex) const
		{
			return mNodes[nodeIndex];
		}


		template <typename T> requires (std::same_as<Payload, std::decay_t<T>>)
		unsigned AddNode(T&& node)
		{
			mNodes.insert({mNextID++, std::forward<T>(node)});
			return mNextID - 1;
		}


		void RemoveNode(unsigned int index)
		{
			mNodes.erase(index);

			auto affectedEdges =  mEdges
				| std::views::filter([index] (Edge e) { return e.ContainsNode(index); })
				| std::ranges::to<std::vector>();


			for (auto edge : affectedEdges)
			{
				RemoveEdge(edge);
			}
		}


		void AddEdge(unsigned nodeAIndex, unsigned nodeBIndex)
		{
			// Insert edge
			mEdges.insert({nodeAIndex, nodeBIndex});
			// Update face list
			UpdateFacesNewEdge(Edge(nodeAIndex, nodeBIndex));
		}


		void AddEdge(Edge e)
		{
			AddEdge(e.nodes[0], e.nodes[1]);
		}


		void RemoveEdge(unsigned int nodeA, unsigned int nodeB)
		{
			Edge edge(nodeA, nodeB);
			mEdges.erase(edge);

			mFaces = mFaces
				| std::views::filter([edge] (auto f) { return !f.ContainsEdge(edge); })
				| std::ranges::to<std::set>();
		}


		void RemoveEdge(Edge e)
		{
			RemoveEdge(e.nodes[0], e.nodes[1]);
		}


		bool IsConnected(unsigned nodeAIndex, unsigned nodeBIndex) const
		{
			return mEdges.contains({nodeAIndex, nodeBIndex});
		}


		std::set<std::pair<unsigned int, Payload&>> GetNeighbours(unsigned int node)
		{
			return GetNeighourIndices(node)
				| std::views::transform([this] (auto x) { return std::make_pair<unsigned int, Payload&>(x, GetValue(x)); })
				| std::ranges::to<std::set>();
		}


		std::set<unsigned int> GetNeighourIndices(unsigned int node) const requires (!Config::Directed::value)
		{
			std::set<unsigned int> neighbours;

			for (const auto& edge : mEdges)
			{
				if (edge.nodes[0] == node) neighbours.insert(edge.nodes[1]);
				if (edge.nodes[1] == node) neighbours.insert(edge.nodes[0]);
			}

			return neighbours;
		}


		std::set<unsigned int> GetIncomingNeighbourIndices(unsigned int node) const requires (Config::Directed::value)
		{
			std::set<unsigned int> incomingNeighbours;

			incomingNeighbours = mEdges
				| std::views::filter([node] (Edge e) { return e.nodes[1] == node; })
				| std::views::transform([] (Edge e) { return e.nodes[0]; })
				| std::ranges::to<std::set>();

			return incomingNeighbours;
		}


		std::set<unsigned int> GetOutgoingNeighbourIndices(unsigned int node) const requires (Config::Directed::value)
		{
			std::set<unsigned int> outgoingNeighbours;

			outgoingNeighbours = mEdges
				| std::views::filter([node] (Edge e) { return e.nodes[0] == node; })
				| std::views::transform([] (Edge e) { return e.nodes[1]; })
				| std::ranges::to<std::set>();

			return outgoingNeighbours;
		}


		const std::set<Face>& GetFaces() const
		{
			return mFaces;
		}


	private:
		void UpdateFacesNewEdge(Edge newEdge)
		{
			if constexpr (!Config::Directed::value)
			{
				std::set<unsigned int> neighboursA = GetNeighourIndices(newEdge.nodes[0]);
				std::set<unsigned int> neighboursB = GetNeighourIndices(newEdge.nodes[1]);
				std::set<unsigned int> mutualNeighbours;
				std::set_intersection(neighboursA.begin(), neighboursA.end(),
									  neighboursB.begin(), neighboursB.end(),
									  std::inserter(mutualNeighbours, mutualNeighbours.begin()));

				for (auto mn : mutualNeighbours)
				{
					mFaces.insert(Face(newEdge.nodes[0], newEdge.nodes[1], mn));
				}
			}
			else
			{
				auto incomingNeighbours = GetIncomingNeighbourIndices(newEdge.nodes[0]);
				auto outgoingNeighbours = GetOutgoingNeighbourIndices(newEdge.nodes[1]);
				std::set<unsigned int> mutualNeighbours;
				std::set_intersection(incomingNeighbours.begin(), incomingNeighbours.end(),
									  outgoingNeighbours.begin(), outgoingNeighbours.end(),
									  std::inserter(mutualNeighbours, mutualNeighbours.begin()));

				for (auto mn : mutualNeighbours)
				{
					mFaces.emplace(newEdge.nodes[0], newEdge.nodes[1], mn);
				}
			}
		}


		unsigned int mNextID = 0;
		std::map<unsigned int, Payload> mNodes;
		std::set<Edge> mEdges;
		std::set<Face> mFaces;
	};


	template <typename T>
	using UndirectedGraph = Graph<T, GraphTypeUndirected>;
	template <typename T>
	using DirectedGraph = Graph<T, GraphTypeDirected>;
}
