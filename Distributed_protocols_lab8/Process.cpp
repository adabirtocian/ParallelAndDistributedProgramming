#include "Process.hpp"
#include <iostream>
#include <string>

Process::Process(int id): id{id}, currentOperationIdx(0)
{
	status = MPI_Status();
}

void Process::addSubscriberToVariable(char variable, int subscriberId)
{
	this->subscribersOfVariables[variable].push_back(subscriberId);
}

void Process::addOperation(char var, int value)
{
	this->operations.push_back(std::pair<char, int>(var,value));
}

std::pair<char, int> Process::execNextOperation()
{
	std::pair<char, int> op;
	if (this->currentOperationIdx < this->operations.size())
	{
		op = this->operations[this->currentOperationIdx];
		this->currentOperationIdx++;
		return op;
	}
	return std::pair<char, int>('-', -1);
	
}

std::vector<int> Process::getSubscribersForVar(char var)
{
	return this->subscribersOfVariables[var];
}

void Process::increaseTimestamp()
{
	this->timestamp++;
}

void Process::setTimestamp(int ts)
{
	this->timestamp = ts;
}

void Process::addReceivedPrepareMsg(char var, int ts, int process)
{
	this->receivedPrepareMsgs.push_back(std::tuple<char, int, int, bool>(var, ts, process, true));
}

void Process::addReceivedPrepareResponse(char var, int ts, int process)
{
	this->receivedPrepareResponses.push_back(std::tuple<char, int, int>(var, ts, process));
}

bool Process::areAllPrepareResponses(char variable)
{
	return this->getSubscribersForVar(variable).size() == this->receivedPrepareResponses.size();
}

bool Process::areAllOperationsForPrepares()
{
	return this->operationsFramework.size() == this->receivedPrepareMsgs.size();
}

void Process::closePrepare(char variable)
{
	for (int i=0; i < this->receivedPrepareMsgs.size(); ++i)
	{
		if (std::get<0>(this->receivedPrepareMsgs[i]) == variable)
		{
			std::get<3>(this->receivedPrepareMsgs[i]) = false;
		}
	}
}

void Process::addOperationFramework(char var, int value, int ts)
{
	this->operationsFramework.push_back(std::tuple<char, int, int>(var, value, ts));
}

void Process::sendNotify()
{
	// take each operation
	for (int i = 0; i < this->operationsFramework.size(); ++i)
	{
		std::tuple<char, int, int> opFramework = this->operationsFramework[i];
		// search for largest timestamp for this operation
		for (std::tuple<char, int, int, bool> receivedPrepare : this->receivedPrepareMsgs)
		{
			// same variable
			if (std::get<0>(receivedPrepare) == std::get<0>(opFramework))
			{
				// compare ts
				if (std::get<1>(receivedPrepare) > std::get<2>(opFramework))
				{
					// set timestamp
					std::get<2>(opFramework) = std::get<1>(receivedPrepare);
				}
			}
		}
	}

	// sort operations based on timestamp
	std::tuple<char, int, int> auxOp;
	for (int i = 0; i < this->operationsFramework.size(); ++i)
		for (int j = i + 1; j < this->operationsFramework.size(); ++j)
			if (std::get<2>(this->operationsFramework[i]) > std::get<2>(this->operationsFramework[j]))
			{
				auxOp = this->operationsFramework[i];
				this->operationsFramework[i] = this->operationsFramework[j];
				this->operationsFramework[j] = auxOp;
			}

	// send the notify messages
	for (int i = 0; i < this->operationsFramework.size(); ++i)
	{
		auto var = std::get<0>(this->operationsFramework[i]);
		auto value = std::get<1>(this->operationsFramework[i]);

		//  set value
		this->setValueForVar(var, value);
	}
}

void Process::setValueForVar(char var, int value)
{
	for (int i = 0; i < this->variables.size(); ++i)
	{
		if (this->variables[i] == var)
		{
			this->values[i] = value;
		}
	}
}

void Process::work()
{
	// worker stuff
	int sizeVars, noSubscribers, noOperations, value=0;
	char variable;

	// receive variables
	MPI_Recv(&sizeVars, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &this->status);
	this->variables.resize(sizeVars);
	MPI_Recv(this->variables.data(), sizeVars, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &this->status);
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

	// receive operations
	MPI_Recv(&noOperations, 1, MPI_INT, 0, 123, MPI_COMM_WORLD, &status);
	for (int i = 0; i < noOperations; ++i)
	{
		MPI_Recv(&variable, 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &this->status);
		MPI_Recv(&value, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &this->status);
		this->addOperation(variable, value);
		std::cout << this->id << " " << variable << "=" << value << "\n";
	}

	
	// select a set operation and send it
	int actionCode = 0; // set operation
	bool atTheBeginning = true;
	int sendCode = 1; // code for prepare
	char receivedVar = '-';
	int parent = 0, ts = 0;
	std::pair<char, int> operation;
	
	while (actionCode != -1)
	{
		if (!atTheBeginning) {
			MPI_Recv(&actionCode, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &this->status);
			parent = this->status.MPI_SOURCE;
			std::cout << "Code=" << actionCode << " from " << parent << "\n";
		}
		atTheBeginning = false;
		switch (actionCode)
		{
		case(0):
			// set operation
			operation = this->execNextOperation();
			variable = operation.first;
			value = operation.second;

			//std::cout << "operation " << variable << " = " << value << "\n";

			// check for valid operation
			if (variable == '-' && value == -1)
			{
				// invalid operation
				actionCode = -1;
				break;
			}

			// store the operation in framework
			this->addOperationFramework(variable, value, this->timestamp);

			// iterate over the subscribers of the variable and send prepare message
			for (auto process : this->getSubscribersForVar(variable))
			{
				if (process != this->id)
				{
					this->increaseTimestamp();
					sendCode = 1; // prepare message
					std::cout << "send to " << process << " " << variable << " " << value << " " << this->timestamp << "\n";
					MPI_Ssend(&sendCode, 1, MPI_INT, process, 1, MPI_COMM_WORLD);
					//MPI_Ssend(&variable, 1, MPI_CHAR, process, 1, MPI_COMM_WORLD);
					//MPI_Ssend(&value, 1, MPI_INT, process, 1, MPI_COMM_WORLD);
					//MPI_Ssend(&this->timestamp, 1, MPI_INT, process, 1, MPI_COMM_WORLD);
				}
			}
			actionCode = 0;
			break;
		case(-1):
			// stop
			break;
		case(1):
			// receive prepare messages
			//MPI_Recv(&receivedVar, 1, MPI_CHAR, parent, 1, MPI_COMM_WORLD, &this->status);
			//MPI_Recv(&value, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
			//MPI_Recv(&ts, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
			std::cout << "received "<<this->id<<" " << variable << " " << value << " " << this->timestamp << "\n";
			// update timestamp
			//this->setTimestamp(std::max(ts, this->timestamp) + 1);
			
			// save received prepare message
			//this->addReceivedPrepareMsg(receivedVar, this->timestamp, parent);

			// send response back
			sendCode = 2; // response for a prepare message
			//MPI_Ssend(&sendCode, 1, MPI_INT, parent, 1, MPI_COMM_WORLD);
			//MPI_Ssend(&receivedVar, 1, MPI_CHAR, parent, 1, MPI_COMM_WORLD);
			//MPI_Ssend(&value, 1, MPI_INT, parent, 1, MPI_COMM_WORLD);
			//MPI_Ssend(&this->timestamp, 1, MPI_INT, parent, 1, MPI_COMM_WORLD);
			actionCode = -1; // FOR TESTING ONLY
			break;
			
		//case(2):
		//	// receive a response for the prepare message sent from ths current process
		//	MPI_Recv(&receivedVar, 1, MPI_CHAR, parent, 1, MPI_COMM_WORLD, &this->status);
		//	MPI_Recv(&value, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
		//	MPI_Recv(&ts, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
		//
		//	// update timestamp
		//	this->setTimestamp(std::max(ts, this->timestamp) + 1);
		//
		//	// save the response to the prepare message
		//	this->addReceivedPrepareResponse(receivedVar, ts, parent);
		//
		//	// check if all responses were received
		//	if (this->areAllPrepareResponses(receivedVar))
		//	{
		//
		//	}
		//
		//	break;
		//
		//case(3):
		//	// receive set operation from another process
		//	MPI_Recv(&receivedVar, 1, MPI_CHAR, parent, 1, MPI_COMM_WORLD, &this->status);
		//	MPI_Recv(&value, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
		//	MPI_Recv(&ts, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
		//
		//	// increase timestamp
		//	this->setTimestamp(std::max(ts, this->timestamp) + 1);
		//
		//	// close prepare phase
		//	this->closePrepare(receivedVar);
		//
		//	// save operation in framework
		//	this->addOperationFramework(receivedVar, value, ts);
		//
		//	// check if all operations for the prepare messages were received
		//	if (this->areAllOperationsForPrepares())
		//	{
		//		// send notify to all processes with the same order for the operations
		//		this->sendNotify();
		//	}
		//
		//	// stop 
		//	actionCode = -1;
		//	break;
		default:
			std::cout << "Invalid code " << actionCode << " in process " << this->id << "\n";
			actionCode = -1;
			break;
		}

		MPI_Recv(&actionCode, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &this->status);
		parent = this->status.MPI_SOURCE;
		std::cout << "Code=" << actionCode << " from " << parent << "\n";
		
	}
	
	std::cout << "STOP " << this->id << "\n";
}


