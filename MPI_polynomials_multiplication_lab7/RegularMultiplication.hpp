#pragma once
#include "Multiplication.hpp"

class RegularMultiplication : public Multiplication
{

public:
	RegularMultiplication();
	RegularMultiplication(std::vector<int> polynomial1, std::vector<int> polynomial2);
	void master();
	void worker();

};

