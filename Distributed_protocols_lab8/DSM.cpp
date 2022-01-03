#include "DSM.hpp"
#include <mpi.h>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

DSM::DSM(int processes) : processes{ processes }
{
}

void DSM::sendSubscribers(int destProcess, char variable, std::vector<int> subscribers)
{
    //std::cout << "Send subs to " << destProcess << "\n";
    int noSubs = subscribers.size();
    MPI_Ssend(&noSubs, 1, MPI_INT, destProcess, 1, MPI_COMM_WORLD);
    MPI_Ssend(&variable, 1, MPI_CHAR, destProcess, 1, MPI_COMM_WORLD);
    MPI_Ssend(subscribers.data(), noSubs, MPI_INT, destProcess, 2, MPI_COMM_WORLD);
}

void DSM::sentSetOperations(int destProcess, char variable, int value)
{
    MPI_Ssend(&variable, 1, MPI_CHAR, destProcess, 1, MPI_COMM_WORLD);
    MPI_Ssend(&value, 1, MPI_INT, destProcess, 1, MPI_COMM_WORLD);
}

void DSM::start()
{
    std::cout << "Master started with " << this->processes << " processes\n";
    std::vector<char> variablesP1{ 'a', 'b', 'c'};
    std::vector<char> variablesP2{ 'a', 'b', 'c' };
    std::vector<char> variablesP3{ 'a', 'c', 'd' };
    std::vector<char> variablesP4{ 'a', 'c', 'd' };
    int sizeVarsP1 = variablesP1.size();
    int sizeVarsP2 = variablesP2.size();
    int sizeVarsP3 = variablesP3.size();
    int sizeVarsP4 = variablesP4.size();
    int process1 = 1, process2 = 2, process3 = 3, process4 = 4;
    int operationsP1 = 3, operationsP3 = 3, operationsP2 = 3, operationsP4 = 3;

    std::unordered_map<char, std::vector<int>> subscribersOfVariables;
    subscribersOfVariables.insert(
        std::make_pair<char, std::vector<int>>('a', std::vector<int>{process1, process2, process3, process4})
    );
    subscribersOfVariables.insert(
        std::make_pair<char, std::vector<int>>('b', std::vector<int>{process1, process2})
    );
    subscribersOfVariables.insert(
        std::make_pair<char, std::vector<int>>('c', std::vector<int>{process1, process2, process3, process4})
    );
    subscribersOfVariables.insert(
        std::make_pair<char, std::vector<int>>('d', std::vector<int>{process3, process4})
    );

    // send to process 1 its variables
    MPI_Ssend(&sizeVarsP1, 1, MPI_INT, process1, 1, MPI_COMM_WORLD);
    MPI_Ssend(variablesP1.data(), sizeVarsP1, MPI_CHAR, process1, 2, MPI_COMM_WORLD);
    
    // send to process 2 its variables
    MPI_Ssend(&sizeVarsP2, 1, MPI_INT, process2, 1, MPI_COMM_WORLD);
    MPI_Ssend(variablesP2.data(), sizeVarsP2, MPI_CHAR, process2, 2, MPI_COMM_WORLD);
    
    // send to process 3 its variables
    MPI_Ssend(&sizeVarsP3, 1, MPI_INT, process3, 1, MPI_COMM_WORLD);
    MPI_Ssend(variablesP3.data(), sizeVarsP3, MPI_CHAR, process3, 2, MPI_COMM_WORLD);
    
    // send to process 4 its variables
    MPI_Ssend(&sizeVarsP4, 1, MPI_INT, process4, 1, MPI_COMM_WORLD);
    MPI_Ssend(variablesP4.data(), sizeVarsP4, MPI_CHAR, process4, 2, MPI_COMM_WORLD);
    
    // send to each process all subscribers
    for (auto entry : subscribersOfVariables)
    {
        if (std::find(entry.second.begin(), entry.second.end(), process1) != entry.second.end())
        {
            this->sendSubscribers(process1, entry.first, entry.second);
        }
    
        if (std::find(entry.second.begin(), entry.second.end(), process2) != entry.second.end())
        {
            this->sendSubscribers(process2, entry.first, entry.second);
        }
            
        if (std::find(entry.second.begin(), entry.second.end(), process3) != entry.second.end())
        {
            this->sendSubscribers(process3, entry.first, entry.second);
        }
            
        if (std::find(entry.second.begin(), entry.second.end(), process4) != entry.second.end())
        {
            this->sendSubscribers(process4, entry.first, entry.second);
        }
    }

    // send operations for variables
    // p1
    MPI_Ssend(&operationsP1, 1, MPI_INT, process1, 123, MPI_COMM_WORLD);
    this->sentSetOperations(process1, 'a', 3);
    this->sentSetOperations(process1, 'b', 4);
    this->sentSetOperations(process1, 'c', 5);
    
    // p2
    MPI_Ssend(&operationsP2, 1, MPI_INT, process2, 123, MPI_COMM_WORLD);
    this->sentSetOperations(process2, 'a', 1);
    this->sentSetOperations(process2, 'b', 2);
    this->sentSetOperations(process2, 'c', 3);
    
    // p3
    MPI_Ssend(&operationsP3, 1, MPI_INT, process3, 123, MPI_COMM_WORLD);
    this->sentSetOperations(process3, 'a', 8);
    this->sentSetOperations(process3, 'c', 9);
    this->sentSetOperations(process3, 'd', 10);
    
    // p4
    MPI_Ssend(&operationsP4, 1, MPI_INT, process4, 123, MPI_COMM_WORLD);
    this->sentSetOperations(process4, 'a', 2);
    this->sentSetOperations(process4, 'c', 3);
    this->sentSetOperations(process4, 'd', 4);

    //std::cout << "Done 0!\n";
}

