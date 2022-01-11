#pragma once
#include <vector>

class WithMPI
{
private:
	std::vector<int> colors;
	std::vector<int> visited;
	std::vector<std::vector<int>> adjacentNodeIds;
	int usedColors;
	int maxNoColors;
	bool noSolution;
    int receivedMessage;


	void runSequentialWrapper();
	void runSequential(int nodeId);
    int runWorker(int processId, int parentProcess, int usedColors, std::vector<int> colors, std::vector<int> visited, int nodeId, std::vector<int> processes);
    void sendData(int nodeId, int destination, std::vector<int> processes, int usedColors, std::vector<int> colors, std::vector<int> visited);
    void broadcastData(std::vector<int> newVisitedNodes, std::vector<int> processes);
    std::vector<int> receiveBroadcastData();
public:
	WithMPI(int noNodes, std::vector<std::vector<int>> adjacentNodeIds, int maxNoColors);
	void master(int processes);
	void worker(int processId);


};