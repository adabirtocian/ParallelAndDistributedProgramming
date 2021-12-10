#include "RegularAlgorithmParallelized.hpp"
#include <vector>
#include <thread>

void RegularAlgorithmParallelized::computeOnePowerOfX(int k, RegularAlgorithmParallelized* obj)
{
	int sumProducts = 0;
	for (int i = 0; i <= k; ++i)
	{
		int p1 = i < obj->size1 ? obj->polynomial1[i] : 0;
		int p2 = k - i < obj->size2 ? obj->polynomial2[k - i] : 0;

		sumProducts += p1 * p2;
	}
	obj->result[k] = sumProducts;
}

RegularAlgorithmParallelized::RegularAlgorithmParallelized(std::vector<int> polynomial1, std::vector<int> polynomial2, int size1, int size2) :
polynomial1 {polynomial1}, polynomial2{ polynomial2 }, size1{ size1 }, size2{ size2 }
{
	this->sizeResult = size1 + size2 - 1;
	this->result = std::vector<int>(this->sizeResult, 0);
}

void RegularAlgorithmParallelized::multiply()
{
	std::vector<std::thread> threads;
	for (int k = 0; k < this->size1 + this->size2 - 1; ++k)
	{
		threads.push_back(std::thread(computeOnePowerOfX, k, this));
	}

	for (int k = 0; k < this->size1 + this->size2 - 1; ++k)
	{
		threads[k].join();
	}
}

std::string RegularAlgorithmParallelized::getPolynomialResult()
{
	std::string result = "";
	for (int i = this->sizeResult - 1; i >= 0; --i)
	{
		result += std::to_string(this->result[i]) + " * x^" + std::to_string(i) + " + ";
	}
	result.erase(result.size() - 3, 3);
	return result;
}
