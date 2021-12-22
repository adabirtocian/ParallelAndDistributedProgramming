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
    if (polynomial1.size() <= 2 || polynomial2.size() <= 2)
    {
        // regural polynomial multiplication
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
    MPI_Status status;

    if (processes == 1)
    {
        // sequential
        karatsubaAlgorithm(polynomial1, polynomial2);
        return;
    }

    int smallerLen = (polynomial1.size() > polynomial2.size() ? polynomial1.size() : polynomial2.size()) / 2;

    // split the polynomials in two parts
    std::vector<int> smallCoeffP1 = slice(0, smallerLen - 1, polynomial1);
    std::vector<int> bigCoeffP1 = slice(smallerLen, polynomial1.size() - 1, polynomial1);
    std::vector<int> smallCoeffP2 = slice(0, smallerLen - 1, polynomial2);
    std::vector<int> bigCoeffP2 = slice(smallerLen, polynomial2.size() - 1, polynomial2);

    // compute the sum of same part (small or high)
    std::vector<int> sumPartsP1 = add2Polynomials(smallCoeffP1, bigCoeffP1);
    std::vector<int> sumPartsP2 = add2Polynomials(smallCoeffP2, bigCoeffP2);

    int sizeSmallCoeffP1 = smallCoeffP1.size();
    int sizeSmallCoeffP2 = smallCoeffP2.size();
    int sizeBigCoeffP1 = bigCoeffP1.size();
    int sizeBigCoeffP2 = bigCoeffP2.size();
    int sizeSumParts1 = sumPartsP1.size();
    int sizeSumParts2 = sumPartsP2.size();
    int sizeSmallCoeffs = -1, sizeBigCoeffs = -1, sizeSumPartsCoeffs = -1, masterId = 0, totalRemainingWorkers = 0, splitNoWorkers = 0, noWorkersToSend;
    std::vector<int> sumPartsCoeffs;
    std::vector<int> bigCoeffs;
    std::vector<int> smallCoeffs;
    std::vector<int> middleCoeffs;
    std::vector<int> result1;
    std::vector<int> result2;
    std::vector<int> result;
    std::vector<int> workers;
    
    if (processes == 2)
    {
        // one worker
        // send lower parts to be computed by the worker
        MPI_Ssend(&masterId, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // send parent id
        MPI_Ssend(&sizeSmallCoeffP1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Ssend(smallCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, 1, 2, MPI_COMM_WORLD);
        MPI_Ssend(&sizeSmallCoeffP2, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Ssend(smallCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, 1, 2, MPI_COMM_WORLD);
        MPI_Ssend(&totalRemainingWorkers, 1, MPI_INT, 1, 3, MPI_COMM_WORLD); // send the remaining workers available

        // maser takes care of the rest
        sumPartsCoeffs = karatsubaRecursiveAsync(sumPartsP1, sumPartsP2);
        bigCoeffs = karatsubaRecursiveAsync(bigCoeffP1, bigCoeffP2);
        
        MPI_Recv(&sizeSmallCoeffs, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
        smallCoeffs = std::vector<int>(sizeSmallCoeffs, 0);
        MPI_Recv(smallCoeffs.data(), sizeSmallCoeffs, MPI_INT, 1, 2, MPI_COMM_WORLD, &status); // receive the result as vector
    }
    else if (processes == 3)
    {
        // 2 workers
        // send lower parts to be computed by the first worker
        MPI_Ssend(&masterId, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // send parent id
        MPI_Ssend(&sizeSmallCoeffP1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Ssend(smallCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, 1, 2, MPI_COMM_WORLD);
        MPI_Ssend(&sizeSmallCoeffP2, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Ssend(smallCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, 1, 2, MPI_COMM_WORLD);
        MPI_Ssend(&totalRemainingWorkers, 1, MPI_INT, 1, 3, MPI_COMM_WORLD); // send the remaining workers available
    
        // send higher parts to be computed by the second worker
        MPI_Ssend(&masterId, 1, MPI_INT, 2, 0, MPI_COMM_WORLD); // send parent id
        MPI_Ssend(&sizeBigCoeffP1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
        MPI_Ssend(bigCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, 2, 2, MPI_COMM_WORLD);
        MPI_Ssend(&sizeBigCoeffP2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
        MPI_Ssend(bigCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, 2, 2, MPI_COMM_WORLD);
        MPI_Ssend(&totalRemainingWorkers, 1, MPI_INT, 2, 3, MPI_COMM_WORLD); // send the remaining workers available
    
        // master takes care of the rest
        sumPartsCoeffs = karatsubaRecursiveAsync(sumPartsP1, sumPartsP2);
    
        MPI_Recv(&sizeSmallCoeffs, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
        smallCoeffs = std::vector<int>(sizeSmallCoeffs, 0);
        MPI_Recv(smallCoeffs.data(), sizeSmallCoeffs, MPI_INT, 1, 2, MPI_COMM_WORLD, &status); // receive the result as vector
    
        MPI_Recv(&sizeBigCoeffs, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
        bigCoeffs = std::vector<int>(sizeBigCoeffs, 0);
        MPI_Recv(bigCoeffs.data(), sizeBigCoeffs, MPI_INT, 2, 2, MPI_COMM_WORLD, &status); // receive the result as vector
    }
    else
    {
        // at least 3 workers
        totalRemainingWorkers = processes - 3;
        if (totalRemainingWorkers > 0)
        {
            workers = std::vector<int>(totalRemainingWorkers);
            for (int i = 3; i < 3 + totalRemainingWorkers; ++i)
            {
                workers[i-3] = i; 
            }
        }
        splitNoWorkers = totalRemainingWorkers % 2 == 0 ? totalRemainingWorkers / 2 : totalRemainingWorkers / 2 + 1;

        // send lower parts to be computed by the first worker
        MPI_Ssend(&masterId, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // send parent id
        MPI_Ssend(&sizeSmallCoeffP1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Ssend(smallCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, 1, 2, MPI_COMM_WORLD);
        MPI_Ssend(&sizeSmallCoeffP2, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Ssend(smallCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, 1, 2, MPI_COMM_WORLD);
        MPI_Ssend(&splitNoWorkers, 1, MPI_INT, 1, 3, MPI_COMM_WORLD); // send the remaining workers available
        
        auto splitWorkers = std::vector<int>(workers.begin(), workers.begin() + splitNoWorkers);
        MPI_Ssend(splitWorkers.data(), splitNoWorkers, MPI_INT, 1, 4, MPI_COMM_WORLD);
        
        // send higher parts to be computed by the second worker
        noWorkersToSend = totalRemainingWorkers - splitNoWorkers;
        MPI_Ssend(&masterId, 1, MPI_INT, 2, 0, MPI_COMM_WORLD); // send parent id
        MPI_Ssend(&sizeBigCoeffP1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
        MPI_Ssend(bigCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, 2, 2, MPI_COMM_WORLD);
        MPI_Ssend(&sizeBigCoeffP2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
        MPI_Ssend(bigCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, 2, 2, MPI_COMM_WORLD);
        MPI_Ssend(&noWorkersToSend, 1, MPI_INT, 2, 3, MPI_COMM_WORLD); // send the remaining workers available
        if (noWorkersToSend > 0)
        {
            auto splitWorkers = std::vector<int>(workers.begin() + splitNoWorkers, workers.end());
            MPI_Ssend(splitWorkers.data(), noWorkersToSend, MPI_INT, 2, 4, MPI_COMM_WORLD);
        }
        
        // master
        sumPartsCoeffs = karatsubaRecursiveAsync(sumPartsP1, sumPartsP2);
        
        MPI_Recv(&sizeSmallCoeffs, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
        smallCoeffs = std::vector<int>(sizeSmallCoeffs, 0);
        MPI_Recv(smallCoeffs.data(), sizeSmallCoeffs, MPI_INT, 1, 2, MPI_COMM_WORLD, &status); // receive the result as vector
        
        MPI_Recv(&sizeBigCoeffs, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
        bigCoeffs = std::vector<int>(sizeBigCoeffs, 0);
        MPI_Recv(bigCoeffs.data(), sizeBigCoeffs, MPI_INT, 2, 2, MPI_COMM_WORLD, &status); // receive the result as vector
    }

    middleCoeffs = substract2Polynomials(substract2Polynomials(sumPartsCoeffs, smallCoeffs), bigCoeffs);
    
    // aggregate the resulting polynomial
    result1 = shift(bigCoeffs, 2 * smallerLen);
    result2 = shift(middleCoeffs, smallerLen);
    result = add2Polynomials(add2Polynomials(result1, result2), smallCoeffs);
    
    printResult(result);
}

void karatsubaWorker(int processId)
{
    MPI_Status status;
    int parentId, size1, size2, workers, sizeResult;
    std::vector<int> polynomial1;
    std::vector<int> polynomial2;
    std::vector<int> workersIds;
    std::vector<int> result;

    MPI_Recv(&parentId, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status); // receive parent id
    MPI_Recv(&size1, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status); // receive size of first polynomial
    polynomial1 = std::vector<int>(size1);
    MPI_Recv(polynomial1.data(), size1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
    MPI_Recv(&size2, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status); // receive size of second polynomial
    polynomial2 = std::vector<int>(size2);
    MPI_Recv(polynomial2.data(), size2, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
    MPI_Recv(&workers, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
    
    if (workers != 0)
    {
        workersIds = std::vector<int>(workers);
        MPI_Recv(workersIds.data(), workers, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, &status);

        // split work between workers
        int smallerLen = (polynomial1.size() > polynomial2.size() ? polynomial1.size() : polynomial2.size()) / 2;
    
        // split the polynomials in two parts
        std::vector<int> smallCoeffP1 = slice(0, smallerLen - 1, polynomial1);
        std::vector<int> bigCoeffP1 = slice(smallerLen, polynomial1.size() - 1, polynomial1);
        std::vector<int> smallCoeffP2 = slice(0, smallerLen - 1, polynomial2);
        std::vector<int> bigCoeffP2 = slice(smallerLen, polynomial2.size() - 1, polynomial2);
    
        // compute the sum of same part (small or high)
        std::vector<int> sumPartsP1 = add2Polynomials(smallCoeffP1, bigCoeffP1);
        std::vector<int> sumPartsP2 = add2Polynomials(smallCoeffP2, bigCoeffP2);
    
        int sizeSmallCoeffP1 = smallCoeffP1.size();
        int sizeSmallCoeffP2 = smallCoeffP2.size();
        int sizeBigCoeffP1 = bigCoeffP1.size();
        int sizeBigCoeffP2 = bigCoeffP2.size();
        int sizeSumParts1 = sumPartsP1.size();
        int sizeSumParts2 = sumPartsP2.size();
        int sizeSmallCoeffs = -1, sizeBigCoeffs = -1, sizeSumPartsCoeffs = -1, totalRemainingWorkers = 0, splitNoWorkers = 0, noWorkersToSend;
        std::vector<int> sumPartsCoeffs;
        std::vector<int> bigCoeffs;
        std::vector<int> smallCoeffs;
        std::vector<int> middleCoeffs;
        std::vector<int> result1;
        std::vector<int> result2;
        std::vector<int> nextWorkersId;
    
        if (workers == 1)
        {
            // one worker
            // send lower parts to be computed by the worker
            MPI_Ssend(&processId, 1, MPI_INT, workersIds[0], 0, MPI_COMM_WORLD); // send parent id
            MPI_Ssend(&sizeSmallCoeffP1, 1, MPI_INT, workersIds[0], 1, MPI_COMM_WORLD);
            MPI_Ssend(smallCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, workersIds[0], 2, MPI_COMM_WORLD);
            MPI_Ssend(&sizeSmallCoeffP2, 1, MPI_INT, workersIds[0], 1, MPI_COMM_WORLD);
            MPI_Ssend(smallCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, workersIds[0], 2, MPI_COMM_WORLD);
            MPI_Ssend(&totalRemainingWorkers, 1, MPI_INT, workersIds[0], 3, MPI_COMM_WORLD); // send the remaining workers available
            
            // master takes care of the rest
            sumPartsCoeffs = karatsubaRecursiveAsync(sumPartsP1, sumPartsP2);
            bigCoeffs = karatsubaRecursiveAsync(bigCoeffP1, bigCoeffP2);
            
            MPI_Recv(&sizeSmallCoeffs, 1, MPI_INT, workersIds[0], 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
            smallCoeffs = std::vector<int>(sizeSmallCoeffs, 0);
            MPI_Recv(smallCoeffs.data(), sizeSmallCoeffs, MPI_INT, workersIds[0], 2, MPI_COMM_WORLD, &status); // receive the result as vector
        }
        else if (workers == 2)
        {
            // 2 workers
            // send lower parts to be computed by the first worker
            MPI_Ssend(&processId, 1, MPI_INT, workersIds[0], 0, MPI_COMM_WORLD); // send parent id
            MPI_Ssend(&sizeSmallCoeffP1, 1, MPI_INT, workersIds[0], 1, MPI_COMM_WORLD);
            MPI_Ssend(smallCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, workersIds[0], 2, MPI_COMM_WORLD);
            MPI_Ssend(&sizeSmallCoeffP2, 1, MPI_INT, workersIds[0], 1, MPI_COMM_WORLD);
            MPI_Ssend(smallCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, workersIds[0], 2, MPI_COMM_WORLD);
            MPI_Ssend(&totalRemainingWorkers, 1, MPI_INT, workersIds[0], 3, MPI_COMM_WORLD); // send the remaining workers available
            
            // send higher parts to be computed by the second worker
            MPI_Ssend(&processId, 1, MPI_INT, workersIds[1], 0, MPI_COMM_WORLD); // send parent id
            MPI_Ssend(&sizeBigCoeffP1, 1, MPI_INT, workersIds[1], 1, MPI_COMM_WORLD);
            MPI_Ssend(bigCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, workersIds[1], 2, MPI_COMM_WORLD);
            MPI_Ssend(&sizeBigCoeffP2, 1, MPI_INT, workersIds[1], 1, MPI_COMM_WORLD);
            MPI_Ssend(bigCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, workersIds[1], 2, MPI_COMM_WORLD);
            MPI_Ssend(&totalRemainingWorkers, 1, MPI_INT, workersIds[1], 3, MPI_COMM_WORLD); // send the remaining workers available
            
            // master takes care of the rest
            sumPartsCoeffs = karatsubaRecursiveAsync(sumPartsP1, sumPartsP2);
            
            MPI_Recv(&sizeSmallCoeffs, 1, MPI_INT, workersIds[0], 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
            smallCoeffs = std::vector<int>(sizeSmallCoeffs, 0);
            MPI_Recv(smallCoeffs.data(), sizeSmallCoeffs, MPI_INT, workersIds[0], 2, MPI_COMM_WORLD, &status); // receive the result as vector
            
            MPI_Recv(&sizeBigCoeffs, 1, MPI_INT, workersIds[1], 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
            bigCoeffs = std::vector<int>(sizeBigCoeffs, 0);
            MPI_Recv(bigCoeffs.data(), sizeBigCoeffs, MPI_INT, workersIds[1], 2, MPI_COMM_WORLD, &status); // receive the result as vector
        }
        else
        {
            // at least 3 workers
            totalRemainingWorkers = workersIds.size() - 2;
            splitNoWorkers = totalRemainingWorkers % 2 == 0 ? totalRemainingWorkers / 2 : totalRemainingWorkers / 2 + 1;
            nextWorkersId = std::vector<int>(workersIds.begin() + 2, workersIds.end());

            // send lower parts to be computed by the first worker
            MPI_Ssend(&processId, 1, MPI_INT, workersIds[0], 0, MPI_COMM_WORLD); // send parent id
            MPI_Ssend(&sizeSmallCoeffP1, 1, MPI_INT, workersIds[0], 1, MPI_COMM_WORLD);
            MPI_Ssend(smallCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, workersIds[0], 2, MPI_COMM_WORLD);
            MPI_Ssend(&sizeSmallCoeffP2, 1, MPI_INT, workersIds[0], 1, MPI_COMM_WORLD);
            MPI_Ssend(smallCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, workersIds[0], 2, MPI_COMM_WORLD);
            MPI_Ssend(&splitNoWorkers, 1, MPI_INT, workersIds[0], 3, MPI_COMM_WORLD); // send the remaining workers available
            auto splitWorkers = std::vector<int>(nextWorkersId.begin() , nextWorkersId.begin() + splitNoWorkers);
            MPI_Ssend(splitWorkers.data(), splitNoWorkers, MPI_INT, workersIds[0], 4, MPI_COMM_WORLD);

            // send higher parts to be computed by the second worker
            noWorkersToSend = totalRemainingWorkers - splitNoWorkers;
            MPI_Ssend(&processId, 1, MPI_INT, workersIds[1], 0, MPI_COMM_WORLD); // send parent id
            MPI_Ssend(&sizeBigCoeffP1, 1, MPI_INT, workersIds[1], 1, MPI_COMM_WORLD);
            MPI_Ssend(bigCoeffP1.data(), sizeSmallCoeffP1, MPI_INT, workersIds[1], 2, MPI_COMM_WORLD);
            MPI_Ssend(&sizeBigCoeffP2, 1, MPI_INT, workersIds[1], 1, MPI_COMM_WORLD);
            MPI_Ssend(bigCoeffP2.data(), sizeSmallCoeffP2, MPI_INT, workersIds[1], 2, MPI_COMM_WORLD);
            MPI_Ssend(&noWorkersToSend, 1, MPI_INT, workersIds[1], 3, MPI_COMM_WORLD); // send the remaining workers available
            if (noWorkersToSend > 0)
            {
                auto splitWorkers = std::vector<int>(nextWorkersId.begin() + splitNoWorkers, nextWorkersId.end());
                MPI_Ssend(splitWorkers.data(), noWorkersToSend, MPI_INT, workersIds[1], 4, MPI_COMM_WORLD);
            }

            // master
            sumPartsCoeffs = karatsubaRecursiveAsync(sumPartsP1, sumPartsP2);

            MPI_Recv(&sizeSmallCoeffs, 1, MPI_INT, workersIds[0], 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
            smallCoeffs = std::vector<int>(sizeSmallCoeffs, 0);
            MPI_Recv(smallCoeffs.data(), sizeSmallCoeffs, MPI_INT, workersIds[0], 2, MPI_COMM_WORLD, &status); // receive the result as vector

            MPI_Recv(&sizeBigCoeffs, 1, MPI_INT, workersIds[1], 1, MPI_COMM_WORLD, &status); // receive size of the partial computation
            bigCoeffs = std::vector<int>(sizeBigCoeffs, 0);
            MPI_Recv(bigCoeffs.data(), sizeBigCoeffs, MPI_INT, workersIds[1], 2, MPI_COMM_WORLD, &status); // receive the result as vector
        }
    
        middleCoeffs = substract2Polynomials(substract2Polynomials(sumPartsCoeffs, smallCoeffs), bigCoeffs);
        
        // aggregate the resulting polynomial
        result1 = shift(bigCoeffs, 2 * smallerLen);
        result2 = shift(middleCoeffs, smallerLen);
        result = add2Polynomials(add2Polynomials(result1, result2), smallCoeffs);
        
        sizeResult = result.size();
        MPI_Ssend(&sizeResult, 1, MPI_INT, parentId, 1, MPI_COMM_WORLD);
        MPI_Ssend(result.data(), sizeResult, MPI_INT, parentId, 2, MPI_COMM_WORLD);
        
        return;
    }

    // no more workers
    result = karatsubaRecursiveAsync(polynomial1, polynomial2);
    sizeResult = result.size();
    MPI_Ssend(&sizeResult, 1, MPI_INT, parentId, 1, MPI_COMM_WORLD);
    MPI_Ssend(result.data(), sizeResult, MPI_INT, parentId, 2, MPI_COMM_WORLD);
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
        //std::cout << "\n";
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
            karatsubaWorker(currentProcessRank);
        }
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}