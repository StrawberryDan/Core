#pragma once


#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Graph/Walker.hpp"
#include <array>
#include <bitset>
#include <concepts>
#include <functional>


namespace Strawberry::Core::Math
{
	template <typename T>
	class Tree
		: public DirectedGraph<T>
	{
	public:
		using Graph = DirectedGraph<T>;
		using NodeID = Graph::NodeID;

		Tree(T rootValue)
		{
			mRoot = AddNode(std::move(rootValue));
		}

		NodeID Root() const
		{
			return mRoot;
		}

		NodeID AddNode(NodeID parent, T value)
		{
			NodeID node = AddNode(std::move(value));
			AddEdge({parent, node});
			return node;
		}

		void RemoveNode(NodeID node)
		{
			for (NodeID child : GetChildren(node))
			{
				RemoveNode(child);
			}
			DirectedGraph<T>::RemoveNode(node);
		}

		std::set<NodeID> GetChildren(NodeID parent) const
		{
			return DirectedGraph<T>::GetOutgoingNeighbours(parent);
		}


	private:
		using DirectedGraph<T>::AddNode;
		using DirectedGraph<T>::AddEdge;
		using DirectedGraph<T>::RemoveEdge;

		NodeID mRoot;
	};


	template <typename Base>
	concept TreeGraphType = requires (Base base)
	{
		GraphType<Base>;
		std::derived_from<Base, Tree<typename Base::Value>>;
	};


	template <typename Base>
	class OrderedTree;


	template <TreeGraphType Base>
	class OrderedTree<Base>
		: public Base
	{
	public:
		using NodeID = Base::NodeID;
		using Value = Base::Value;


		using Base::Base;


		NodeID AddNode(NodeID parent, Value value)
		{
			NodeID child = Base::AddNode(parent, std::move(value));
			mParentChildMap[parent].emplace_back(child);
			return child;
		}

		NodeID InsertNode(NodeID parent, NodeID index, Value value)
		{
			NodeID child = Base::AddNode(parent, std::move(value));
			mParentChildMap[parent].insert(mParentChildMap.begin() + index, child);
		}

		void RemoveNode(NodeID node)
		{
			for (NodeID child : this->GetChildren(node))
			{
				RemoveNode(child);
			}
			Base::RemoveNode(node);
			mParentChildMap.erase(node);
			for (auto& [parent, children] : mParentChildMap)
			{
				std::erase_if(children, [node] (NodeID child) { return child == node; });
			}
		}

		Optional<NodeID> GetChildIndex(NodeID parent, NodeID child) const
		{
			const auto& children = mParentChildMap[parent];
			for (int i = 0; i < children.size(); i++)
			{
				if (children[i] == child) [[unlikely]]
				{
					return i;
				}
			}
			return NullOpt;
		}

		std::vector<NodeID> GetChildren(NodeID parent) const
		{
			return mParentChildMap[parent];
		}


	protected:
		mutable std::map<NodeID, std::vector<NodeID>> mParentChildMap;
	};


	template <typename Base>
	concept OrderedTreeGraphType = requires (Base base)
	{
		TreeGraphType<Base>;
		std::derived_from<Base, OrderedTree<typename Base::Value>>;
	};


	template <TreeGraphType Base, unsigned int _ChildCount>
	class SizedTree
		: public Base
	{
	public:
		using NodeID = Base::NodeID;
		static constexpr auto ChildCount = _ChildCount;

		using Base::Base;


		NodeID AddNode(NodeID parent, Base::Value value)
		{
			Assert(std::ranges::size(this->GetChildren(parent)) < ChildCount,
				   "Attempt to overfill node in SizedTree!");
			return Base::AddNode(parent, value);
		}
	};


	template <typename Base>
	concept SizedTreeGraphType = requires (Base base)
	{
		TreeGraphType<Base>;
		{ Base::ChildCount } -> std::same_as<unsigned int>;
		std::derived_from<Base, SizedTree<typename Base::Graph, Base::ChildCount>>;
	};


	template <TreeGraphType Base, unsigned int I>
	class OrderedTree<SizedTree<Base, I>>
		: public SizedTree<Base, I>
	{
	public:
		using SizedTree<Base, I>::SizedTree;
		using NodeID = SizedTree<Base, I>::NodeID;


		NodeID AddNode(NodeID parent, Base::Value value)
		{
			NodeID node = SizedTree<Base, I>::AddNode(parent, std::move(value));
			auto& childArray = mParentChildMap[parent];
			Assert(!childArray.Full());
			childArray.AddChild(node);
			return node;
		}


		NodeID InsertNode(NodeID parent, NodeID index, Base::Value value)
		{
			NodeID node = SizedTree<Base, I>::AddNode(parent, std::move(value));
			auto& childArray = mParentChildMap[parent];
			Assert(!childArray.Full());
			childArray.InsertNode(index, node);
			return node;
		}


		void RemoveNode(NodeID node)
		{
			for (auto child : GetChildren(node))
			{
				RemoveNode(child);
			}
			SizedTree<Base, I>::RemoveNode(node);
			mParentChildMap.erase(node);
			for (auto& [parent, children] : mParentChildMap)
			{
				children.RemoveNode(node);
			}
		}


		Optional<NodeID> GetChildIndex(NodeID parent, NodeID child) const
		{
			return mParentChildMap[parent].GetChildIndex(child);
		}


		std::vector<NodeID> GetChildren(NodeID parent) const
		{
			return mParentChildMap[parent].GetChildren();
		}


	protected:
		struct ChildArray
		{
			std::bitset<I> mFlags = 0;
			std::array<NodeID, I> mValues { 0 };


			void AddChild(NodeID node)
			{
				NodeID firstIndex = 0;
				while (firstIndex < I - 1 && mFlags[firstIndex]) firstIndex++;
				Assert(firstIndex < I);
				mFlags.set(firstIndex);
				mValues[firstIndex] = node;
			}


			void InsertNode(NodeID index, NodeID node)
			{
				if (mFlags[index])
				{
					NodeID nextZero = index;
					while (nextZero < I && mFlags[nextZero]) nextZero++;

					for (int i = nextZero; i > index; i--)
					{
						mFlags[i] = mFlags[i - 1];
						mFlags.reset(i - 1);
						mValues[i] = std::exchange(mValues[i - 1], 0);
					}
				}

				Assert(!mFlags[index]);
				mFlags.set(index);
				mValues[index] = node;
			}


			void RemoveChild(NodeID node)
			{
				for (int i = 0; i < I; i++)
				{
					bool match = mFlags[i] && mValues[i] == node;
					if (match)
					{
						mFlags.reset(i);
						mValues[i] = 0;
					}
				}
			}


			Optional<NodeID> GetChildIndex(NodeID child) const
			{
				for (int i = 0; i < I; i++)
				{
					if (mFlags[i] && mValues[i] == child) [[unlikely]]
					{
						return i;
					}
				}
				return NullOpt;
			}


			std::vector<NodeID> GetChildren() const
			{
				std::vector<NodeID> children;
				for (int i = 0; i < I; i++)
				{
					if (mFlags[i])
					{
						children.emplace_back(mValues[i]);
					}
				}
				return children;
			}


			bool Full() const
			{
				return mFlags.all();
			}
		};

		mutable std::map<NodeID, ChildArray> mParentChildMap;
	};


	template <typename T, unsigned int N>
	using NAryTree = OrderedTree<SizedTree<Tree<T>, N>>;


	template <typename T>
	using BinaryTree = NAryTree<T, 2>;


	template <typename Base, typename Comparison = std::less<typename Base::Value>>
	class SortedTree;


	/// Class representing a tree in which the order of children depends on
	/// their ordering with respect to the function Comparison.
	template <OrderedTreeGraphType Base, typename Comparison>
	class SortedTree<Base, Comparison>
		: public Base
	{
	public:
		using NodeID = Base::NodeID;


		/// Inserts the node into the tree into the properly ordered position.
		NodeID AddNode(NodeID parent, Base::Value value)
		{
			const auto& children = this->GetChildren(parent);
			int position = 0;
			Comparison comparison;

			while (position < children.size() && comparison(
					   this->GetValue(children[position]),
					   value))
			{
				position++;
			}

			return this->InsertNode(
				parent,
				children.empty() ? 0 : this->GetChildIndex(parent, children[position]).Unwrap(),
				std::move(value));
		}

	protected:
		/// Hide insert node as it would allow the breaking of ordering.
		using Base::InsertNode;
	};


	/// Class representing a graph walker over a tree.
	/// Keeps a stack of it's path from the root.
	template <GraphWalkerType Base>
	class TreeWalker
		: public Base
	{
	public:
		using NodeID = Base::NodeID;


		TreeWalker(const Base::Graph& tree, NodeID node)
			: Base(tree, tree.Root())
		{
			mPathStack.emplace_back(node);
		}


		TreeWalker(const Base& tree)
			: Base(tree, tree.Root())
		{
			mPathStack.emplace_back(tree.Root());
		}

		void WalkUp()
		{
			Assert(mPathStack.size() > 1);
			mPathStack.pop_back();
			Base::Jump(mPathStack.back());
		}

		void WalkTo(NodeID node)
		{
			mPathStack.emplace_back(node);
			Base::WalkTo(node);
		}


	private:
		std::deque<NodeID> mPathStack;
	};
}
