#include <iostream>
#include <vector>
#include <chrono>
#include "RegularAlgorithm.hpp"
#include "RegularAlgorithmParallelized.hpp"


std::vector<int> generatePolynomialCoefficients(int size)
{
	std::vector<int> polynomial(size);
	srand(time(NULL));

	for (int i = 0; i < size - 1; ++i)
	{
		polynomial[i] = rand() % 1000;
		std::cout << polynomial[i] << " * x^" << i << " + ";
	}
	polynomial[size - 1] = rand() % 1000 + 1;
	std::cout << polynomial[size - 1] << " * x^" << size - 1 << "\n";

	return polynomial;
}

int main()
{
	int size1, size2;

	std::cout << "Size polynomial 1: ";
	std::cin >> size1;

	std::cout << "Size polynomial 2: ";
	std::cin >> size2;

	std::cout << "Polynomial 1 coefficients: \n";
	std::vector<int> polynomial1 = generatePolynomialCoefficients(size1);

	std::cout << "Polynomial 2 coefficients: \n";
	std::vector<int> polynomial2 = generatePolynomialCoefficients(size2);

	RegularAlgorithm regularAlgo = RegularAlgorithm(polynomial1, polynomial2, size1, size2);
	RegularAlgorithmParallelized regularAlgoParallelized = RegularAlgorithmParallelized(polynomial1, polynomial2, size1, size2);
	
	std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	
	//regularAlgo.multiply();
	regularAlgoParallelized.multiply();

	std::chrono::milliseconds end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	//std::cout << "Result: \n" << regularAlgo.getPolynomialResult() << "\n";
	std::cout << "Result: \n" << regularAlgoParallelized.getPolynomialResult() << "\n";
	std::cout << "Time: " << (end - start).count() << " ms \n";

	return 0;
}