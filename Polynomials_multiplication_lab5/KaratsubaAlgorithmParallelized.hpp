#pragma once
#include <vector>
#include <string>
#include "RegularAlgorithm.hpp"

class KaratsubaAlgorithmParallelized
{
private:
	std::vector<int> polynomial1;
	std::vector<int> polynomial2;
	std::vector<int> result;
	int size1;
	int size2;

	static std::vector<int> karatsubaAlgorithm(std::vector<int> polynomial1, std::vector<int> polynomial2);
	static std::vector<int> slice(int start, int stop, std::vector<int> original);
	static std::vector<int> add2Polynomials(std::vector<int> polynomial1, std::vector<int> polynomial2);
	static std::vector<int> substract2Polynomials(std::vector<int> polynomial1, std::vector<int> polynomial2);
	static std::vector<int> shift(std::vector<int> original, int offset);

public:
	KaratsubaAlgorithmParallelized(std::vector<int> polynomial1, std::vector<int> polynomial2, int size1, int size2);
	void multiply();
	std::string getPolynomialResult();
};

