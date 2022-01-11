#include <iostream>
#include <mpi.h>
#include <vector>
#include <string>
#include "DSM.hpp"
#include "Process.hpp"

void worker(int currentProcessRank)
{
    
}


// mpiexec -n 5
int main(int argc, char** argv)
{
	// Initialize the MPI environment
	MPI_Init(0, 0);

    int processes, currentProcessRank;
    Process* process;

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
        process = new Process(currentProcessRank);
        process->work();
    }

    // Finalize the MPI environment.
    MPI_Finalize();
    return 0;
}