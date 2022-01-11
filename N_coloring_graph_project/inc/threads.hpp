#pragma once
#include <vector>
#include <atomic>
#include "node.hpp"

class Threads
{
private:
	int maxNoColors;
	int usedColors;
	std::atomic<int> noSolution;
	std::vector<Node> solution;
	std::vector<Node*> nodes;

public:
	Threads(std::vector<Node*> nodes, int maxNoColors);
	void runWrapper(int noThreads);
	void run(Node& node, int noThreads);



};