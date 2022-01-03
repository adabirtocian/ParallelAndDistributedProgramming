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
	return this->getSubscribersForVar(variable).size() - 1 == this->receivedPrepareResponses.size();
}

bool Process::areAllOperationsForPrepares()
{
	return this->operationsFramework.size() - 1 == this->receivedPrepareMsgs.size();
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
	for (int i = 0; i < this->operationsFramework.size(); ++i)
	{
		if (std::get<0>(this->operationsFramework[i]) == var)
		{
			// update the timestamp
			this->operationsFramework[i] = std::tuple<char, int, int>(var, value, ts);
			return;
		}
	}
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
		auto ts = std::get<2>(this->operationsFramework[i]);

		//  set value
		this->setValueForVar(var, value);
		std::cout << "notify " << var << "=" << value << "; ts=" << ts << "\n";
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

void Process::sendSetOperationsFromFramework()
{
	std::vector<std::tuple<char, int, int, int>> setOperations;
	for (int i = 0; i < this->receivedPrepareResponses.size(); ++i)
	{
		auto var = std::get<0>(this->receivedPrepareResponses[i]);
		auto ts = std::get<1>(this->receivedPrepareResponses[i]);
		auto process = std::get<2>(this->receivedPrepareResponses[i]);

		setOperations.push_back(std::tuple<char, int, int, int>(var, this->operations[0].second, ts, process));
	}

	// sort by ts
	std::tuple<char, int, int, int> aux;
	for (int i = 0; i < setOperations.size(); ++i)
	{
		for (int j = i + 1; j < setOperations.size(); ++j)
		{
			if (std::get<2>(setOperations[i]) > std::get<2>(setOperations[j]))
			{
				aux = setOperations[i];
				setOperations[i] = setOperations[j];
				setOperations[j] = aux;
			}
		}
	}
	for (int i = 0; i < setOperations.size(); ++i)
	{
		std::cout << std::get<0>(setOperations[i]) << "=" << std::get<1>(setOperations[i]) << " ts=" << std::get<2>(setOperations[i]) << " process=" << std::get<3>(setOperations[i])<<"\n";
	}
	// send operations 
	//int sendCode = 3, value, ts, destination;
	//char variable;
	//for (auto op : setOperations)
	//{
	//	variable = std::get<0>(op);
	//	value = std::get<1>(op);
	//	ts = std::get<2>(op);
	//	destination = std::get<3>(op);
	//
	//	if (this->isTsSmaller(ts) || this->id == 1)
	//	{
	//		this->increaseTimestamp();
	//		if (this->id == destination)
	//		{
	//			this->addOperationFramework(variable, value, ts);
	//		}
	//		else
	//		{
	//			MPI_Send(&sendCode, 1, MPI_INT, destination, 123, MPI_COMM_WORLD);
	//			MPI_Send(&variable, 1, MPI_CHAR, destination, 123, MPI_COMM_WORLD);
	//			MPI_Send(&value, 1, MPI_INT, destination, 123, MPI_COMM_WORLD);
	//			MPI_Send(&ts, 1, MPI_INT, destination, 123, MPI_COMM_WORLD);
	//		}
	//	}
	//	else
	//	{
	//		std::cout << this->id << " failed\n";
	//	}
	//}


}

bool Process::isTsSmaller(int ts)
{
	for (auto prepare : this->receivedPrepareMsgs)
	{
		if (std::get<3>(prepare))
		{
			if (ts > std::get<1>(prepare))
			{
				return false;
			}
		}
	}
	return true;
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
		//std::cout << this->id << " " << variable << "=" << value << "\n";
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
			//std::cout << "Code=" << actionCode << " from " << parent << "\n";
		}
		atTheBeginning = false;
		switch (actionCode)
		{
		case(0):
			// set operation
			operation = this->execNextOperation();
			variable = operation.first;
			value = operation.second;

			std::cout << "operation " << variable << " = " << value << "\n";

			// check for valid operation
			if (variable == '-' && value == -1)
			{
				// invalid operation
				actionCode = -1;
				break;
			}

			this->addOperationFramework(variable, value, this->timestamp);

			// iterate over the subscribers of the variable and send prepare message
			for (auto process : this->getSubscribersForVar(variable))
			{
				if (process != this->id)
				{
					this->increaseTimestamp();
					sendCode = 1; // prepare message
					std::cout <<this->id<< " send to " << process << " : " << variable << "=" << value << " " << this->timestamp << "\n";
					MPI_Send(&sendCode, 1, MPI_INT, process, 1, MPI_COMM_WORLD);
					MPI_Send(&variable, 1, MPI_CHAR, process, 1, MPI_COMM_WORLD);
					MPI_Send(&value, 1, MPI_INT, process, 1, MPI_COMM_WORLD);
					MPI_Send(&this->timestamp, 1, MPI_INT, process, 1, MPI_COMM_WORLD);
				}
			}
			actionCode = 0;
			break;
		case(-1):
			// stop
			break;
		case(1):
			// receive prepare messages
			MPI_Recv(&receivedVar, 1, MPI_CHAR, parent, 1, MPI_COMM_WORLD, &this->status);
			MPI_Recv(&value, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
			MPI_Recv(&ts, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
			std::cout <<this->id<< " received from "<<parent<<" : "<< variable << "=" << value << " " << this->timestamp << "\n";
			
			// update timestamp
			this->setTimestamp(std::max(ts, this->timestamp) + 1);
			
			// save received prepare message
			this->addReceivedPrepareMsg(receivedVar, this->timestamp, parent);

			// send response back
			sendCode = 2; // response for a prepare message
			MPI_Send(&sendCode, 1, MPI_INT, parent, 1, MPI_COMM_WORLD);
			MPI_Send(&receivedVar, 1, MPI_CHAR, parent, 1, MPI_COMM_WORLD);
			MPI_Send(&value, 1, MPI_INT, parent, 1, MPI_COMM_WORLD);
			MPI_Send(&this->timestamp, 1, MPI_INT, parent, 1, MPI_COMM_WORLD);
			break;
		
		case(2):
			// receive a response for the prepare message sent from ths current process
			MPI_Recv(&receivedVar, 1, MPI_CHAR, parent, 1, MPI_COMM_WORLD, &this->status);
			MPI_Recv(&value, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
			MPI_Recv(&ts, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
		
			// update timestamp
			this->setTimestamp(std::max(ts, this->timestamp) + 1);
		
			// save the response to the prepare message
			this->addReceivedPrepareResponse(receivedVar, ts, parent);
		
			// check if all responses were received
			if (this->areAllPrepareResponses(receivedVar))
			{
				// send the set operations
				this->sendSetOperationsFromFramework();

				for (auto process : this->getSubscribersForVar(receivedVar))
				{
					if (process != this->id)
					{
						sendCode = 0; // prepare message
						//std::cout << this->id << " send to " << process << " : " << variable << "=" << value << " " << this->timestamp << "\n";
						MPI_Send(&sendCode, 1, MPI_INT, process, 1, MPI_COMM_WORLD);
					}
				}
				//actionCode = -1;
			}
			
			break;
		
		case(3):
			// receive set operation from another process
			MPI_Recv(&receivedVar, 1, MPI_CHAR, parent, 1, MPI_COMM_WORLD, &this->status);
			MPI_Recv(&value, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
			MPI_Recv(&ts, 1, MPI_INT, parent, 1, MPI_COMM_WORLD, &this->status);
		
			// increase timestamp
			this->setTimestamp(std::max(ts, this->timestamp) + 1);
		
			// close prepare phase
			this->closePrepare(receivedVar);
		
			// save operation in framework
			this->addOperationFramework(receivedVar, value, ts);
		
			// check if all operations for the prepare messages were received
			if (this->areAllOperationsForPrepares())
			{
				// send notify to all processes with the same order for the operations
				this->sendNotify();
			}
		
			// stop 
			actionCode = -1;
			break;
		default:
			std::cout << "Invalid code " << actionCode << " in process " << this->id << "\n";
			actionCode = -1;
			break;
		}		
	}
	
	std::cout << "STOP " << this->id << "\n";
}


