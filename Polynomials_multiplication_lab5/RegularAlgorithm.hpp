#pragma once
#include <vector>
#include <string>

class RegularAlgorithm
{
private:
	std::vector<int> polynomial1;
	std::vector<int> polynomial2;
	std::vector<int> result;
	int size1;
	int size2;
	int sizeResult;

public:
	RegularAlgorithm(std::vector<int> polynomial1, std::vector<int> polynomial2, int size1, int size2);
	void multiply();
	std::string getPolynomialResult();
};
