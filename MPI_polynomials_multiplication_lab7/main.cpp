#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>

std::vector<int> generatePolynomialCoefficients(int size, int additional=0)
{
    std::vector<int> polynomial(size);
    srand(time(NULL));

    for (int i = 0; i < size; ++i)
    {
        polynomial[i] = rand() % (i+1) + 1 + additional;
    }

    return polynomial;
}


std::vector<int> multiplySection(int size1, int size2,int start, int end, std::vector<int> polynomial1, std::vector<int> polynomial2)
{
    std::vector<int> partialResult = std::vector<int>(size1 + size2 - 1, 0);

    for (int i = start; i <= end; ++i)
    {
        for (int j = 0; j < size2; ++j)
        {
            partialResult[i + j] += polynomial1[i] * polynomial2[j];
            //std::cout << i + j << "=" << partialResult[i + j] << "\n";
        }
    }

    return partialResult;
}

std::vector<int> aggregateResults(std::vector<int> master, std::vector<int> worker)
{
    std::vector<int> result = std::vector<int>(master.size(), 0);
    for (int i = 0; i < master.size(); ++i)
    {
        result[i] += master[i] + worker[i];
    }

    return result;
}

void regularWorker(int processId)
{
    MPI_Status status;
    int start, end, size1, size2;
    std::vector<int> polynomial1;
    std::vector<int> polynomial2;

    MPI_Recv(&start, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(&end, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    
    MPI_Recv(&size1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    polynomial1.resize(size1);
    MPI_Recv(polynomial1.data(), size1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
    
    MPI_Recv(&size2, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    polynomial2.resize(size2);
    MPI_Recv(polynomial2.data(), size2, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

    std::cout << "Worker "<<processId << ": " << start << " " << end << "\n";

    // do the computations
    std::vector<int> partialResult = multiplySection(size1, size2, start, end, polynomial1, polynomial2);

    // send the result back to master
    MPI_Ssend(partialResult.data(), size1+size2, MPI_INT, 0, 2, MPI_COMM_WORLD);
}

void regularMaster(int processes, std::vector<int> polynomial1, std::vector<int> polynomial2)
{
    int size1 = polynomial1.size();
    int size2 = polynomial2.size();
    int workerLength = size1 / processes;

    // split work between workers
    for (int i = 1; i < processes; ++i)
    {
        int start = i * workerLength;
        int end = start + workerLength - 1;
        if (i == processes - 1)
        {
            end = size1 - 1;
        }

        std::cout << "Master sends: " << start << " " << end << "\n";
        // send the indices
        MPI_Send(&start, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Ssend(&end, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

        // send the data 
        MPI_Ssend(&size1, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Ssend(polynomial1.data(), size1, MPI_INT, i, 2, MPI_COMM_WORLD);
        MPI_Ssend(&size2, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Ssend(polynomial2.data(), size2, MPI_INT, i, 2, MPI_COMM_WORLD);
    }

    // do master part of the computation
    std::vector<int> finalResult = multiplySection(size1, size2, 0, workerLength-1, polynomial1, polynomial2);
    MPI_Status status;
    std::vector<int> workerResult = std::vector<int>(size1 + size2, 0);

    for (int i = 1; i < processes; ++i)
    {
        MPI_Recv(workerResult.data(), size1 + size2, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
        finalResult = aggregateResults(finalResult, workerResult);
    }
    //for (int i = 0; i < finalResult.size(); ++i)
    //{
    //    std::cout << finalResult[i] << " ";
    //}
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
        std::vector<int> polynomial2 = generatePolynomialCoefficients(size2, 5);
        //std::cout << "Polynomials generated with sizes "<<size1 << " and " << size2 << "\n";
        //for (int i : polynomial1)
        //{
        //    std::cout << i << " ";
        //}
        //std::cout << "\n";
        //for (int i : polynomial2)
        //{
        //    std::cout << i << " ";
        //}
        // set the method to be used in multiplication
        if (method == "regular")
        {
            regularMaster(processes, polynomial1, polynomial2);
        }
        else if (method == "karatsuba")
        {
            
        }
    }
    else
    {
        // worker process
        if (method == "regular")
        {
            regularWorker(currentProcessRank);
        }
        else if (method == "karatsuba")
        {

        }
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}