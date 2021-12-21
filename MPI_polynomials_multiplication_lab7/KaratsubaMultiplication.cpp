#include "KaratsubaMultiplication.hpp"
#include <iostream>

KaratsubaMultiplication::KaratsubaMultiplication() : Multiplication()
{
}

KaratsubaMultiplication::KaratsubaMultiplication(std::vector<int> polynomial1, std::vector<int> polynomial2) : Multiplication(polynomial1, polynomial2)
{
}

void KaratsubaMultiplication::master()
{
	std::cout << "Karatsuba\n";
}

void KaratsubaMultiplication::worker()
{
}
