#pragma once
// Strawberry Core
#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <algorithm>


namespace Strawberry::Core::Math
{
	/// Base graph walker type.
	/// Can walk along any arbitrary graph type.
	template <typename Base>
	class GraphWalker
	{
	public:
		using Graph = Base;

		/// Constructor initialising the walker to a node in a graph.
		GraphWalker(Graph& graph, unsigned int node)
			: mGraph(&graph)
			, mNode(node)
		{}


		/// Getter for the value at the current node.
		decltype(auto) GetValue(this auto* self) noexcept
		{
			return self->mGraph->GetValue(self->CurrentNode());
		}


		/// Returns the index of the current node this walker is sat on.
		unsigned int CurrentNode() const noexcept { return mNode; }


		/// Returns the set of possible next steps the walker can take from its current location.
		std::set<unsigned int> PossibleSteps() const noexcept
		{
			if constexpr (Graph::Config::Directed::value)
			{
				return mGraph->GetOutgoingNeighbourIndices(this->mNode);
			}
			else
			{
				return mGraph->GetNeighbourIndices(this->mNode);
			}
		}


		/// Moves this walker to the node specified.
		///
		/// @param node Target node. Must be in the list of possible steps.
		void WalkTo(unsigned int node) noexcept
		{
			Assert(PossibleSteps().contains(node), "Attempt to walk to unavailable node in Graph::Walker::WalkTo!");
			mNode = node;
		}


	protected:
		/// Pointer to the graph we're walking.
		Graph*       mGraph;
		/// Current node.
		unsigned int mNode;
	};


	/// Graph walker which remembers it's path.
	template <typename Base>
	class PathGraphWalker
		: public GraphWalker<Base>
	{
	public:
		/// Alias to Graph type
		using Graph = GraphWalker<Base>::Graph;

		/// Constructor overload.
		/// Initialised mHistory
		PathGraphWalker(Graph& graph, unsigned int node)
			: GraphWalker<Graph>(graph, node)
			, mHistory{node}
		{}


		/// Overloads WalkTo to record the steps walked into the history list.
		void WalkTo(unsigned int node) noexcept
		{
			GraphWalker<Graph>::WalkTo(node);
			mHistory.emplace_back(node);
		}


		/// Returns the number of nodes this walker has visited.
		size_t PathLength() const noexcept
		{
			return mHistory.size();
		}


		/// Rewinds the node walker back index steps in it's history
		/// If index == 0, then this function does nothing.
		void GoBack(unsigned int index = 1) noexcept
		{
			Assert(mHistory.size() > index + 1);
			for (int i = 0; i < index; i++)
			{
				mHistory.pop_back();
			}
			this->mNode = mHistory.back();
		}


		/// Returns true if the current node has been visited at least once before the current position.
		bool HasVisitedCurrentNode() const noexcept
		{
			return std::ranges::count(this->mHistory, this->mNode) > 1;
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


	protected:
		/// Double ended queue of nodes this walker has visited.
		std::deque<unsigned int> mHistory;
	};


	/// Walker for graphs that use Vector<*, *> as a payload.
	///
	/// Provides special functions for navigating graphs in a space.
	template <typename Base>
	class VectorGraphWalker
		: public PathGraphWalker<Base>
	{
	public:
		/// Type alias for base graph.
		using Graph = PathGraphWalker<Base>::Graph;
		/// Expose base constructor.
		using PathGraphWalker<Base>::PathGraphWalker;

		/// Returns the vector pointing from the previous node to the current node.
		Graph::Value GetIncomingVector() const noexcept
		{
			Assert(this->PathLength() > 1,
				   "Call to VectorGraphWalker::GetIncomingDirection with path history length <= 1.");
			return this->mGraph->GetValue(this->mNode)
			     - this->mGraph->GetValue(this->GetPreviousNode());
		}

		/// Returns the vector pointing from the current node to the specified outgoing node.
		Graph::Value GetOutgoingVector(unsigned int outgoingNode) const noexcept
		{
			Assert(this->PossibleSteps().contains(outgoingNode));
			return this->mGraph->GetValue(outgoingNode)
			     - this->mGraph->GetValue(this->mNode);
		}

		/// Get the possible next steps of this walker, sorted in order of ascending
		/// CCW angle between the outgoing vector for that neighbour, and the incoming vector
		/// from the previous node.
		///
		/// @returns A vector of pairs of node indices, and the CCW between them and the
		///          incoming vector.
		std::vector<std::pair<unsigned int, Radians>> GetNeighboursCCW() const noexcept
		{
			/// Possible next steps.
			auto outgoingNeighbours = this->PossibleSteps();
			/// Invert the incoming vector so it points away from the current node,
			/// which mean s that the CCW difference is calculated correctly.
			auto incomingVector = -1.0 * this->GetIncomingVector();

			/// Get the outgoingNeighbours as a pair of the neighbour and it's CCW angle.
			auto pairs = outgoingNeighbours
				| std::views::transform([&,this] (unsigned int x) -> std::pair<unsigned int, Radians>
				{
					return {x, incomingVector.AngleBetweenCCW(this->GetOutgoingVector(x))};
				})
				| std::ranges::to<std::vector>();

			/// Sort the neighbours according to their
			std::ranges::sort(pairs, std::less{}, [] (auto x) { return x.second; });

			/// Return result.
			return pairs;
		}


		/// Attempts to walk
		bool TryWalkCCW() noexcept
		{
			Assert(this->PathLength() > 1,
				   "Cannot Try to walk CCW without having a path history size >= 2 to set initial direction.");

			/// Get neighbours in CCW order
			std::vector sortedNeighbours = this->GetNeighboursCCW();
			/// Hunt for next node whilst there are sorted neighbours
			/// Skip the most CCW node until we find one that is not the node we came from.
			while (sortedNeighbours.size() > 0)
			{
				if (sortedNeighbours[sortedNeighbours.size() - 1].first != this->GetPreviousNode)
				{
					this->WalkTo(sortedNeighbours[sortedNeighbours.size() - 1].first);
					return true;
				}

				sortedNeighbours.pop_back();
			}

			return false;
		}
	};
}
