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
		}


		void AddEdge(Edge e)
		{
			AddEdge(e.nodes[0], e.nodes[1]);
		}


		void RemoveEdge(unsigned int nodeA, unsigned int nodeB)
		{
			Edge edge(nodeA, nodeB);
			mEdges.erase(edge);
		}


		void RemoveEdge(Edge e)
		{
			RemoveEdge(e.nodes[0], e.nodes[1]);
		}


		bool IsConnected(unsigned nodeAIndex, unsigned nodeBIndex) const
		{
			return mEdges.contains({nodeAIndex, nodeBIndex});
		}


		bool IsConnected(Edge edge) const
		{
			return IsConnected(edge.nodes[0], edge.nodes[1]);
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


	private:
		unsigned int mNextID = 0;
		std::map<unsigned int, Payload> mNodes;
		std::set<Edge> mEdges;
	};


	template <typename T>
	using UndirectedGraph = Graph<T, GraphTypeUndirected>;
	template <typename T>
	using DirectedGraph = Graph<T, GraphTypeDirected>;
}
