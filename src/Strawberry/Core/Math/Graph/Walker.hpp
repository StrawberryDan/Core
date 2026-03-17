#pragma once
// Strawberry Core
#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <algorithm>
#include <concepts>


namespace Strawberry::Core::Math
{
	/// Base graph walker type.
	/// Can walk along any arbitrary graph type.
	template <typename Base>
	class BasicGraphWalker
	{
	public:
		using Graph = Base;

		/// Constructor initialising the walker to a node in a graph.
		BasicGraphWalker(const Graph& graph, unsigned int node)
			: mGraph(&graph)
			, mNode(node)
		{}


		/// Getter for the value at the current node.
		decltype(auto) GetValue(this auto& self) noexcept
		{
			return self.mGraph->GetValue(self.CurrentNode());
		}


		/// Returns the index of the current node this walker is sat on.
		unsigned int CurrentNode(this const auto& self) noexcept { return self.mNode; }


		/// Returns the set of possible next steps the walker can take from its current location.
		std::set<unsigned int> NextNodes(this const auto& self) noexcept
		{
			if constexpr (Graph::Config::Directed::value)
			{
				return self.mGraph->GetOutgoingNeighbourIndices(self.CurrentNode());
			}
			else
			{
				return self.mGraph->GetNeighbours(self.CurrentNode());
			}
		}

		/// Returns whether a walker can walk to the given node in one step.
		bool CanWalkTo(this const auto& self, unsigned int node) noexcept
		{
			return self.NextNodes().contains(node);
		}


		/// Jumps this walker to another node without any requirement
		/// of there being a connection.
		void Jump(this auto& self, unsigned int node) noexcept
		{
			self.mNode = node;
		}


		/// Moves this walker to the node specified.
		///
		/// @param node Target node. Must be in the list of possible steps.
		void WalkTo(this auto& self, unsigned int node) noexcept
		{
			Assert(self.CanWalkTo(node), "Attempt to walk to unavailable node in Graph::Walker::WalkTo!");
			self.mNode = node;
		}


	protected:
		/// Pointer to the graph we're walking.
		const Graph* mGraph;
		/// Current node.
		unsigned int mNode;
	};


	template <typename Walker>
	concept GraphWalkerType = requires (Walker walker)
	{
		typename Walker::Graph;
		std::derived_from<Walker, BasicGraphWalker<typename Walker::Graph>>;
	};


	/// Graph walker which remembers it's path.
	template <GraphWalkerType Base>
	class PathGraphWalker
		: public Base
	{
	public:
		/// Alias to Graph type
		using Graph = Base::Graph;

		/// Constructor overload.
		/// Initialised mHistory
		PathGraphWalker(const Graph& graph, unsigned int node)
			: Base(graph, node)
			, mHistory{node}
		{}

		/// Construcotr from base.
		PathGraphWalker(Base&& base)
			: Base(std::move(base))
			, mHistory(this->CurrentNode())
		{}

		/// Returns the set of possible next nodes, excluding the node previously visited.
		std::set<unsigned int> ForwardNodes(this const auto& self) noexcept
		{
			auto possibleSteps = self.NextNodes();
			if (self.PathLength() > 1) [[likely]]
			{
				possibleSteps.erase(self.GetPreviousNode());
			}
			return possibleSteps;
		}

		/// Jumps to the givne node, and tries to change mHistory to include the last occurence of node in path.
		void Jump(unsigned int node) noexcept
		{
			Base::Jump(node);
			auto it = std::find(mHistory.rbegin(), mHistory.rend(), node);
			if (it == mHistory.rend())
			{
				mHistory.clear();
				mHistory.emplace_back(node);
			}
			else
			{
				mHistory.erase(it.base(), mHistory.end());
			}
			Assert(mHistory.back() == node);
		}

		/// Overloads WalkTo to record the steps walked into the history list.
		void WalkTo(this auto& self, unsigned int node) noexcept
		{
			self.Base::WalkTo(node);
			self.mHistory.emplace_back(node);
		}


		/// Returns the number of nodes this walker has visited.
		size_t PathLength(this const auto& self) noexcept
		{
			return self.mHistory.size();
		}


		/// Rewinds the node walker back index steps in it's history
		/// If index == 0, then this function does nothing.
		void GoBack(this const auto& self, unsigned int index = 1) noexcept
		{
			Assert(self.PathLength() > index + 1);
			for (int i = 0; i < index; i++)
			{
				self.mHistory.pop_back();
			}
			self.Jump(self.mHistory.back());
		}


		/// Returns true if the current node has been visited at least once before the current position.
		bool HasVisitedCurrentNode() const noexcept
		{
			return std::ranges::count(this->mHistory, this->mNode) > 1;
		}


		/// Returns whether this path begins and ends on the same node.
		bool IsLoop() const noexcept
		{
			return PathLength() > 1 && mHistory.front() == mHistory.back();
		}


		/// Returns the index of a previous node in the path.
		///
		/// @param index The number of steps back to return from in the history.
		///              Defaults to 1 which corresponds to the node before the current node.
		///              An index of zero will just return the current node.
		unsigned int GetPreviousNode(unsigned int index = 1) const noexcept
		{
			Assert(mHistory.size() >= index + 1);
			return mHistory[mHistory.size() - 1 - index];
		}


		bool TryWalkForwards(this auto& self)
		{
			auto forwardNodes = self.ForwardNodes();
			if (forwardNodes.empty())
			{
				return false;
			}

			self.WalkTo(*forwardNodes.begin());
			return true;
		}


	protected:
		/// Double ended queue of nodes this walker has visited.
		std::deque<unsigned int> mHistory;
	};

	template <GraphWalkerType Base>
	PathGraphWalker(Base) -> PathGraphWalker<Base>;


	/// Walker for graphs that use Vector<*, *> as a payload.
	///
	/// Provides special functions for navigating graphs in a space.
	template <GraphWalkerType Base>
	class VectorGraphWalker
		: public Base
	{
	public:
		/// Type alias for base graph.
		using Graph = Base::Graph;
		/// Expose base constructor.
		using Base::Base;

		VectorGraphWalker(Base&& base)
			: Base(std::move(base))
		{}

		/// Returns the vector pointing from the previous node to the current node.
		Graph::Value GetIncomingVector(this const auto& self) noexcept
		{
			Assert(self.PathLength() > 1,
				   "Call to VectorGraphWalker::GetIncomingDirection with path history length <= 1.");
			return self.mGraph->GetValue(self.mNode)
			     - self.mGraph->GetValue(self.GetPreviousNode());
		}

		/// Returns the vector pointing from the current node to the specified outgoing node.
		Graph::Value GetOutgoingVector(this const auto& self, unsigned int outgoingNode) noexcept
		{
			Assert(self.NextNodes().contains(outgoingNode));
			return self.mGraph->GetValue(outgoingNode)
			     - self.mGraph->GetValue(self.mNode);
		}

		/// Returns the neighbours of this current node in CCW order.
		std::vector<unsigned int> GetNeighboursCCW(this const auto& self) noexcept
		{
			/// Possible next steps.
			auto outgoingNeighbours = self.NextNodes() | std::ranges::to<std::vector>();

			/// Sort the neighbours according to their
			std::ranges::sort(
				outgoingNeighbours, std::less{},
				[self] (auto x) { return self.GetOutgoingVector(x).ATan2(); });

			/// Return result.
			return outgoingNeighbours;
		}

		/// Get the possible next steps of this walker, sorted in order of ascending
		/// CCW angle between the outgoing vector for that neighbour, and the incoming vector
		/// from the previous node.
		///
		/// @returns A vector of pairs of node indices, and the CCW between them and the
		///          incoming vector.
		std::vector<std::pair<unsigned int, Radians>> GetNeighboursCCWFromIncoming(this const auto& self) noexcept
		{
			/// Possible next steps.
			auto outgoingNeighbours = self.NextNodes();
			/// Invert the incoming vector so it points away from the current node,
			/// which mean s that the CCW difference is calculated correctly.
			auto incomingVector = -1.0 * self.GetIncomingVector();

			/// Get the outgoingNeighbours as a pair of the neighbour and it's CCW angle.
			auto pairs = outgoingNeighbours
				| std::views::transform([&, self] (unsigned int x) -> std::pair<unsigned int, Radians>
				{
					return {x, incomingVector.AngleBetweenCCW(self.GetOutgoingVector(x))};
				})
				| std::ranges::to<std::vector>();

			/// Sort the neighbours according to their
			std::ranges::sort(pairs, std::less{}, [] (auto x) { return x.second; });

			/// Return result.
			return pairs;
		}


		/// Attempts to walk to any node which is not the one it just came from.
		///
		/// @returns Whether the walker was able to take a step.
		bool TryWalkCCW(this auto& self) noexcept
		{
			Assert(self.PathLength() > 1,
				   "Cannot Try to walk CCW without having a path history size >= 2 to set initial direction.");

			/// Get neighbours in CCW order
			std::vector sortedNeighbours = self.GetNeighboursCCWFromIncoming();
			/// Hunt for next node whilst there are sorted neighbours
			/// Skip the most CCW node until we find one that is not the node we came from.
			while (sortedNeighbours.size() > 0)
			{
				if (sortedNeighbours[sortedNeighbours.size() - 1].first != self.GetPreviousNode())
				{
					self.WalkTo(sortedNeighbours[sortedNeighbours.size() - 1].first);
					return true;
				}

				sortedNeighbours.pop_back();
			}

			return false;
		}
	};

	template <GraphWalkerType Base>
	VectorGraphWalker(Base) -> VectorGraphWalker<Base>;


	/// Class representing a walker that can only travel within the set of boundNodes.
	template <GraphWalkerType Base>
	class BoundGraphWalker
		: public Base
	{
	public:
		BoundGraphWalker(Base&& base, std::set<unsigned int> boundNodes)
			: Base(std::move(base))
			, mBoundNodes(std::move(boundNodes))
		{}

		BoundGraphWalker(Base::Graph& graph, unsigned int node, std::set<unsigned int> boundNodes)
			: Base(graph, node)
			, mBoundNodes(std::move(boundNodes))
		{}

		/// Returns whether this walker is bound to this node.
		bool BoundToNode(unsigned int node) const noexcept
		{
			return mBoundNodes.contains(node);
		}

		/// NextNodes implementation only includes bound nodes in result.
		std::set<unsigned int> NextNodes(this const auto self) noexcept
		{
			return Base::NextNodes()
				| std::views::filter([&] (unsigned int n) { return self.BoundToNode(n); })
				| std::ranges::to<std::set>();
		}


	private:
		std::set<unsigned int> mBoundNodes;
	};


	template <GraphWalkerType Base>
	BoundGraphWalker(Base) -> BoundGraphWalker<Base>;
}
