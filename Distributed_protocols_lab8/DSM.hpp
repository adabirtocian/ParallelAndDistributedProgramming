#pragma once
#include <string>
#include <vector>

class DSM
{
private:
	int processes;

	void sendSubscribers(int destProcess, char variable, std::vector<int> subscribers);
	void sentSetOperations(int destProcess, char variable, int value);
public:
	DSM(int processes);
	void start();
};

