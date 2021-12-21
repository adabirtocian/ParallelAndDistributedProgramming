#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>

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

int main(int argc, char** argv) 
{
    // Initialize the MPI environment
    MPI_Init(0, 0);

    // Get the rank of the process
    int processes, currentProcessRank, size1, size2;
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &currentProcessRank);

    if (argc != 3 || 1 != sscanf_s(argv[1], "%u", &size1) || 1 != sscanf_s(argv[2], "%u", &size2)) {
        fprintf(stderr, "polynomials sizes must be given as args\n");
        return 1;
    }

    // generate 2 polynomials
    std::vector<int> polynomial1 = generatePolynomialCoefficients(size1);
    std::vector<int> polynomial2 = generatePolynomialCoefficients(size2);
    std::cout << size1 << " " << size2;

    // Finalize the MPI environment.
    MPI_Finalize();
}