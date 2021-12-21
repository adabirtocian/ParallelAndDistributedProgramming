#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include "Multiplication.hpp"
#include "RegularMultiplication.hpp"
#include "KaratsubaMultiplication.hpp"

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
    Multiplication* multiplication = new RegularMultiplication();

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

        if (method == "regular")
        {
            multiplication = new RegularMultiplication(polynomial1, polynomial2);
        }
        else if (method == "karatsuba")
        {
            multiplication = new KaratsubaMultiplication(polynomial1, polynomial2);
        }
        multiplication->master();
    }
    else
    {
        // worker process

    }

    // Finalize the MPI environment.
    MPI_Finalize();
}