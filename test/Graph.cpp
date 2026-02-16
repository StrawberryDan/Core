#include "Strawberry/Core/Math/Graph.hpp"
#include "Strawberry/Core/Assert.hpp"


using namespace Strawberry::Core;
using namespace Math;


int main()
{
	Strawberry::Core::Math::UndirectedGraph<int> graphA;

	graphA.AddNode(0);
	graphA.AddNode(1);
	graphA.AddNode(2);

	graphA.AddEdge(0, 1);
	graphA.AddEdge(1, 2);
	graphA.AddEdge(2, 0);

	Assert(graphA.IsConnected(0, 1));
	Assert(graphA.IsConnected(1, 0));
	Assert(graphA.IsConnected(1, 2));
	Assert(graphA.IsConnected(2, 1));
	Assert(graphA.IsConnected(0, 2));
	Assert(graphA.IsConnected(2, 0));

	AssertEQ(graphA.GetNeighourIndices(0), std::set<unsigned int>{1, 2});
	AssertEQ(graphA.GetNeighourIndices(1), std::set<unsigned int>{0, 2});
	AssertEQ(graphA.GetNeighourIndices(2), std::set<unsigned int>{0, 1});

	graphA.AddNode(3);
	graphA.AddEdge(1, 3);
	graphA.AddEdge(2, 3);

	Assert(graphA.IsConnected(1, 3));
	Assert(graphA.IsConnected(2, 3));

	graphA.RemoveNode(0);


	Strawberry::Core::Math::DirectedGraph<int> graphB;

	graphB.AddNode(0);
	graphB.AddNode(1);
	graphB.AddNode(2);

	graphB.AddEdge(0, 1);
	graphB.AddEdge(1, 2);
	graphB.AddEdge(2, 0);

	Assert(graphB.IsConnected(0, 1));
	Assert(!graphB.IsConnected(1, 0));
	Assert(graphB.IsConnected(1, 2));
	Assert(!graphB.IsConnected(2, 1));
	Assert(!graphB.IsConnected(0, 2));
	Assert(graphB.IsConnected(2, 0));

	AssertEQ(graphB.GetOutgoingNeighbourIndices(0), std::set<unsigned int>{1});
	AssertEQ(graphB.GetOutgoingNeighbourIndices(1), std::set<unsigned int>{2});
	AssertEQ(graphB.GetOutgoingNeighbourIndices(2), std::set<unsigned int>{0});


	return 0;
}
