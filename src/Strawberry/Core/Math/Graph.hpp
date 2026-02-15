#pragma once

#include "Strawberry/Core/IO/Logging.hpp"
#include "fmt/ranges.h"
#include <algorithm>
#include <array>
#include <deque>
#include <type_traits>
#include <vector>


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


			auto operator<=>(const Edge& other) const = default;


			std::array<unsigned int, 2> nodes;
		};


		template <typename T> requires (std::same_as<Payload, std::decay_t<T>>)
		unsigned AddNode(T&& node)
		{
			mNodes.emplace_back(std::forward<T>(node));
			return mNodes.size() - 1;
		}


		void AddEdge(unsigned nodeAIndex, unsigned nodeBIndex)
		{
			// Insert edge
			Edge edge(nodeAIndex, nodeBIndex);
			auto lowerBound = std::lower_bound(mEdges.begin(), mEdges.end(), edge);
			mEdges.insert(lowerBound, edge);
		}


		bool IsConnected(unsigned nodeAIndex, unsigned nodeBIndex) const
		{
			return std::binary_search(mEdges.begin(), mEdges.end(), Edge(nodeAIndex, nodeBIndex));
		}


	private:
		std::vector<Payload> mNodes;
		std::deque<Edge> mEdges;
	};


	template <typename T>
	using UndirectedGraph = Graph<T, GraphTypeUndirected>;
	template <typename T>
	using DirectedGraph = Graph<T, GraphTypeDirected>;
}
