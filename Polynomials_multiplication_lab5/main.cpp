#include <iostream>
#include <vector>
#include "RegularAlgorithm.hpp"


int main()
{
	int size1, size2;

	std::cout << "Size polynomial 1: ";
	std::cin >> size1;

	std::cout << "\nSize polynomial 2: ";
	std::cin >> size2;

	std::vector<int> polynomial1(size1);
	std::vector<int> polynomial2(size2);

	std::cout << "Polynomial 1 coefficients: \n";
	for (int i = 0; i < size1; ++i)
	{
		std::cin >> polynomial1[i];
	}

	std::cout << "Polynomial 2 coefficients: \n";
	for (int i = 0; i < size2; ++i)
	{
		std::cin >> polynomial2[i];
	}

	RegularAlgorithm regularAlgo = RegularAlgorithm(polynomial1, polynomial2, size1, size2);
	regularAlgo.multiply();
	std::cout << regularAlgo.getPolynomialResult();

	return 0;
}