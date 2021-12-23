#include "Process.hpp"
#include <iostream>
#include <string>

Process::Process(int id): id{id}
{
	status = MPI_Status();
}

void Process::addSubscriberToVariable(char variable, int subscriberId)
{
	this->subscribersOfVariables[variable].push_back(subscriberId);
}

void Process::addOperation(char var, int value)
{
	this->operations[var].push_back(value);
}

void Process::work()
{
	// worker stuff
	int sizeVars, noSubscribers, noOperations, value=0;
	char variable, var='a';

	// receive variables
	MPI_Recv(&sizeVars, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &this->status);
	this->variables.resize(sizeVars);
	MPI_Recv(this->variables.data(), sizeVars, MPI_INT, 0, 2, MPI_COMM_WORLD, &this->status);
	//std::cout << this->id << " sizeVars=" << sizeVars << "\n";
	//for (auto a : this->variables)
	//	std::cout << a << " ";
	//std::cout << "\n";
	
	// receive subscribers for all variables in the process
	for (int i = 0; i < sizeVars; ++i)
	{
		MPI_Recv(&noSubscribers, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &this->status);
		MPI_Recv(&variable, 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &this->status);
		
		this->subscribersOfVariables[variable].resize(noSubscribers);
		MPI_Recv(this->subscribersOfVariables[variable].data(), noSubscribers, MPI_INT, 0, 2, MPI_COMM_WORLD, &this->status);
	
		//std::cout << this->id << " var=" << variable << " subs=" << noSubscribers << "\n";
		//for (auto s : this->subscribersOfVariables[variable])
		//	std::cout << s << " ";
		//std::cout << "\n";
	}

	MPI_Recv(&noOperations, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
	for (int i = 0; i < noOperations; ++i)
	{
		MPI_Recv(&variable, 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &this->status);
		MPI_Recv(&value, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &this->status);
		this->addOperation(variable, value);
		//std::cout << this->id << " " << variable << "=" << value << "\n";
	}
}


