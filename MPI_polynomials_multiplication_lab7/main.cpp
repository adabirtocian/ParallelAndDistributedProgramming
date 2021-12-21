#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>

std::vector<int> generatePolynomialCoefficients(int size)
{
    std::vector<int> polynomial(size);
    srand(time(NULL));

    for (int i = 0; i < size - 1; ++i)
    {
        polynomial[i] = rand() % 4 + 1;
    }
    polynomial[size - 1] = rand() % 4 + 1;

    return polynomial;
}

void regularWorker(int processId)
{
    MPI_Status status;
    int start, end, size1, size2;
    std::vector<int> p1;
    std::vector<int> p2;


    MPI_Recv(&start, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&end, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    
    MPI_Recv(&size1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    p1.resize(size1);
    MPI_Recv(p1.data(), size1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
    
    MPI_Recv(&size2, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    p2.resize(size2);
    MPI_Recv(p2.data(), size2, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

    std::cout << "Worker "<<processId << " " << start;// << " " << end << " " << size1 << " " << size2 << "\n";
}

void regularMaster(int processes, std::vector<int> polynomial1, std::vector<int> polynomial2)
{
    std::cout << "Master starts\n";
    int workerLength = (polynomial1.size() + polynomial2.size() - 1) / (processes - 1);
    int size1 = polynomial1.size();
    int size2 = polynomial2.size();

    // split work between workers
    for (int i = 1; i < processes; ++i)
    {
        int start = i * workerLength;
        int end = start + workerLength - 1;

        std::cout << "Master sends: " << start;// << " " << end << " " << size1 << " " << size2 << "\n";
        // send the indices
        MPI_Send(&start, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Ssend(&end, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

        // send the data 
        MPI_Ssend(&size1, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Ssend(polynomial1.data(), size1, MPI_INT, i, 2, MPI_COMM_WORLD);
        MPI_Ssend(&size2, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Ssend(polynomial2.data(), size2, MPI_INT, i, 2, MPI_COMM_WORLD);
    }

    // compute first coefficients of the result
}

int main(int argc, char** argv) 
{
    // Initialize the MPI environment
    MPI_Init(0, 0);

    int processes, currentProcessRank, size1, size2;
    // Get the number of available processes
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &currentProcessRank);

    if (argc != 4)
    {
        fprintf(stderr, "Help: <.exe> <size1> <size2> <method(regular/karatsuba)>\n");
        return 1;
    }

    if(1 != sscanf_s(argv[1], "%u", &size1) || 1 != sscanf_s(argv[2], "%u", &size2)) 
    {
        fprintf(stderr, "Polynomials sizes must be given as args\n");
        return 1;
    }
    
    std::string method = argv[3];

    if (method != "regular" && method != "karatsuba")
    {
        fprintf(stderr, "Invalid method! Choose from: regular / karatsuba\n");
        return 1;
    }

    if (currentProcessRank == 0)
    {
        // master process
        // generate 2 polynomials
        std::vector<int> polynomial1 = generatePolynomialCoefficients(size1);
        std::vector<int> polynomial2 = generatePolynomialCoefficients(size2);
        std::cout << "Polynomials generated with sizes "<<size1 << " and " << size2 << "\n";
        for (int i : polynomial1)
        {
            std::cout << i << " ";
        }
        std::cout << "\n";
        for (int i : polynomial2)
        {
            std::cout << i << " ";
        }
        // set the method to be used in multiplication
        if (method == "regular")
        {
            std::cout << "start\n";
            regularMaster(processes, polynomial1, polynomial2);
        }
        else if (method == "karatsuba")
        {
            
        }

    }
    else
    {
        // worker process
        regularWorker(currentProcessRank);
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}