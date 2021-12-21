#include "RegularMultiplication.hpp"
#include <iostream>

RegularMultiplication::RegularMultiplication() : Multiplication()
{
}

RegularMultiplication::RegularMultiplication(std::vector<int> polynomial1, std::vector<int> polynomial2) : Multiplication(polynomial1, polynomial2)
{
}

void RegularMultiplication::master()
{
	std::cout << "Regular\n";
}

void RegularMultiplication::worker()
{
}
