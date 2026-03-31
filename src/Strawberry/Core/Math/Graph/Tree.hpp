#pragma once

#include "Strawberry/Core/Markers.hpp"
#include <bitset>
#include <concepts>
#include <map>
#include <vector>
#include <algorithm>
#include <set>

#include "Strawberry/Core/Types/Optional.hpp"

namespace Strawberry::Core::Math
{
	enum class VisitMode
	{
		Before, Middle, After,
	};


	template <typename Config>
	concept TreeConfigType = requires ()
	{
		typename Config::NodeID;
		{ Config::ChildCount } -> std::convertible_to<unsigned int>;
		{ Config::Ordered } -> std::convertible_to<bool>;
		{ Config::Sorted } -> std::convertible_to<bool>;
	}
	&& (Config::Sorted == false || (Config::Ordered == true && requires { typename Config::SortingFunction; }));


	template <typename T, TreeConfigType Config>
	class Tree;


	template <typename Value, TreeConfigType Config>
	struct TreeStorage;


	template <typename Value, TreeConfigType Config> requires (Config::ChildCount == 0)
	struct TreeStorage<Value, Config>
	{
		std::map<typename Config::NodeID, Value>                                mValues;
		std::map<typename Config::NodeID, std::vector<typename Config::NodeID>> childrenMap;
	};


	template <typename Value, TreeConfigType Config> requires (Config::ChildCount > 0)
	struct TreeStorage<Value, Config>
	{
		struct ChildArray;

		std::map<typename Config::NodeID, Value>      mValues;
		std::map<typename Config::NodeID, ChildArray> childrenMap;
	};


	template <typename Value, TreeConfigType Config> requires (Config::ChildCount > 0)
	struct TreeStorage<Value, Config>::ChildArray
	{
	public:
		void AddNode(Config::NodeID node) requires (!Config::Sorted)
		{
			Assert(ChildCount() < Config::ChildCount);
			for (int i = 0; i < Config::ChildCount; i++)
			{
				if (!mFlags[i])
				{
					mFlags.set(i);
					mValues[i] = node;
					return;
				}
			}
			Unreachable();
		}

		void AddNode(Config::NodeID node, const std::map<typename Config::NodeID, Value>& values) requires (Config::Sorted)
		{
			auto pos = std::ranges::lower_bound(
				mValues.begin(), std::next(mValues.begin(), ChildCount()),
				values.at(node),
				typename Config::SortingFunction{},
				[&] (const auto& x) { return values.at(x); });
			Insert(std::distance(mValues.begin(), pos), node);
		}

		void Insert(unsigned int index, Config::NodeID node)
		{
			Assert(ChildCount() < Config::ChildCount);
			do
			{
				std::swap(mValues[index], node);
			} while (index < Config::ChildCount && mFlags[index++]);
			mFlags.set(index - 1);
		}

		bool Contains(Config::NodeID node) const
		{
			for (int i = 0; i < Config::ChildCount; i++)
			{
				if (mFlags[i] && mValues[i] == node)
				{
					return true;
				}
			}
			return false;
		}


		std::vector<typename Config::NodeID> AsVector() const
		{
			std::vector<typename Config::NodeID> result;
			result.reserve(Config::ChildCount);
			for (int i = 0; i < Config::ChildCount; i++)
			{
				if (mFlags[i])
				{
					result.emplace_back(mValues[i]);
				}
			}
			return result;
		}

		unsigned int ChildCount() const
		{
			unsigned int count = 0;
			for (int i = 0; i < Config::ChildCount; i++)
			{
				if (mFlags[i]) count++;
			}
			return count;
		}

		Config::NodeID operator[](unsigned int i) const
		{
			Assert(mFlags[i]);
			return mValues[i];
		}


	private:
		std::bitset<Config::ChildCount> mFlags = { 0 };
		std::array<typename Config::NodeID, Config::ChildCount> mValues = { 0 };
	};


	template <typename _Value, TreeConfigType _Config>
	class Tree
	{
	public:
		using Value = _Value;
		using Config = _Config;

		Tree(Value root)
		{
			mRoot = 0;
			mStorage.mValues.emplace(0, std::move(root));
		}


		Config::NodeID Root() const
		{
			return mRoot;
		}


		const Value& GetValue(Config::NodeID node) const
		{
			return mStorage.mValues.at(node);
		}


		Config::NodeID AddNode(Config::NodeID parent, Value value)
		{
			typename Config::NodeID id = mStorage.mValues.rbegin()->first + 1;
			mStorage.mValues.emplace(id, std::move(value));
			if constexpr (Config::ChildCount == 0)
			{
				if constexpr (Config::Sorted)
				{
					auto pos = std::ranges::lower_bound(
						mStorage.childrenMap[parent],
						value,
						Config::SortingFunction,
						[this] (const auto& x) { return GetValue(x); });
					mStorage.childrenMap[parent].insert(pos, id);
				}
				else
				{
					mStorage.childrenMap[parent].emplace_back(id);
				}
			}
			else
			{
				if constexpr (Config::Sorted)
				{
					mStorage.childrenMap[parent].AddNode(id, mStorage.mValues);
				}
				else
				{
					mStorage.childrenMap[parent].AddNode(id);
				}
			}
			return id;
		}


		Config::NodeID InsertNode(Config::NodeID parent, unsigned int index, Value value) requires (Config::Ordered)
		{
			typename Config::NodeID id = mStorage.mValues.rbegin()->first + 1;
			mStorage.mValues.emplace(id, std::move(value));
			if constexpr (Config::ChildCount == 0)
			{
				mStorage.childrenMap[parent].insert(index, id);
			}
			else
			{
				mStorage.childrenMap[parent].Insert(index, id);
			}
			return id;
		}

		const auto& GetChildren(Config::NodeID parent) const
		{
			return mStorage.childrenMap[parent];
		}

		auto GetChildCount(Config::NodeID parent) const
		{
			if constexpr (Config::ChildCount == 0)
			{
				return mStorage.childrenMap.at(parent).size();
			}
			else
			{
				return mStorage.childrenMap.at(parent).ChildCount();
			}
		}


		Optional<typename Config::NodeID> FindParent(Config::NodeID node) const
		{
			for (const auto& [parent, children] : mStorage.childrenMap)
			{
				if constexpr (Config::ChildCount == 0)
				{
					if (children.contains(node))
					{
						return parent;
					}
				}
				else
				{
					if (children.Contains(node))
					{
						return parent;
					}
				}

			}

			return NullOpt;
		}


		std::map<typename Config::NodeID, typename Config::NodeID> GenerateParentMap() const
		{
			std::map<typename Config::NodeID, typename Config::NodeID> map;
			Visit([&] (const VisitContext& visitContext) {
				if (visitContext.path.size() >= 2)
				{
					map.emplace(visitContext.CurrentNode(), visitContext.GetParent().Value());
				}
			});
			return map;
		}


		struct VisitContext
		{
			VisitContext(const Tree& tree)
			{
				this->tree = &tree;
				this->path = { tree.Root() };
			}

			Config::NodeID CurrentNode() const
			{
				return path.back();
			}

			const auto& Value() const
			{
				return tree->GetValue(this->CurrentNode());
			}

			Optional<typename Config::NodeID> GetParent() const noexcept
			{
				if (path.size() >= 2) [[likely]]
					return path[path.size() - 2];
				else
					return NullOpt;
			}

			void GoUp()
			{
				path.pop_back();
			}

			const Tree*                          tree;
			std::vector<typename Config::NodeID> path;
			std::set<typename Config::NodeID>    visitedNodes;
		};


		template <VisitMode Mode = VisitMode::Before> requires (Config::Ordered)
		void Visit(this auto& self, auto&& fn)
		{
			self.template Visit<Mode>(std::forward<decltype(fn)>(fn), VisitContext(self));
		}

		template <VisitMode Mode> requires (Config::Ordered)
		void Visit(this auto& self, auto&& fn, VisitContext visitContext) requires (std::invocable<decltype(fn), const VisitContext&>)
		{
			const auto INVOKE = [&] {
				std::invoke(std::forward<decltype(fn)>(fn), static_cast<const VisitContext&>(visitContext));
				visitContext.visitedNodes.emplace(visitContext.CurrentNode());
			};

			AssertImplication(Mode == VisitMode::Middle, Config::ChildCount == 2);
			while (visitContext.path.size() > 0)
			{
				if (Mode == VisitMode::Before && !visitContext.visitedNodes.contains(visitContext.CurrentNode()))
				{
					INVOKE();
				}

				Optional<typename Config::NodeID> next;
				unsigned int childIndex = 0;
				auto children = self.GetChildren(visitContext.CurrentNode());
				const auto ChildCount = self.GetChildCount(visitContext.CurrentNode());
				for (childIndex = 0; childIndex < ChildCount; childIndex++)
				{
					if (!visitContext.visitedNodes.contains(children[childIndex]))
					{
						next = children[childIndex];
						break;
					}
				}

				if (next)
				{
					if (Mode == VisitMode::Middle && childIndex == 1 && !visitContext.visitedNodes.contains(visitContext.CurrentNode()))
					{
						INVOKE();
					}

					visitContext.path.emplace_back(next.Value());
				}
				else
				{
					if ((Mode == VisitMode::Middle || Mode == VisitMode::After) && !visitContext.visitedNodes.contains(visitContext.CurrentNode()))
					{
						INVOKE();
					}

					visitContext.GoUp();
				}
			}
		}

	private:
		Config::NodeID mRoot;
		mutable TreeStorage<Value, Config> mStorage;
	};


	template <typename NodeIDType = unsigned int>
	struct TreeConfigOrderedTree
	{
		using NodeID = NodeIDType;
		static constexpr unsigned int ChildCount = 0;
		static constexpr bool Ordered = true;
		static constexpr bool Sorted = false;
	};


	template <unsigned int I, typename NodeIDType = unsigned int>
	struct TreeConfigNAryTree
	{
		using NodeID = NodeIDType;
		static constexpr unsigned int ChildCount = I;
		static constexpr bool Ordered = true;
		static constexpr bool Sorted = false;
	};


	template <typename Value, typename NodeIDType = unsigned int>
	using OrderedTree = Tree<Value, TreeConfigOrderedTree<NodeIDType>>;


	template <typename Value, unsigned int ChildCount, typename NodeIDType = unsigned int>
	using NAryTree = Tree<Value, TreeConfigNAryTree<ChildCount, NodeIDType>>;


	template <typename Value, typename NodeIDType = unsigned int>
	using BinaryTree = NAryTree<Value, 2, NodeIDType>;


	template <TreeConfigType Base, typename SORTING_FUNCTION>
	struct MakeSortedTreeConfig
	{
		using NodeID = Base::NodeID;
		static constexpr unsigned int ChildCount = Base::ChildCount;
		static constexpr bool Ordered = true;
		static constexpr bool Sorted = true;
		using SortingFunction = SORTING_FUNCTION;
	};

	template <typename _Tree, typename SORTING_FUNCTION = std::less<typename _Tree::Value>>
	using SortedTree = Tree<typename _Tree::Value, MakeSortedTreeConfig<typename _Tree::Config, SORTING_FUNCTION>>;
}
