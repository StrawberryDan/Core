#pragma once

#include "Strawberry/Core/IO/Logging.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
#include "fmt/ranges.h"
#include <algorithm>
#include <array>
#include <concepts>
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
		typename T::Directed;
		{ T::Directed::value } -> std::convertible_to<bool>;
	};


	template <typename _Value, GraphConfig _Config = GraphTypeUndirected>
	class Graph
	{
	public:
		using Value = _Value;
		using Config = _Config;
		using NodeID = unsigned int;


		struct Edge
		{
			Edge(NodeID a, NodeID b) : nodes{ a, b }
			{
				// Puts edges in normalised order for undirected graphs
				if constexpr (!Config::Directed::value)
				{
					if (nodes[0] > nodes[1]) std::swap(nodes[0], nodes[1]);
				}

				AssertNEQ(a, b);
			}


			/// String formatter
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


			/// Returns whether this edge contains the given node.
			bool ContainsNode(NodeID node) const noexcept
			{
				return nodes[0] == node || nodes[1] == node;
			}


			/// Reverses the order of the nodes in this edge.
			void Reverse() requires (Config::Directed::value)
			{
				std::swap(nodes[0], nodes[1]);
			}


			auto operator<=>(const Edge& other) const = default;


			NodeID A() const noexcept { return nodes[0]; }
			NodeID B() const noexcept { return nodes[1]; }
			std::array<NodeID, 2> Nodes() const { return nodes; }


			std::array<NodeID, 2> nodes;
		};


		_Value& GetValue(NodeID nodeIndex)
		{
			return mNodes.at(nodeIndex);
		}


		const _Value& GetValue(NodeID nodeIndex) const
		{
			return mNodes.at(nodeIndex);
		}


		auto Nodes() const noexcept
		{
			return std::ranges::views::all(mNodes);
		}


		auto NodeIndices() const noexcept
		{
			return mNodes | std::views::keys;
		}


		auto Values() const noexcept
		{
			return mNodes | std::views::values;
		}


		bool ContainsValue(const _Value& value) const noexcept
		{
			return std::ranges::find_if(
				mNodes,
				[value] (const auto& x) { return x.second == value; }) != mNodes.end();
		}


		template <typename T> requires (std::same_as<_Value, std::decay_t<T>>)
		NodeID AddNode(T&& node)
		{
			mNodes.insert({mNextID++, std::forward<T>(node)});
			return mNextID - 1;
		}


		/// Removes the node with the given index, as well as all it's connected edges.
		void RemoveNode(NodeID index)
		{
			auto affectedEdges =  Edges()
				| std::views::filter([index] (Edge e) { return e.ContainsNode(index); })
				| std::ranges::to<std::vector>();


			for (auto edge : affectedEdges)
			{
				RemoveEdge(edge);
			}

			mNodes.erase(index);
		}


		void AddEdge(Edge e)
		{
			Assert(mNodes.contains(e.A()));
			Assert(mNodes.contains(e.B()));
			mEdges[e.A()].emplace(e.B());
			if (!Config::Directed::value)
			{
				mEdges[e.B()].emplace(e.A());
			}
		}


		void RemoveEdge(Edge e)
		{
			mEdges[e.A()].erase(e.B());
			if (!Config::Directed::value)
			{
				mEdges[e.B()].erase(e.A());
			}
		}


		std::set<Edge> Edges() const noexcept
		{
			return mEdges
				| std::views::transform( [] (auto&& x)
				{
					return x.second | std::views::transform([x] (const auto x2) { return Edge(x.first, x2); });
				})
				| std::views::join
				| std::ranges::to<std::set>();
		}


		bool IsConnected(NodeID nodeAIndex, NodeID nodeBIndex) const
		{
			Edge edge(nodeAIndex, nodeBIndex);
			return mEdges[edge.A()].contains(edge.B());
		}


		bool IsConnected(Edge edge) const
		{
			return mEdges[edge.A()].contains(edge.B());
		}


		std::set<NodeID> GetNeighbours(NodeID node) const
		{
			std::set<NodeID> neighbours = mEdges[node];
			return neighbours;
		}


		std::set<NodeID> GetIncomingNeighbours(NodeID node) const requires (Config::Directed::value)
		{
			std::set<NodeID> incomingNeighbours;

			incomingNeighbours = mEdges
				| std::views::filter([node] (const Edge& e) { return e.nodes[1] == node; })
				| std::views::transform([] (const Edge& e) { return e.nodes[0]; })
				| std::ranges::to<std::set>();

			return incomingNeighbours;
		}


		std::set<NodeID> GetOutgoingNeighbours(NodeID node) const requires (Config::Directed::value)
		{
			return mEdges[node];
		}


	private:
		/// Incrementing counter for generating node IDs
		NodeID mNextID = 0;
		/// The map of nodes, associates node ids to values.
		std::map<NodeID, _Value> mNodes;
		/// The set of edges in this graph.
		mutable std::map<NodeID, std::set<NodeID>> mEdges;
	};


	template <typename T>
	using UndirectedGraph = Graph<T, GraphTypeUndirected>;
	template <typename T>
	using DirectedGraph = Graph<T, GraphTypeDirected>;


	template <typename Base>
	concept GraphType = requires(Base base)
	{
		typename Base::Value;
		typename Base::Config;
		GraphConfig<typename Base::Config>;
		std::derived_from<Base, Graph<typename Base::Value, typename Base::Config>>
			|| std::same_as<Base, Graph<typename Base::Value, typename Base::Config>>;
	};
}
