#pragma once

#include "Strawberry/Core/IO/Logging.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
#include "fmt/ranges.h"
#include <algorithm>
#include <array>
#include <concepts>
#include <deque>
#include <map>
#include <type_traits>
#include <vector>
#include <set>
#include <ranges>


namespace Strawberry::Core::Math
{
	struct GraphTypeUndirected
	{
		using NodeID = unsigned int;
		static constexpr bool Weighted = false;
		static constexpr bool Directed = false;
	};

	struct GraphTypeDirected
	{
		using NodeID = unsigned int;
		static constexpr bool Weighted = false;
		static constexpr bool Directed = true;
	};


	template <typename Config>
	concept GraphConfig = requires(Config)
	{
		typename Config::NodeID;
		std::integral<typename Config::NodeID>;
		{ Config::Weighted } -> std::convertible_to<bool>;
		{ Config::Directed } -> std::convertible_to<bool>;
	} && (Config::Weighted == false || requires { typename Config::WeightType; });


	template <GraphConfig Config>
	struct Edge;

	template <GraphConfig Config>
	struct EdgeCommon
	{
		EdgeCommon(Config::NodeID a, Config::NodeID b) : nodes{ a, b }
		{
			AssertNEQ(a, b);

			// Puts edges in normalised order for undirected graphs
			if constexpr (!Config::Directed)
			{
				if (nodes[0] > nodes[1]) std::swap(nodes[0], nodes[1]);
			}
		}


		/// String formatter
		[[nodiscard]] std::string ToString() const
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
		bool ContainsNode(Config::NodeID node) const noexcept
		{
			return nodes[0] == node || nodes[1] == node;
		}


		/// Reverses the order of the nodes in this edge.
		void Reverse() requires (Config::Directed)
		{
			std::swap(nodes[0], nodes[1]);
		}


		auto operator<=>(const EdgeCommon& other) const = default;


		Config::NodeID A() const noexcept { return nodes[0]; }
		Config::NodeID B() const noexcept { return nodes[1]; }
		std::array<typename Config::NodeID, 2> Nodes() const { return nodes; }


		std::array<typename Config::NodeID, 2> nodes;
	};


	template <GraphConfig Config> requires (!Config::Weighted)
	struct Edge<Config>
		: EdgeCommon<Config>
	{
		using EdgeCommon<Config>::EdgeCommon;
	};


	template <GraphConfig Config> requires (Config::Weighted)
	struct Edge<Config>
		: EdgeCommon<Config>
	{
	public:
		Edge(Config::NodeID a, Config::NodeID b, Config::WeightType weight = 0)
			: EdgeCommon<Config>(a, b)
			, mWeight(weight)
		{}

		const auto& Weight() const { return mWeight; }

		void SetWeight(Config::WeightType weight) { mWeight = weight; }

	private:
		Config::WeightType mWeight;
	};


	template <GraphConfig Config>
	struct GraphEdgeStorage;


	template <GraphConfig Config> requires (!Config::Weighted)
	struct GraphEdgeStorage<Config>
	{
		unsigned int Count() const { return static_cast<unsigned int>(mEdges.size()); }


		mutable std::map<typename Config::NodeID, std::set<typename Config::NodeID>> mEdges;
	};


	template <GraphConfig Config> requires (Config::Weighted)
	struct GraphEdgeStorage<Config>
	{
		unsigned int Count() const { return static_cast<unsigned int>(mEdges.size()); }


		mutable std::map<typename Config::NodeID, std::set<typename Config::NodeID>> mEdges;
		std::map<EdgeCommon<Config>, typename Config::WeightType> mWeights;
	};


	template <typename _value, GraphConfig _config>
	class Graph
	{
	public:
		using Value = _value;
		using Config = _config;
		using NodeID = unsigned int;
		using Edge = Edge<Config>;


		Value& GetValue(NodeID nodeIndex)
		{
			return mNodes.at(nodeIndex);
		}


		const Value& GetValue(NodeID nodeIndex) const
		{
			return mNodes.at(nodeIndex);
		}


		[[nodiscard]] unsigned int NodeCount() const
		{
			return mNodes.size();
		}


		auto Nodes() const noexcept
		{
			return std::ranges::views::all(mNodes);
		}


		[[nodiscard]] unsigned int InDegree(NodeID node) const requires (Config::Directed)
		{
			unsigned int result = 0;
			for (auto edge : mEdgeStorage.mEdges)
			{
				if (edge.first == node) result++;
			}
			return result;
		}


		[[nodiscard]] unsigned int OutDegree(NodeID node) const requires (Config::Directed)
		{
			unsigned int result = 0;
			for (auto edge : mEdgeStorage.mEdges)
			{
				if (edge.second == node) result++;
			}
			return result;
		}


		[[nodiscard]] unsigned int Degree(NodeID node) const
		{
			if constexpr (Config::Directed)
			{
				return OutDegree();
			}
			else
			{
				unsigned int result = 0;
				for (auto edge : mEdgeStorage.mEdges)
				{
					if (edge.first == node) result++;
				}
				return result;
			}
		}


		auto NodeIndices() const noexcept
		{
			return mNodes | std::views::keys;
		}


		auto Values() const noexcept
		{
			return mNodes | std::views::values;
		}


		[[nodiscard]] bool ContainsValue(const Value& value) const noexcept
		{
			return std::ranges::find_if(
				mNodes,
				[value] (const auto& x) { return x.second == value; }) != mNodes.end();
		}


		template <typename T> requires (std::same_as<Value, std::decay_t<T>>)
		NodeID AddNode(T&& node)
		{
			mNodes.insert({mNextID++, std::forward<T>(node)});
			return mNextID - 1;
		}


		template <std::convertible_to<Value> T>
		void UpdateNode(NodeID id, T&& value)
		{
			mNodes.at(id) = std::forward<T>(value);
		}


		/// Removes the node with the given index, as well as all it's connected edges.
		void RemoveNode(NodeID index)
		{
			auto affectedEdges =  Edges()
				| std::views::filter([index] (const Edge& e) { return e.ContainsNode(index); })
				| std::ranges::to<std::vector>();


			for (auto edge : affectedEdges)
			{
				RemoveEdge(edge);
			}

			mNodes.erase(index);
		}


		void AddEdge(const Edge& e)
		{
			Assert(mNodes.contains(e.A()));
			Assert(mNodes.contains(e.B()));
			mEdgeStorage.mEdges[e.A()].emplace(e.B());
			if constexpr (!Config::Directed)
			{
				mEdgeStorage.mEdges[e.B()].emplace(e.A());
				if constexpr (Config::Weighted)
				{
					mEdgeStorage.mWeights[{e.B(), e.A()}] = e.Weight();
				}
			}
			if constexpr (Config::Weighted)
			{
				mEdgeStorage.mWeights[e] = e.Weight();
			}
		}


		void RemoveEdge(const Edge& e)
		{
			mEdgeStorage.mEdges[e.A()].erase(e.B());
			if (!Config::Directed)
			{
				mEdgeStorage.mEdges[e.B()].erase(e.A());
			}
		}


		[[nodiscard]] unsigned int EdgeCount() const
		{
			std::set<Edge> edgeSet;
			for (auto edge : Edges())
			{
				edgeSet.emplace(edge);
			}
			return edgeSet.size();
		}


		Edge GetEdge(Config::NodeID a, Config::NodeID b) const
		{
			EdgeCommon<Config> edge(a, b);
			Edge result(edge.A(), edge.B());

			if constexpr (Config::Weighted)
			{
				result.SetWeight(mEdgeStorage.mWeights.at(edge));
			}

			return result;
		}


		std::set<Edge> Edges() const noexcept
		{
			return mEdgeStorage.mEdges
				| std::views::transform( [this] (auto&& x)
				{
					return x.second | std::views::transform([this, x] (const auto x2) { return GetEdge(x.first, x2); });
				})
				| std::views::join
				| std::ranges::to<std::set>();
		}


		[[nodiscard]] bool IsConnected(NodeID nodeAIndex, NodeID nodeBIndex) const
		{
			Edge edge(nodeAIndex, nodeBIndex);
			return mEdgeStorage.mEdges[edge.A()].contains(edge.B());
		}


		bool IsConnected(const Edge& edge) const
		{
			return mEdgeStorage.mEdges[edge.A()].contains(edge.B());
		}


		[[nodiscard]] std::set<NodeID> GetNeighbours(NodeID node) const
		{
			std::set<NodeID> neighbours = mEdgeStorage.mEdges[node];
			return neighbours;
		}


		[[nodiscard]] std::set<NodeID> GetIncomingNeighbours(NodeID node) const requires (Config::Directed)
		{
			std::set<NodeID> incomingNeighbours;

			incomingNeighbours = mEdgeStorage.mEdges
				| std::views::filter([node] (const Edge& e) { return e.nodes[1] == node; })
				| std::views::transform([] (const Edge& e) { return e.nodes[0]; })
				| std::ranges::to<std::set>();

			return incomingNeighbours;
		}


		[[nodiscard]] std::set<NodeID> GetOutgoingNeighbours(NodeID node) const requires (Config::Directed)
		{
			return mEdgeStorage.mEdges[node];
		}


	private:
		/// Incrementing counter for generating node IDs
		NodeID mNextID = 0;
		/// The map of nodes, associates node ids to values.
		std::map<NodeID, Value> mNodes;
		/// The set of edges in this graph.
		GraphEdgeStorage<Config> mEdgeStorage;
	};


	template <typename T>
	using UndirectedGraph = Graph<T, GraphTypeUndirected>;
	template <typename T>
	using DirectedGraph = Graph<T, GraphTypeDirected>;


	template <GraphConfig Config>
	struct WeightedGraphConfig
	{
		using NodeID = Config::NodeID;
		static constexpr bool Weighted = true;
		using WeightType = unsigned int;
		static constexpr bool Directed = Config::Directed;
	};


	template <typename _graph>
	using WeightedGraph = Graph<typename _graph::Value, WeightedGraphConfig<typename _graph::Config>>;


	template <typename T, GraphConfig Config>
	class VectorGraph;

	template <typename T, unsigned int D, GraphConfig Config>
	class VectorGraph<Vector<T, D>, Config>
		: public Graph<Vector<T, D>, Config>
	{
	public:
		using Graph<Vector<T, D>, Config>::Graph;



	private:
	};

	template <typename T>
	using DirectedVectorGraph = VectorGraph<T, GraphTypeDirected>;

	template <typename T>
	using UndirectedVectorGraph = VectorGraph<T, GraphTypeUndirected>;
}
