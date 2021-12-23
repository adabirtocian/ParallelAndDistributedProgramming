#include <iostream>
#include <mpi.h>
#include <vector>
#include <string>
#include "DSM.hpp"
#include "Process.hpp"

int main(int argc, char** argv)
{
	// Initialize the MPI environment
	MPI_Init(0, 0);

    int processes, currentProcessRank;
    // Get the number of available processes
    MPI_Comm_size(MPI_COMM_WORLD, &processes);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &currentProcessRank);

    if (currentProcessRank == 0)
    {
        // master
        DSM dsm = DSM(processes);
        dsm.start();

    }
    else
    {
        // workers
        Process process = Process(currentProcessRank);
        process.work();
    }

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}