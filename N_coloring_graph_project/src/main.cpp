#include <mpi.h>
#include <iostream>
#include <vector>
#include "../inc/node.hpp"
#include "../inc/threads.hpp"
#include "../inc/withMPI.hpp"
#include <chrono>

int main()
{
    auto node1 = new Node(1);
    auto node2 = new Node(2);
    auto node3 = new Node(3);
    auto node4 = new Node(4);
    auto node5 = new Node(5);
    auto node6 = new Node(6);
    auto node7 = new Node(7);
    auto node8 = new Node(8);
    auto node9 = new Node(9);
    auto node10 = new Node(10);

    node1->addEdge(node3);
    node1->addEdge(node4);
    node1->addEdge(node6);
    node2->addEdge(node4);
    node2->addEdge(node5);
    node2->addEdge(node7);
    node3->addEdge(node1);
    node3->addEdge(node5);
    node3->addEdge(node8);
    node4->addEdge(node2);
    node4->addEdge(node1);
    node4->addEdge(node9);
    node5->addEdge(node2);
    node5->addEdge(node3);
    node5->addEdge(node10);
    node6->addEdge(node7);
    node6->addEdge(node10);
    node6->addEdge(node1);
    node7->addEdge(node6);
    node7->addEdge(node8);
    node7->addEdge(node2);
    node8->addEdge(node7);
    node8->addEdge(node9);
    node8->addEdge(node3);
    node9->addEdge(node8);
    node9->addEdge(node10);
    node9->addEdge(node4);
    node10->addEdge(node6);
    node10->addEdge(node9);
    node10->addEdge(node5);

    std::vector<Node*> nodes;
    nodes.push_back(node1);
    nodes.push_back(node2);
    nodes.push_back(node3);
    nodes.push_back(node4);
    nodes.push_back(node5);
    nodes.push_back(node6);
    nodes.push_back(node7);
    nodes.push_back(node8);
    nodes.push_back(node9);
    nodes.push_back(node10);
    

//    Threads threads = Threads(nodes, 3);
//    threads.runWrapper(10);

    std::vector<std::vector<int>> adjacentNodeIds;
    std::vector<int> v1{ 2, 3, 5 };
    std::vector<int> v2{ 3,4,6};
    std::vector<int> v3{ 0, 4, 7};
    std::vector<int> v4{ 0, 1, 8};
    std::vector<int> v5{ 1, 2, 9};
    std::vector<int> v6{ 0, 6, 9};
    std::vector<int> v7{ 1, 5, 7};
    std::vector<int> v8{ 2, 6, 8};
    std::vector<int> v9{ 3,7,9};
    std::vector<int> v10{ 4,5,8};


    adjacentNodeIds.push_back(v1);
    adjacentNodeIds.push_back(v2);
    adjacentNodeIds.push_back(v3);
    adjacentNodeIds.push_back(v4);
    adjacentNodeIds.push_back(v5);
    adjacentNodeIds.push_back(v6);
    adjacentNodeIds.push_back(v7);
    adjacentNodeIds.push_back(v8);
    adjacentNodeIds.push_back(v9);
    adjacentNodeIds.push_back(v10);

    WithMPI withMPI = WithMPI(10, adjacentNodeIds, 3);

    // Initialize the MPI environment
    MPI_Init(0, 0);

    int processes, currentProcessRank;
    // Get the number of available processes
    MPI_Comm_size(MPI_COMM_WORLD, &processes);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &currentProcessRank);

    if (currentProcessRank == 0)
    {
        withMPI.master(processes);
    }
    else
    {
        withMPI.worker(currentProcessRank);
    }

    // Finalize the MPI environment.
    MPI_Finalize();

	return 0;
}