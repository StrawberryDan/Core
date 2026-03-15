#include "Strawberry/Core/Math/Graph/Tree.hpp"
#include "Strawberry/Core/Assert.hpp"


using namespace Strawberry::Core;



int main()
{
	Math::OrderedTree<Math::Tree<int>> tree(0);

	tree.AddNode(0, 1);
	tree.AddNode(0, 2);

	Assert(tree.GetChildren(0).size() == 2);

	Math::BinaryTree<int> binaryTree(0);
	binaryTree.AddNode(0, 2);
	binaryTree.InsertNode(0, 0, 1);

	Assert(binaryTree.GetValue(binaryTree.GetChildren(0)[0]) == 1);
	Assert(binaryTree.GetValue(binaryTree.GetChildren(0)[1]) == 2);

	return 0;
}
