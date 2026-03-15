#pragma once


#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Graph/Walker.hpp"
#include <array>
#include <bitset>
#include <concepts>


namespace Strawberry::Core::Math
{
	template <typename T>
	class Tree
		: public DirectedGraph<T>
	{
	public:
		using Graph = DirectedGraph<T>;

		Tree(T rootValue)
		{
			mRoot = AddNode(std::move(rootValue));
		}

		unsigned int Root() const
		{
			return mRoot;
		}

		unsigned int AddNode(unsigned int parent, T value)
		{
			unsigned int node = AddNode(std::move(value));
			AddEdge({parent, node});
			return node;
		}

		void RemoveNode(unsigned int node)
		{
			for (unsigned int child : GetChildren(node))
			{
				RemoveNode(child);
			}
			DirectedGraph<T>::RemoveNode(node);
		}

		std::set<unsigned int> GetChildren(unsigned int parent) const
		{
			return DirectedGraph<T>::GetOutgoingNeighbourIndices(parent);
		}


	private:
		using DirectedGraph<T>::AddNode;
		using DirectedGraph<T>::AddEdge;
		using DirectedGraph<T>::RemoveEdge;

		unsigned int mRoot;
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
		using Value = Base::Value;


		using Base::Base;


		unsigned int AddNode(unsigned int parent, Value value)
		{
			unsigned int child = Base::AddNode(parent, std::move(value));
			mParentChildMap[parent].emplace_back(child);
			return child;
		}

		unsigned int InsertNode(unsigned int parent, unsigned int index, Value value)
		{
			unsigned int child = Base::AddNode(parent, std::move(value));
			mParentChildMap[parent].insert(mParentChildMap.begin() + index, child);
		}

		void RemoveNode(unsigned int node)
		{
			for (unsigned int child : this->GetChildren(node))
			{
				RemoveNode(child);
			}
			Base::RemoveNode(node);
			mParentChildMap.erase(node);
			for (auto& [parent, children] : mParentChildMap)
			{
				std::erase_if(children, [node] (unsigned int child) { return child == node; });
			}
		}

		std::vector<unsigned int> GetChildren(unsigned int parent) const
		{
			return mParentChildMap[parent];
		}


	private:
		mutable std::map<unsigned int, std::vector<unsigned int>> mParentChildMap;
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
		static constexpr auto ChildCount = _ChildCount;

		using Base::Base;


		unsigned int AddNode(unsigned int parent, Base::Value value)
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


		unsigned int AddNode(unsigned int parent, Base::Value value)
		{
			unsigned int node = SizedTree<Base, I>::AddNode(parent, std::move(value));
			auto& childArray = mParentChildMap[parent];
			Assert(!childArray.Full());
			childArray.AddChild(node);
			return node;
		}


		unsigned int InsertNode(unsigned int parent, unsigned int index, Base::Value value)
		{
			unsigned int node = SizedTree<Base, I>::AddNode(parent, std::move(value));
			auto& childArray = mParentChildMap[parent];
			Assert(!childArray.Full());
			childArray.InsertNode(index, node);
			return node;
		}


		void RemoveNode(unsigned int node)
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


		std::vector<unsigned int> GetChildren(unsigned int parent) const
		{
			return mParentChildMap[parent].GetChildren();
		}


	private:
		struct ChildArray
		{
			std::bitset<I> mFlags = 0;
			std::array<unsigned int, I> mValues { 0 };


			void AddChild(unsigned int node)
			{
				unsigned int firstIndex = 0;
				while (firstIndex < I - 1 && mFlags[firstIndex]) firstIndex++;
				Assert(firstIndex < I);
				mFlags.set(firstIndex);
				mValues[firstIndex] = node;
			}


			void InsertNode(unsigned int index, unsigned int node)
			{
				if (mFlags[index])
				{
					for (int i = I - 1; i > index; i--)
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


			void RemoveChild(unsigned int node)
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


			std::vector<unsigned int> GetChildren() const
			{
				std::vector<unsigned int> children;
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

		mutable std::map<unsigned int, ChildArray> mParentChildMap;
	};


	template <typename T, unsigned int N>
	using NAryTree = OrderedTree<SizedTree<Tree<T>, N>>;


	template <typename T>
	using BinaryTree = NAryTree<T, 2>;


	template <GraphWalkerType Base>
	class TreeWalker
		: public Base
	{
	public:
		TreeWalker(const Base::Graph& tree, unsigned int node)
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
			this->mNode = mPathStack.back();
		}





		void WalkTo(unsigned int node)
		{
			mPathStack.emplace_back(node);
			Base::WalkTo(node);
		}


	private:
		std::deque<unsigned int> mPathStack;
	};
}
