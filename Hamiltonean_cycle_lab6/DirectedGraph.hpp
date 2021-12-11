#pragma once
#include <vector>
#include <stack>
#include "Node.hpp"

class DirectedGraph
{
private:
	std::vector<Node*> nodes;
	bool hasHamiltoneanCycle;

	void findHamiltoneanCycleRecursive(Node& node, std::stack<Node*> visitedNodes);

public:
	DirectedGraph(std::vector<Node*> nodes);
	void findHamiltoneanCycle(int threads);
	void printCycle(Node* node, std::stack<Node*> visitedNodes);

};

