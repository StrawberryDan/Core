#pragma once

#include "Strawberry/Core/IO/Logging.hpp"
#include "fmt/ranges.h"
#include <array>
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


	template <typename Payload, GraphConfig Config>
	class Graph
	{
	public:
		template <typename T> requires (std::same_as<Payload, std::decay_t<T>>)
		unsigned AddNode(T&& node)
		{
			mNodes.emplace_back(std::forward<T>(node));
			return mNodes.size() - 1;
		}


		void AddEdge(unsigned nodeAIndex, unsigned nodeBIndex)
		{
			// If this is an undirected graph, then sort the edge indices
			if constexpr (!Config::Directed::value)
			{
				if (nodeAIndex > nodeBIndex) std::swap(nodeAIndex, nodeBIndex);
			}

			// Insert edge
			mEdges.emplace_back(std::array{nodeAIndex, nodeBIndex});
		}


		bool IsConnected(unsigned nodeAIndex, unsigned nodeBIndex) const
		{
			Core::Logging::Info("{}", mEdges);
			if constexpr (!Config::Directed::value)
			{
				if (nodeAIndex > nodeBIndex) std::swap(nodeAIndex, nodeBIndex);
			}

			return std::find(mEdges.begin(), mEdges.end(), std::array{nodeAIndex, nodeBIndex}) != mEdges.end();
		}


	private:
		std::vector<Payload> mNodes;
		std::vector<std::array<unsigned, 2>> mEdges;
	};


	template <typename T>
	using UndirectedGraph = Graph<T, GraphTypeUndirected>;
	template <typename T>
	using DirectedGraph = Graph<T, GraphTypeDirected>;
}
