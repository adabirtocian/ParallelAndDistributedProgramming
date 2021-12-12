#include "DirectedGraph.hpp"
#include <iostream>
#include <algorithm>
#include <future>
#include <thread>

DirectedGraph::DirectedGraph(std::vector<Node*> nodes): nodes{nodes}, hasHamiltoneanCycle(0)
{
}

void DirectedGraph::findHamiltoneanCycleRecursive(Node& node, std::vector<Node*> visitedNodes, int threads)
{
	if (std::find(visitedNodes.begin(), visitedNodes.end(), &node) != visitedNodes.end())
	{
		this->hasHamiltoneanCycle = this->hasHamiltoneanCycle.fetch_or(1);
		this->printCycle(node, visitedNodes);
		return;
	}

	node.setVisited(true);
	visitedNodes.push_back(&node);
	
	if (threads > 1)
	{
		for (int i = 0; i < node.getGoToNodes().size(); ++i)
		{
			if (this->hasHamiltoneanCycle)
			{
				return;

			}
			Node* goToNode = node.getGoToNodes()[i];
			this->findHamiltoneanCycleRecursive(*goToNode, visitedNodes, threads);
		}
	}
	else
	{
		// One thread available. Continue the search sequentially.

		for (int i = 0; i < node.getGoToNodes().size() - 1; i+=2)
		{
			if (this->hasHamiltoneanCycle)
			{
				return;

			}
			Node* goToNode = node.getGoToNodes()[i];
			std::future<void> other = std::async(std::launch::async, [this, goToNode, visitedNodes, threads]() { this->findHamiltoneanCycleRecursive(*goToNode, visitedNodes, threads / 2);  });
			
			goToNode = node.getGoToNodes()[i + 1];
			this->findHamiltoneanCycleRecursive(*goToNode, visitedNodes, threads - threads / 2);
			
			other.wait();
		}
		this->findHamiltoneanCycleRecursive(*node.getGoToNodes()[node.getGoToNodes().size()-1], visitedNodes, threads - threads / 2);
	}
}

void DirectedGraph::findHamiltoneanCycle(int threads)
{
	for (auto node : this->nodes)
	{
		if (!node->getVisited())
		{
			std::vector<Node*> visitedNodes;
			this->findHamiltoneanCycleRecursive(*node, visitedNodes, threads);
		}
	}
}

void DirectedGraph::printCycle(Node& node, std::vector<Node*> visitedNodes)
{
	auto it = std::find(visitedNodes.begin(), visitedNodes.end(), &node);
	std::cout << "Hamiltonean cycle: ";

	for (auto i = it; i != visitedNodes.end(); ++i)
	{
		std::cout << **i;
	}
	
	std::cout << node << "\n";
}
