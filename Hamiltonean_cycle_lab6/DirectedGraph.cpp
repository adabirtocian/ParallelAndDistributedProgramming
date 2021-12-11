#include "DirectedGraph.hpp"
#include <time.h>
#include <iostream>


DirectedGraph::DirectedGraph(std::vector<Node*> nodes): hasHamiltoneanCycle{false}, nodes{nodes}
{
}

void DirectedGraph::findHamiltoneanCycleRecursive(Node& node, std::stack<Node*> visitedNodes)
{
	if (node.getVisited())
	{
		this->hasHamiltoneanCycle = true;
		this->printCycle(&node, visitedNodes);
		return;
	}

	node.setVisited(true);
	visitedNodes.push(&node);
	
	for (int i = 0; i < node.getGoToNodes().size(); ++i)
	{
		if (this->hasHamiltoneanCycle)
		{
			return;

		}
		Node* goToNode = node.getGoToNodes()[i];
		if (goToNode->getVisited())
		{
			this->hasHamiltoneanCycle = true;
			this->printCycle(goToNode, visitedNodes);
			return;
		}
		this->findHamiltoneanCycleRecursive(*goToNode, visitedNodes);
	}
}

void DirectedGraph::findHamiltoneanCycle(int threads)
{
	std::stack<Node*> visitedNodes;
	this->findHamiltoneanCycleRecursive(*this->nodes[0], visitedNodes);
}

void DirectedGraph::printCycle(Node* node, std::stack<Node*> visitedNodes)
{
	std::vector<Node*> cycle;
	Node* currentNode = visitedNodes.top();
	visitedNodes.pop();
	cycle.push_back(node);

	while (currentNode->getInfo() != node->getInfo())
	{
		cycle.insert(cycle.begin(), currentNode);
		currentNode = visitedNodes.top();
		visitedNodes.pop();
	}
	cycle.insert(cycle.begin(), currentNode);

	for (auto node : cycle)
	{
		std::cout << *node;
	}
}
