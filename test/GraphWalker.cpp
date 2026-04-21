#include "Strawberry/Core/Math/Graph/Graph.hpp"
#include "Strawberry/Core/Math/Graph/GraphWalker.hpp"


int main()
{
	Strawberry::Core::Math::UndirectedGraph<int> graph;
	graph.AddNode(0);
	graph.AddNode(1);
	graph.AddNode(2);
	graph.AddNode(3);

	Strawberry::Core::Math::BasicGraphWalker<decltype(graph)> walker(graph, 0);


	return 0;
}
