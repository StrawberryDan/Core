#pragma once


#include "Strawberry/Core/Math/Graph/Tree.hpp"


namespace Strawberry::Core::Math
{
	enum class TreeWalkStyle
	{
		None,
		DepthFirst,
		BreadthFirst,
	};

	enum class DepthFirstOrderType
	{
		Before, Middle, After
	};

	template <typename Config>
	concept TreeWalkerConfig = requires(Config)
	{
		{ Config::WalkStyle } -> std::convertible_to<TreeWalkStyle>;
	}
	&& (Config::WalkStyle != TreeWalkStyle::DepthFirst || requires { { Config::DepthFirstOrder } -> std::convertible_to<DepthFirstOrderType>; });


	template <typename _Tree>
	class TreeWalker
	{
	public:
		using Tree= _Tree;


		TreeWalker(const Tree& tree)
			: TreeWalker(tree, tree.Root())
		{}

		TreeWalker(const Tree& tree, Tree::Config::NodeID node)
			: mTree(&tree)
			, mNode(node)
		{}


		Tree::Config::NodeID Node() const
		{
			return mNode;
		}


		const Tree::Value& GetValue() const
		{
			return mTree->GetValue(Node());
		}


	private:
		const Tree* mTree;
		Tree::Config::NodeID mNode;
	};


	template <typename Value, TreeConfigType Config>
	TreeWalker(const Tree<Value, Config>&) -> TreeWalker<Tree<Value, Config>>;
	template <typename Value, TreeConfigType Config>
	TreeWalker(const Tree<Value, Config>&, typename Config::NodeID) -> TreeWalker<Tree<Value, Config>>;
}