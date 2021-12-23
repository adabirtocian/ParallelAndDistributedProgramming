#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <mpi.h>

class Process
{
private:
	int id;
	int timestamp = 1;
	std::vector<char> variables;
	std::unordered_map<char, std::vector<int>> subscribersOfVariables;
	std::unordered_map<char, std::vector<int>> operations;
	
	MPI_Status status;

public:
	Process(int id);
	void addSubscriberToVariable(char variable, int subscriberId);
	void work();
	void addOperation(char var, int value);
};

