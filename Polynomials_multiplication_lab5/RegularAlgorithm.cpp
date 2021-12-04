#include "RegularAlgorithm.hpp"

RegularAlgorithm::RegularAlgorithm(std::vector<int> polynomial1, std::vector<int> polynomial2, int size1, int size2): 
	polynomial1{polynomial1}, polynomial2{polynomial2}, size1{size1}, size2{size2}
{
	this->sizeResult = size1 + size2 - 1;
	this->result = std::vector<int>(this->sizeResult, 0);
}

void RegularAlgorithm::multiply()
{
	for (int i = 0; i < this->size1; ++i)
	{
		for (int j = 0; j < this->size2; ++j)
		{
			this->result[i + j] += this->polynomial1[i] * this->polynomial2[j];
		}
	}
}

std::string RegularAlgorithm::getPolynomialResult()
{
	std::string result = "";
	for (int i = this->sizeResult - 1; i >= 0; --i)
	{
		result += std::to_string(this->result[i]) + " * x^" + std::to_string(i) + " + ";
	}
	result.erase(result.size() - 3, 3);
	return result;
}
