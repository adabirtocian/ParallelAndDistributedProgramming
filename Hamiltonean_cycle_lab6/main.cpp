#include <iostream>
#include <vector>
#include "DirectedGraph.hpp"
#include "Node.hpp"

int main()
{
	std::vector<Node*> nodes;
	Node node1 = Node(1);
	Node node2 = Node(2);
	Node node3 = Node(3);
	Node node4 = Node(4);
	Node node5 = Node(5);
	Node node6 = Node(6);

	node1.addGoToNode(&node2);
	node1.addGoToNode(&node3);
	node2.addGoToNode(&node3);
	node3.addGoToNode(&node4);
	node4.addGoToNode(&node6);
	node4.addGoToNode(&node5);
	node5.addGoToNode(&node2);

	nodes.push_back(&node1);
	nodes.push_back(&node2);
	nodes.push_back(&node3);
	nodes.push_back(&node4);
	nodes.push_back(&node5);
	nodes.push_back(&node6);


	DirectedGraph graph = DirectedGraph(nodes);
	graph.findHamiltoneanCycle(5);

	return 0;
}