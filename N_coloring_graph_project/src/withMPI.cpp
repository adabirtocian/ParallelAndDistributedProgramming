#include "../inc/withMPI.hpp"
#include <iostream>
#include <mpi.h>

WithMPI::WithMPI(int noNodes, std::vector<std::vector<int>> adjacentNodeIds, int maxNoColors)
        : adjacentNodeIds(adjacentNodeIds), usedColors(1), maxNoColors(maxNoColors), noSolution(false) {
    this->colors = std::vector<int>(noNodes, 1);
    this->visited = std::vector<int>(noNodes, 0);
    this->receivedMessage = 0;
}

void WithMPI::master(int processes) {
    int solutionStatus;
    MPI_Status status;

    if (processes == 1) {
        std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        this->runSequentialWrapper();
        std::chrono::milliseconds end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        std::cout << "\nTime: " << (end - start).count() << " ms \n\n";
    } else {
        std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        int nodeId = 0, usedColors = 1, newProcesses = processes - 2;
        std::vector<int> remainingProcesses;
        std::vector<int> solution = std::vector<int>(this->colors.size());

        for (int i = 2; i < processes; i++) {
            remainingProcesses.push_back(i);
        }

        this->sendData(nodeId, 1, remainingProcesses, usedColors, this->colors, this->visited);

        // receive solution from workers
//        for (int pid = 1; pid < processes; ++pid) {
//            MPI_Recv(
//                    &solutionStatus,
//                    1,
//                    MPI_INT,
//                    pid,
//                    123,
//                    MPI_COMM_WORLD,
//                    &status
//            );
//            //std::cout << pid << " -> " << solutionStatus << "\n";
//
//            // if solution found
//            if (solutionStatus == 1) {
//                MPI_Recv(
//                        solution.data(),
//                        int(this->colors.size()),
//                        MPI_INT,
//                        pid,
//                        123,
//                        MPI_COMM_WORLD,
//                        &status
//                );
//                std::cout << "Solution:\n";
//                for (int color: solution) {
//
//                    std::cout << color << " ";
//                }
//
//                std::cout << "\n";
//                MPI_Abort(MPI_COMM_WORLD, 0);
//            }
//        }


        int count;
        int flag=0;
        while(flag==0)
        {
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag,&status);
        }
        if(flag)
        {
            MPI_Recv(
                    &solutionStatus,
                    1,
                    MPI_INT,
                    MPI_ANY_SOURCE,
                    123,
                    MPI_COMM_WORLD,
                    &status
            );

            if (solutionStatus == 1) {
                MPI_Recv(
                        solution.data(),
                        int(this->colors.size()),
                        MPI_INT,
                        MPI_ANY_SOURCE,
                        123,
                        MPI_COMM_WORLD,
                        &status
                );
                std::cout << "Solution:\n";
                for (int color: solution) {

                    std::cout << color << " ";
                }
                std::chrono::milliseconds end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
                std::cout << "\nTime: " << (end - start).count() << " ms \n\n";
                std::cout << "\n";
                MPI_Abort(MPI_COMM_WORLD, 0);
            }
        }
    }


}

void WithMPI::worker(int processId) {
    int nodeId, solutionFound, noProcesses, usedColors;
    std::vector<int> visited = std::vector<int>(this->visited.size());
    std::vector<int> colors = std::vector<int>(this->colors.size());
    MPI_Status status;

    MPI_Recv(
            &nodeId,
            1,
            MPI_INT,
            MPI_ANY_SOURCE,
            0,
            MPI_COMM_WORLD,
            &status
    );

    MPI_Recv(
            &noProcesses,
            1,
            MPI_INT,
            MPI_ANY_SOURCE,
            1,
            MPI_COMM_WORLD,
            &status
    );

    MPI_Recv(
            &usedColors,
            1,
            MPI_INT,
            MPI_ANY_SOURCE,
            2,
            MPI_COMM_WORLD,
            &status
    );

    std::vector<int> processes = std::vector<int>(noProcesses);
    if (noProcesses > 0) {

        MPI_Recv(
                processes.data(),
                noProcesses,
                MPI_INT,
                MPI_ANY_SOURCE,
                3,
                MPI_COMM_WORLD,
                &status
        );
        //std::cout << processId <<" " <<noProcesses<<"\n";
        //for (int i : processes)
        //    std::cout << i << " ";
        //std::cout << "\n";
    }

    MPI_Recv(
            visited.data(),
            int(this->visited.size()),
            MPI_INT,
            MPI_ANY_SOURCE,
            4,
            MPI_COMM_WORLD,
            &status
    );

    MPI_Recv(
            colors.data(),
            int(this->colors.size()),
            MPI_INT,
            MPI_ANY_SOURCE,
            5,
            MPI_COMM_WORLD,
            &status
    );

    visited[nodeId] = 1;
    solutionFound = this->runWorker(processId, status.MPI_SOURCE, usedColors, colors, visited, nodeId, processes);


    //std::cout << "Worker " << processId << " " << solutionFound << "\n";

}


void WithMPI::runSequentialWrapper() {
    for (int i = 0; i < this->colors.size(); ++i) {
        if (!this->visited[i]) {
            this->visited[i] = 1;
            this->runSequential(i);
        }
    }
    if (this->noSolution) std::cout << "No solution\n";
    else {
        std::cout << "Solution:\n";
        for (auto color: this->colors) {
            std::cout << color << " ";
        }
    }
}

void WithMPI::runSequential(int nodeId) {
    std::vector<int> adjacentNodeIds = this->adjacentNodeIds[nodeId];
    for (int i = 0; i < adjacentNodeIds.size(); ++i) {
        if (this->colors[adjacentNodeIds[i]] == this->colors[nodeId]) {
            this->colors[adjacentNodeIds[i]] += 1;
        }

        this->usedColors = std::max(this->usedColors, std::max(this->colors[adjacentNodeIds[i]], this->colors[nodeId]));

        if (this->usedColors > this->maxNoColors) {
            this->noSolution = true;
            return;
        }
    }

    for (int i = 0; i < adjacentNodeIds.size(); ++i) {
        if (!this->visited[adjacentNodeIds[i]]) {
            this->visited[adjacentNodeIds[i]] = 1;
            this->runSequential(adjacentNodeIds[i]);
        }
    }
}

int
WithMPI::runWorker(int processId, int parentProcess, int usedColors, std::vector<int> colors, std::vector<int> visited,
                   int nodeId, std::vector<int> processes) {
//-------EXPERIMENTAL------------------------------------------------
//    if (this->receivedMessage == 0) {
//        MPI_Status status;
//        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &this->receivedMessage,&status);
//    }
//    if (this->receivedMessage == 1) {
//        std::vector<int> newVisitedNodes = this->receiveBroadcastData();
//        for(int i = 0; i< newVisitedNodes.size(); i++){
//            std::cout << newVisitedNodes[i]<< " ";
//            visited[newVisitedNodes[i]] = 1;
//        }
//        std::cout << std::endl;
//        this->receivedMessage = 0;
//    }
    int solutionFound = 0;
    visited[nodeId] = 1;
    //std::cout << processId << " nodeID: " << nodeId <<" from "<< parentProcess <<"\n";
    //for (int i : colors)
    //{
    //    std::cout << i << " ";
    //}
    //std::cout << "\n";
    //for (int i : visited)
    //{
    //    std::cout << i << " ";
    //}
    //std::cout << "\n";

    std::vector<int> adjacentNodeIds = this->adjacentNodeIds[nodeId];
    for (int i = 0; i < adjacentNodeIds.size(); ++i) {
        if (colors[adjacentNodeIds[i]] == colors[nodeId]) {
            colors[adjacentNodeIds[i]] += 1;
        }

        usedColors = std::max(usedColors, std::max(colors[adjacentNodeIds[i]], colors[nodeId]));

        //std::cout << "Colors: " << usedColors << "\n";

        if (usedColors > this->maxNoColors) {
            std::cout << "NO SOLUTION!!";
            MPI_Send(
                    &solutionFound,
                    1,
                    MPI_INT,
                    0,
                    123,
                    MPI_COMM_WORLD
            );
            return 0;
        }

    }


    std::vector<int> unvisitedNodes = std::vector<int>();
    for (int i = 0; i < adjacentNodeIds.size(); i++) {
        int adjNode = adjacentNodeIds[i];
        if (visited[adjNode] == 0) {
            unvisitedNodes.push_back(adjNode);
        }
    }



    if (unvisitedNodes.size() == 0) {
        //if(usedColors == this->maxNoColors) std::cout << "SOLUTION " << processId << " "<< parentProcess << "\n";
        //for (int i : colors)
        //{
        //    std::cout << i << " ";
        //}
        //std::cout << "\n";

        solutionFound = 1;
        MPI_Send(
                &solutionFound,
                1,
                MPI_INT,
                0,
                123,
                MPI_COMM_WORLD
        );
        MPI_Send(
                colors.data(),
                int(colors.size()),
                MPI_INT,
                0,
                123,
                MPI_COMM_WORLD
        );

        return 1;
    }

        //for (int i : colors)
        //{
        //    std::cout << i << " ";
        //}
        //std::cout << "\n";
        //for (int i : visited)
        //{
        //    std::cout << i << " ";
        //}
        //std::cout << "\n";
        //for (int i : unvisitedNodes)
        //{
        //    std::cout << i << " ";
        //}
        //std::cout << "\n------------------ PROCS = 0\n";

//-----EXPERIMENTAL---------------
//    this->broadcastData(unvisitedNodes, processes);

    if (processes.size() >= unvisitedNodes.size()) {
        //std::cout << "------------------ PROCS = " << processes.size() << " nodes=" << unvisitedNodes.size() <<"\n";
        std::vector<int> destinations;
        for (int i = 0; i < unvisitedNodes.size(); ++i) {
            destinations.push_back(processes[0]);
            processes.erase(processes.begin(), processes.begin() + 1);

        }
//        std:: cout<< nodeId<< " "<<processes.size() << std::endl;
        for (int i = 0; i< destinations.size(); i++){
            sendData(unvisitedNodes[i], destinations[i], processes, usedColors, colors, visited);
        }
    } else {
        //for (int i : colors)
        //{
        //    std::cout << i << " ";
        //}
        //std::cout << "\n";
        //for (int i : unvisitedNodes)
        //{
        //    std::cout << i << " ";
        //}
        //std::cout << " PROCS = " << processes.size() << " nodes=" << unvisitedNodes.size() << "\n";
        std::vector<int> destinations;
        for (int i = 0; i < processes.size(); ++i) {
            destinations.push_back(processes[0]);
            processes.erase(processes.begin(), processes.begin() + 1);

        }
        for (int i = 0; i< destinations.size(); i++){
            sendData(unvisitedNodes[i], destinations[i], processes, usedColors, colors, visited);
        }
//        std:: cout<< nodeId<< " "<<processes.size() << std::endl;

        for (int i = processes.size(); i < unvisitedNodes.size(); ++i) {
            //visited[unvisitedNodes[i]] = 1;
            //std::cout <<"here "<< unvisitedNodes[i] << "\n";
            solutionFound = solutionFound ||
                            this->runWorker(processId, processId, usedColors, colors, visited, unvisitedNodes[i],
                                            processes);
            if (solutionFound == 1) return 1;
        }
    };
    return 0;
}

void WithMPI::sendData(int nodeId, int destination, std::vector<int> processes, int usedColors,
                       std::vector<int> colors, std::vector<int> visited) {
    MPI_Send(
            &nodeId,
            1,
            MPI_INT,
            destination,
            0,
            MPI_COMM_WORLD
    );
    int noProcesses = int(processes.size());
    MPI_Send(
            &noProcesses,
            1,
            MPI_INT,
            destination,
            1,
            MPI_COMM_WORLD
    );

    MPI_Send(
            &usedColors,
            1,
            MPI_INT,
            destination,
            2,
            MPI_COMM_WORLD
    );
    if (processes.size() > 0) {
        MPI_Send(
                processes.data(),
                int(processes.size()),
                MPI_INT,
                destination,
                3,
                MPI_COMM_WORLD
        );
    }

    MPI_Send(
            visited.data(),
            int(visited.size()),
            MPI_INT,
            destination,
            4,
            MPI_COMM_WORLD
    );
    MPI_Send(
            colors.data(),
            int(colors.size()),
            MPI_INT,
            destination,
            5,
            MPI_COMM_WORLD
    );
}

void WithMPI::broadcastData(std::vector<int> newVisitedNodes, std::vector<int> processes) {
    int noProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &noProcesses);
    int noNewVisitedNodes = newVisitedNodes.size();
    for (int i = 1; i < noProcesses; i++) {

        if (std::count(processes.begin(), processes.end(), i) == 0) {
            MPI_Send(
                    &noNewVisitedNodes,
                    1,
                    MPI_INT,
                    i,
                    10,
                    MPI_COMM_WORLD
            );
            MPI_Send(
                    newVisitedNodes.data(),
                    noNewVisitedNodes,
                    MPI_INT,
                    i,
                    11,
                    MPI_COMM_WORLD
            );
        }

    }
}

std::vector<int> WithMPI::receiveBroadcastData() {
    int noNewVisitedNodes = 0;
    MPI_Status status;
    MPI_Recv(
            &noNewVisitedNodes,
            1,
            MPI_INT,
            MPI_ANY_SOURCE,
            10,
            MPI_COMM_WORLD,
            &status
    );
    std::vector<int> newVisitedNodes = std::vector<int>(noNewVisitedNodes);
    MPI_Recv(
            newVisitedNodes.data(),
            noNewVisitedNodes,
            MPI_INT,
            MPI_ANY_SOURCE,
            11,
            MPI_COMM_WORLD,
            &status
    );

    return newVisitedNodes;
}
