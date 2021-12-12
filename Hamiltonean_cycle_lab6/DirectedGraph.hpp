#pragma once
#include <vector>
#include <stack>
#include <atomic>
#include "Node.hpp"

class DirectedGraph
{
private:
	std::vector<Node*> nodes;
	std::atomic<int> hasHamiltoneanCycle;
	//int hasHamiltoneanCycle;

	void findHamiltoneanCycleRecursive(Node& node, std::vector<Node*> visitedNodes, int threads);

public:
	DirectedGraph(std::vector<Node*> nodes);
	void findHamiltoneanCycle(int threads);
	void printCycle(Node& node, std::vector<Node*> visitedNodes);

};

