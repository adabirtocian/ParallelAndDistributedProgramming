#include "DirectedGraph.hpp"
#include <iostream>
#include <algorithm>


DirectedGraph::DirectedGraph(std::vector<Node*> nodes): hasHamiltoneanCycle{false}, nodes{nodes}
{
}

void DirectedGraph::findHamiltoneanCycleRecursive(Node& node, std::vector<Node*> visitedNodes)
{
	if (std::find(visitedNodes.begin(), visitedNodes.end(), &node) != visitedNodes.end())
	{
		this->hasHamiltoneanCycle = true;
		this->printCycle(&node, visitedNodes);
		return;
	}

	node.setVisited(true);
	visitedNodes.push_back(&node);
	
	for (int i = 0; i < node.getGoToNodes().size(); ++i)
	{
		if (this->hasHamiltoneanCycle)
		{
			return;

		}
		Node* goToNode = node.getGoToNodes()[i];
		this->findHamiltoneanCycleRecursive(*goToNode, visitedNodes);
	}
}

void DirectedGraph::findHamiltoneanCycle(int threads)
{
	for (auto node : this->nodes)
	{
		if (!node->getVisited())
		{
			std::vector<Node*> visitedNodes;
			this->findHamiltoneanCycleRecursive(*node, visitedNodes);
		}
	}
}

void DirectedGraph::printCycle(Node* node, std::vector<Node*> visitedNodes)
{
	std::cout << *node;
	int i = visitedNodes.size() - 1;
	Node* currentNode = visitedNodes[i];
	
	while (currentNode->getInfo() != node->getInfo())
	{
		std::cout << *currentNode;
		i--;
		currentNode = visitedNodes[i];
	}
	std::cout << *node;
}
