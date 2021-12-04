#include <iostream>
#include <vector>
#include "RegularAlgorithm.hpp"

std::vector<int> generatePolynomialCoefficients(int size)
{
	std::vector<int> polynomial(size);
	srand(time(NULL));

	for (int i = 0; i < size - 1; ++i)
	{
		polynomial[i] = rand() % 10;
		std::cout << polynomial[i] << " * x^" << i << " + ";
	}
	polynomial[size - 1] = rand() % 10 + 1;
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
	regularAlgo.multiply();
	std::cout << "Result: \n" << regularAlgo.getPolynomialResult();

	return 0;
}