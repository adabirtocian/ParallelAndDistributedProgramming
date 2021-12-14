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
	// if we already found the first solution
	if (this->hasHamiltoneanCycle != 0)
	{
		return;
	}

	// check for cycle: the current node is in fact the starting node for the cycle
	if (std::find(visitedNodes.begin(), visitedNodes.end(), &node) != visitedNodes.end())
	{
		int firstSolution = this->hasHamiltoneanCycle.fetch_or(1);

		// print the first solution
		if(firstSolution == 0) this->printCycle(node, visitedNodes);
		return;
	}

	// mark node as visited
	node.setVisited(true);
	visitedNodes.push_back(&node);
	
	if (threads > 1)
	{
		// split search between threads
		// go through all the possible edges starting from current node
		for (int i = 0; i < node.getGoToNodes().size() - 1; i += 2)
		{
			Node* goToNode = node.getGoToNodes()[i];
			// lauch async thread to continue the search from goToNode
			std::future<void> other = std::async(std::launch::async, [this, goToNode, visitedNodes, threads]() { this->findHamiltoneanCycleRecursive(*goToNode, visitedNodes, threads / 2);  });
			
			// take the next edge
			goToNode = node.getGoToNodes()[i + 1];
			// search on main thread starting from goToNode
			this->findHamiltoneanCycleRecursive(*goToNode, visitedNodes, threads - threads / 2);

			// wait for async thread to finish
			other.wait();
		}

		// if odd number of edges, ensure to explore the last one as well
		if (node.getGoToNodes().size() % 2 == 1)
		{
			// continue the search with the remaining available threads
			this->findHamiltoneanCycleRecursive(*node.getGoToNodes()[node.getGoToNodes().size() - 1], visitedNodes, threads - threads / 2);
		}
	}
	else
	{
		// continue the search sequentially on one thread
		for (int i = 0; i < node.getGoToNodes().size(); ++i)
		{
			Node* goToNode = node.getGoToNodes()[i];
			this->findHamiltoneanCycleRecursive(*goToNode, visitedNodes, threads);
		}
	}
}

void DirectedGraph::findHamiltoneanCycle(int threads)
{
	// try each node as starting node for the cycle
	for (auto node : this->nodes)
	{
		// consider it when not already visited
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
