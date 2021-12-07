#include <iostream>
#include <vector>
#include <chrono>
#include "RegularAlgorithm.hpp"
#include "RegularAlgorithmParallelized.hpp"
#include "KaratsubaAlgorithm.hpp"
#include "KaratsubaAlgorithmParallelized.hpp"


std::vector<int> generatePolynomialCoefficients(int size)
{
	std::vector<int> polynomial(size);
	srand(time(NULL));

	for (int i = 0; i < size - 1; ++i)
	{
		polynomial[i] = rand() % 4 + 1;
	}
	polynomial[size - 1] = rand() % 4 + 1;

	return polynomial;
}

int main()
{
	int size1, size2;

	std::cout << "Size polynomial 1: ";
	std::cin >> size1;

	std::cout << "Size polynomial 2: ";
	std::cin >> size2;

	std::vector<int> polynomial1 = generatePolynomialCoefficients(size1);
	std::vector<int> polynomial2 = generatePolynomialCoefficients(size2);

	RegularAlgorithm regularAlgo = RegularAlgorithm(polynomial1, polynomial2, size1, size2);
	RegularAlgorithmParallelized regularAlgoParallelized = RegularAlgorithmParallelized(polynomial1, polynomial2, size1, size2);
	KaratsubaAlgorithm karatsubaAlgo = KaratsubaAlgorithm(polynomial1, polynomial2, size1, size2);
	KaratsubaAlgorithmParallelized karatsubaAlgoParallelized = KaratsubaAlgorithmParallelized(polynomial1, polynomial2, size1, size2);

	std::chrono::milliseconds startR = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	regularAlgo.multiply();
	std::chrono::milliseconds endR = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	
	std::chrono::milliseconds startRP = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	regularAlgoParallelized.multiply();
	std::chrono::milliseconds endRP = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	
	std::chrono::milliseconds startK = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	karatsubaAlgo.multiply();
	std::chrono::milliseconds endK = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	
	std::chrono::milliseconds startKP = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	//karatsubaAlgoParallelized.multiply();
	std::chrono::milliseconds endKP = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());


	std::cout << "\nTime: " << (endR - startR).count() << " ms \n\n";
	std::cout << "\nTime: " << (endRP - startRP).count() << " ms \n";
	std::cout << "\nTime: " << (endK - startK).count() << " ms \n";
	//std::cout << "\nTime: " << (endKP - startKP).count() << " ms \n";

	return 0;
}