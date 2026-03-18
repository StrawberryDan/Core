#include "Strawberry/Core/Math/Graph/Tree.hpp"
#include "Strawberry/Core/Assert.hpp"


using namespace Strawberry::Core;



int main()
{
	Math::OrderedTree<int> tree(0);

	tree.AddNode(0, 1);
	tree.AddNode(0, 2);

	Assert(tree.GetChildren(0).size() == 2);

	Math::BinaryTree<int> binaryTree(0);
	binaryTree.AddNode(0, 2);
	binaryTree.InsertNode(0, 0, 1);

	Assert(binaryTree.GetValue(binaryTree.GetChildren(0)[0]) == 1);
	Assert(binaryTree.GetValue(binaryTree.GetChildren(0)[1]) == 2);

	Math::SortedTree<Math::BinaryTree<int>> sorted(0);
	sorted.AddNode(0, 1);
	sorted.AddNode(1, 3);
	sorted.AddNode(1, 2);
	sorted.AddNode(0, 4);
	sorted.AddNode(4, 6);
	sorted.AddNode(4, 5);

	std::vector<int> order;
	sorted.Visit<Math::VisitMode::Before>(
		[&] (int x) { order.emplace_back(x); });
	AssertEQ(order, std::vector<int>{0, 1, 2, 3, 4, 5, 6});
	order.clear();

	sorted.Visit<Math::VisitMode::Middle>([&] (int x) { order.emplace_back(x); });
	AssertEQ(order, std::vector<int>{2, 1, 3, 0, 5, 4, 6});
	order.clear();

	sorted.Visit<Math::VisitMode::After>([&] (int x) { order.emplace_back(x); });
	AssertEQ(order, std::vector<int>{2, 3, 1, 5, 6, 4, 0});
	order.clear();

	return 0;
}
