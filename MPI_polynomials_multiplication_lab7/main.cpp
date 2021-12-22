#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <future>
#include <thread> 

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

void printResult(std::vector<int> result)
{
    std::string resultStr = "";
    for (int i = result.size() - 1; i >= 0; --i)
    {
        if (result[i] != 0)
        {
            resultStr += std::to_string(result[i]) + " * x^" + std::to_string(i) + " + ";
        }
    }
    resultStr.erase(resultStr.size() - 3, 3);   //eliminate last plus sign from string

    std::cout << "\nResult: " << resultStr << "\n";
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

    //std::cout << "Worker "<<processId << ": " << start << " " << end << "\n";

    // do the computations
    std::vector<int> partialResult = multiplySection(size1, size2, start, end, polynomial1, polynomial2);

    // send the result back to master
    MPI_Ssend(partialResult.data(), partialResult.size(), MPI_INT, 0, 2, MPI_COMM_WORLD);
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

        //std::cout << "Master sends: " << start << " " << end << "\n";
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
    std::vector<int> workerResult = std::vector<int>(size1 + size2-1, 0);

    for (int i = 1; i < processes; ++i)
    {
        MPI_Recv(workerResult.data(), size1 + size2-1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
        finalResult = aggregateResults(finalResult, workerResult);
    }
    //for (int i = 0; i < finalResult.size(); ++i)
    //{
    //    std::cout << finalResult[i] << " ";
    //}
    printResult(finalResult);
}


std::vector<int> slice(int start, int stop, std::vector<int> original)
{
    std::vector<int> slice;
    for (int i = start; i <= stop; ++i)
    {
        slice.push_back(original[i]);
    }

    return slice;
}

std::vector<int> add2Polynomials(std::vector<int> polynomial1, std::vector<int> polynomial2)
{
    std::vector<int> result;
    int smallerLen = polynomial1.size() < polynomial2.size() ? polynomial1.size() : polynomial2.size();
    for (int i = 0; i < smallerLen; ++i)
    {
        result.push_back(polynomial1[i] + polynomial2[i]);
    }

    for (int i = smallerLen; i < polynomial1.size(); ++i)
    {
        result.push_back(polynomial1[i]);
    }

    for (int i = smallerLen; i < polynomial2.size(); ++i)
    {
        result.push_back(polynomial2[i]);

    }
    return result;
}

std::vector<int> substract2Polynomials(std::vector<int> polynomial1, std::vector<int> polynomial2)
{
    std::vector<int> result;
    int smallerLen = polynomial1.size() < polynomial2.size() ? polynomial1.size() : polynomial2.size();
    for (int i = 0; i < smallerLen; ++i)
    {
        result.push_back(polynomial1[i] - polynomial2[i]);
    }

    for (int i = smallerLen; i < polynomial1.size(); ++i)
    {
        result.push_back(polynomial1[i]);
    }

    for (int i = smallerLen; i < polynomial2.size(); ++i)
    {
        result.push_back(polynomial2[i]);

    }
    return result;
}

std::vector<int> shift(std::vector<int> original, int offset)
{
    std::vector<int> shifted = std::vector<int>(offset, 0);
    for (int i = 0; i < original.size(); ++i)
    {
        shifted.push_back(original[i]);
    }

    return shifted;
}


std::vector<int> karatsubaRecursiveAsync(std::vector<int> polynomial1, std::vector<int> polynomial2)
{
    if (polynomial1.size() == 2 || polynomial2.size() == 2)
    {
        std::vector<int> result = std::vector<int>(polynomial1.size() + polynomial2.size()-1,0);
        for (int i = 0; i < polynomial1.size(); ++i)
        {
            for (int j = 0; j < polynomial2.size(); ++j)
            {
                result[i + j] += polynomial1[i] * polynomial2[j];
            }
        }

        return result;
    }

    int smallerLen = (polynomial1.size() > polynomial2.size() ? polynomial1.size() : polynomial2.size()) / 2;
    
    // split the polynomials in two parts
    std::vector<int> smallCoeffP1 = slice(0, smallerLen - 1, polynomial1);
    std::vector<int> bigCoeffP1 = slice(smallerLen, polynomial1.size() - 1, polynomial1);

    std::vector<int> smallCoeffP2 = slice(0, smallerLen - 1, polynomial2);
    std::vector<int> bigCoeffP2 = slice(smallerLen, polynomial2.size() - 1, polynomial2);

    std::vector<int> sumPartsP1 = add2Polynomials(smallCoeffP1, bigCoeffP1);
    std::vector<int> sumPartsP2 = add2Polynomials(smallCoeffP2, bigCoeffP2);

    // decompose in smaller parts
    std::future<std::vector<int>> smallCoeffsFuture = std::async(&karatsubaRecursiveAsync, smallCoeffP1, smallCoeffP2);
    std::future<std::vector<int>> sumPartsCoeffsFuture = std::async(&karatsubaRecursiveAsync, sumPartsP1, sumPartsP2);
    std::future<std::vector<int>> bigCoeffsFuture = std::async(&karatsubaRecursiveAsync, bigCoeffP1, bigCoeffP2);
    std::vector<int> sumPartsCoeffs = sumPartsCoeffsFuture.get();
    std::vector<int> bigCoeffs = bigCoeffsFuture.get();
    std::vector<int> smallCoeffs = smallCoeffsFuture.get();

    std::vector<int> middleCoeffs = substract2Polynomials(substract2Polynomials(sumPartsCoeffs, smallCoeffs), bigCoeffs);

    // aggregate the resulting polynomial
    std::vector<int> result1 = shift(bigCoeffs, 2 * smallerLen);
    std::vector<int> result2 = shift(middleCoeffs, smallerLen);
    std::vector<int> result = add2Polynomials(add2Polynomials(result1, result2), smallCoeffs);

    return result;
}

void karatsubaAlgorithm(std::vector<int> polynomial1, std::vector<int> polynomial2)
{
    std::vector<int> result = karatsubaRecursiveAsync(polynomial1, polynomial2);
    printResult(result);
}

void karatsubaMaster(int processes, std::vector<int> polynomial1, std::vector<int> polynomial2)
{
    int size1 = polynomial1.size();
    int size2 = polynomial2.size();

    if (processes == 1)
    {
        // sequential
        karatsubaAlgorithm(polynomial1, polynomial2);
    }
    //// send the data 
    //MPI_Ssend(&size1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
    //MPI_Ssend(polynomial1.data(), size1, MPI_INT, 1, 2, MPI_COMM_WORLD);
    //MPI_Ssend(&size2, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
    //MPI_Ssend(polynomial2.data(), size2, MPI_INT, 1, 2, MPI_COMM_WORLD);
}

void karatsubaWorker(int processId)
{

}

std::vector<int> zeroPadding(std::vector<int> polynomial, int afterPaddingSize)
{
    std::vector<int> paddedPolynomial = std::vector<int>(afterPaddingSize, 0);
    for (int i = 0; i < polynomial.size(); ++i)
    {
        paddedPolynomial[i] = polynomial[i];
    }

    return paddedPolynomial;
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
        if (polynomial1.size() < polynomial2.size())
        {
            polynomial1 = zeroPadding(polynomial1, polynomial2.size());
        }
        else if(polynomial2.size() < polynomial1.size())
        {
            polynomial2 = zeroPadding(polynomial2, polynomial1.size());
        }

        // set the method to be used in multiplication
        if (method == "regular")
        {
            regularMaster(processes, polynomial1, polynomial2);
        }
        else if (method == "karatsuba")
        {
            karatsubaMaster(processes, polynomial1, polynomial2);
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