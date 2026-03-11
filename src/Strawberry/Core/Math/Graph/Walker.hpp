#pragma once


#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
#include <algorithm>
#include <concepts>


namespace Strawberry::Core::Math
{
	template <typename Base>
	class GraphWalker
	{
	public:
		using Graph = Base;


		GraphWalker(Graph& graph, unsigned int node)
			: mGraph(&graph)
			, mNode(node)
		{}


		unsigned int CurrentNode() const noexcept { return mNode; }


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


		void WalkTo(unsigned int node) noexcept
		{
			Assert(PossibleSteps().contains(node), "Attempt to walk to unavailable node in Graph::Walker::WalkTo!");
			mNode = node;
		}


	protected:
		Graph*       mGraph;
		unsigned int mNode;
	};


	template <typename Base>
	class PathGraphWalker
		: public GraphWalker<Base>
	{
	public:
		using Graph = GraphWalker<Base>::Graph;

		using GraphWalker<Base>::GraphWalker;

		PathGraphWalker(Graph& graph, unsigned int node)
			: GraphWalker<Graph>(graph, node)
		{
			mHistory.emplace_back(node);
		}


		void WalkTo(unsigned int node) noexcept
		{
			GraphWalker<Graph>::WalkTo(node);
			mHistory.emplace_back(node);
		}


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
		std::deque<unsigned int> mHistory;
	};


	template <typename Base>
	class VectorGraphWalker
		: public PathGraphWalker<Base>
	{
	public:
		using PathGraphWalker<Base>::PathGraphWalker;


		using Graph = PathGraphWalker<Base>::Graph;


		Graph::Value GetIncomingVector() const noexcept
		{
			Assert(this->PathLength() > 1,
				   "Call to VectorGraphWalker::GetIncomingDirection with path history length <= 1.");
			return this->mGraph->GetValue(this->mNode)
			     - this->mGraph->GetValue(this->GetPreviousNode());
		}


		Graph::Value GetOutgoingVector(unsigned int outgoingNode) const noexcept
		{
			Assert(this->PossibleSteps().contains(outgoingNode));
			return this->mGraph->GetValue(outgoingNode)
			     - this->mGraph->GetValue(this->mNode);
		}


		std::vector<std::pair<unsigned int, Radians>> GetNeighboursCCW() const noexcept
		{
			auto outgoingNeighbours = this->PossibleSteps();
			auto incomingVector = this->GetIncomingVector();

			auto pairs = outgoingNeighbours
				| std::views::transform([&,this] (unsigned int x) -> std::pair<unsigned int, Radians>
				{
					return {x, std::move((-1.0 * incomingVector).AngleBetweenCCW(this->GetOutgoingVector(x)))};
				})
				| std::ranges::to<std::vector>();

			std::ranges::sort(pairs, std::less{}, [] (auto x) { return x.second; });

			return pairs;
		}


		bool TryWalkCCW() noexcept
		{
			Assert(this->PathLength() > 1,
				   "Cannot Try to walk CCW without having a path history size >= 2 to set initial direction.");

			auto sortedNeighbours = this->GetNeighboursCCW()
				| std::views::filter([this] (auto x) { return x.first != this->GetPreviousNode(); })
				| std::ranges::to<std::vector>();

			// We cannot walk forward. This one neighbour must be where we came from.
			if (sortedNeighbours.size() == 0)
				return false;

			this->WalkTo(sortedNeighbours[sortedNeighbours.size() - 1].first);
			return true;
		}
	};
}
