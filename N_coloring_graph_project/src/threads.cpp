#include "../inc/threads.hpp"
#include <iostream>
#include <future>
#include <chrono>

Threads::Threads(std::vector<Node*> nodes, int maxNoColors): nodes(nodes), maxNoColors(maxNoColors), usedColors(1), noSolution(0)
{
	this->solution = std::vector<Node>();
}

void Threads::runWrapper(int noThreads)
{
    std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	for (auto node : this->nodes)
	{
		if (!node->getVisited())
		{	
			node->setVisited(true);
			this->run(*node, noThreads);
		}
	}
	if(this->noSolution) std::cout << "No solution\n";
	else
	{
		for (auto node : this->nodes)
		{
			std::cout << node->getId() << " color:" << node->getColorId() << "\n";
		}
	}
    std::chrono::milliseconds end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    std::cout << "\nTime: " << (end - start).count() << " ms \n\n";
}

void Threads::run(Node& node, int noThreads)
{
	for (int i = 0; i < int(node.getAdjacentNodes().size()); ++i)
	{
		Node* nextNode = node.getAdjacentNodes()[i];
		if (nextNode->getColorId() == node.getColorId())
		{
			nextNode->setColorId(nextNode->getColorId() + 1);
		}

		this->usedColors = std::max(this->usedColors, std::max(nextNode->getColorId(), node.getColorId()));
		if (this->usedColors > this->maxNoColors)
		{
			std::cout << "Colors: " << this->usedColors << "\n";
			/*for (auto node : this->nodes)
			{
				std::cout << node->getId() << " color:" << node->getColorId() << "\n";
			}*/
			this->noSolution.fetch_or(1);
			
			return;
		}

	}

    std::vector<Node*> unvisitedNodes = std::vector<Node*>();
    for (int i = 0; i < node.getAdjacentNodes().size(); i++){
        Node* adjNode = node.getAdjacentNodes()[i];
        if (!adjNode->getVisited())
        {
            unvisitedNodes.push_back(adjNode);
        }
    }

    if (noThreads == 1){
        for (int i = 0; i < unvisitedNodes.size(); ++i)
        {
            Node* nextNode = unvisitedNodes[i];
            nextNode->setVisited(true);
            this->run(*nextNode, 1);
        }
    }
    else if (noThreads >= unvisitedNodes.size()) {
        std::vector<std::future<void> > futures = std::vector<std::future<void> >();
        int newThreadNo = noThreads / int(unvisitedNodes.size());
        for (int i = 0; i < unvisitedNodes.size(); ++i)
        {
            Node* nextNode = unvisitedNodes[i];
            nextNode->setVisited(true);
            futures.push_back(std::async(std::launch::async, [this, nextNode, newThreadNo]() { this->run(*nextNode, newThreadNo); }));
        }
        for (int i = 0; i < futures.size(); i++){
            futures[i].wait();
        }
    }
    else {
        std::vector<std::future<void> > futures = std::vector<std::future<void> >();
        for (int i = 0; i< noThreads; i++)
        {
            Node* nextNode = unvisitedNodes[i];
            nextNode->setVisited(true);
            futures.push_back(std::async(std::launch::async, [this, nextNode]() { this->run(*nextNode, 1); }));
        }

        for (int i = noThreads; i< unvisitedNodes.size(); i++){
            Node* nextNode = unvisitedNodes[i];
            nextNode->setVisited(true);
            this->run(*nextNode, 1);
        }
        for (int i = 0; i < futures.size(); i++){
            futures[i].wait();
        }
    }

}


