#pragma once
#include <vector>
#include <string>

class RegularAlgorithmParallelized
{
private:
	std::vector<int> polynomial1;
	std::vector<int> polynomial2;
	std::vector<int> result;
	int size1;
	int size2;
	int sizeResult;

	static void computeOnePowerOfX(int power, RegularAlgorithmParallelized* obj);

public:
	RegularAlgorithmParallelized(std::vector<int> polynomial1, std::vector<int> polynomial2, int size1, int size2);
	void multiply();
	std::string getPolynomialResult();
};

