#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <mpi.h>
#include <tuple>

class Process
{
private:
	int id;
	int timestamp = 1;
	std::vector<char> variables;
	std::vector<int> values;
	std::unordered_map<char, std::vector<int>> subscribersOfVariables;
	std::vector<std::pair<char, int>> operations;
	std::vector<std::tuple<char, int, int, bool>> receivedPrepareMsgs; // variable, ts, sender process, open/close
	std::vector<std::tuple<char, int, int>> receivedPrepareResponses; // variable, ts, sender process
	std::vector<std::tuple<char, int, int>> operationsFramework; // variable, value, ts
	int currentOperationIdx;
	MPI_Status status;

public:
	Process(int id);
	void addSubscriberToVariable(char variable, int subscriberId);
	void work();
	void addOperation(char var, int value);
	std::pair<char, int> execNextOperation();
	std::vector<int> getSubscribersForVar(char var);
	void increaseTimestamp();
	void setTimestamp(int ts);
	void addReceivedPrepareMsg(char var, int ts, int process);
	void addReceivedPrepareResponse(char var, int ts, int process);
	bool areAllPrepareResponses(char variable);
	bool areAllOperationsForPrepares();
	void closePrepare(char variable);
	void addOperationFramework(char var, int value, int ts);
	void sendNotify();
	void setValueForVar(char var, int value);
};

