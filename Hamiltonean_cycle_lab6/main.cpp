#include <iostream>
#include <vector>
#include <chrono>
#include "DirectedGraph.hpp"
#include "Node.hpp"

int main()
{
	std::vector<Node*> nodes;

	// 3 hamiltonean cycles:  2,3,4,5,2;   3,4,5,3;   7,8,9,7
	//Node node1 = Node(1);
	//Node node2 = Node(2);
	//Node node3 = Node(3);
	//Node node4 = Node(4);
	//Node node5 = Node(5);
	//Node node6 = Node(6);
	//Node node7 = Node(7);
	//Node node8 = Node(8);
	//Node node9 = Node(9);

	//node1.addGoToNode(&node2);
	//node1.addGoToNode(&node3);
	//node2.addGoToNode(&node3);
	//node3.addGoToNode(&node4);
	//node4.addGoToNode(&node6);
	//node4.addGoToNode(&node5);
	//node5.addGoToNode(&node2);
	//node5.addGoToNode(&node3);
	//node7.addGoToNode(&node8);
	//node8.addGoToNode(&node9);
	//node9.addGoToNode(&node7);

	//nodes.push_back(&node1);
	//nodes.push_back(&node2);
	//nodes.push_back(&node3);
	//nodes.push_back(&node4);
	//nodes.push_back(&node5);
	//nodes.push_back(&node6);
	//nodes.push_back(&node7);
	//nodes.push_back(&node8);
	//nodes.push_back(&node9);

	// no cycle
	Node node1 = Node(1);
	Node node2 = Node(2);
	Node node3 = Node(3);
	Node node4 = Node(4);
	Node node5 = Node(5);
	Node node6 = Node(6);

	node1.addGoToNode(&node2);
	node1.addGoToNode(&node3);
	node3.addGoToNode(&node4);
	node4.addGoToNode(&node6);
	node4.addGoToNode(&node5);

	//nodes.push_back(&node1);
	//nodes.push_back(&node2);
	//nodes.push_back(&node3);
	//nodes.push_back(&node4);
	//nodes.push_back(&node5);
	//nodes.push_back(&node6);

	// 1 hamiltonean cycle: 1,2,3,4,1
	/*Node node1 = Node(1);
	Node node2 = Node(2);
	Node node3 = Node(3);
	Node node4 = Node(4);

	node1.addGoToNode(&node2);
	node2.addGoToNode(&node3);
	node3.addGoToNode(&node4);
	node4.addGoToNode(&node1);
	
	nodes.push_back(&node1);
	nodes.push_back(&node2);
	nodes.push_back(&node3);
	nodes.push_back(&node4);*/


	// create the directed graph and run the algorithm
	DirectedGraph graph = DirectedGraph(nodes);
	
	std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	graph.findHamiltoneanCycle(5);
	std::chrono::milliseconds end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::cout << "\nTime: " << (end - start).count() << " ms \n\n";

	return 0;
}