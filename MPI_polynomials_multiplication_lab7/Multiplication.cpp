#include "Multiplication.hpp"
#include <iostream>

Multiplication::Multiplication()
{
}

Multiplication::Multiplication(std::vector<int> polynomial1, std::vector<int> polynomial2): polynomial1{polynomial1}, polynomial2{polynomial2}
{
    this->result = std::vector<int>(polynomial1.size() + polynomial2.size(), 0);
}

std::string Multiplication::getPolynomialResult()
{
	std::string result = "";
	for (int i = this->result.size() - 1; i >= 0; --i)
	{
		result += std::to_string(this->result[i]) + " * x^" + std::to_string(i) + " + ";
	}
	result.erase(result.size() - 3, 3);
	return result;
}
