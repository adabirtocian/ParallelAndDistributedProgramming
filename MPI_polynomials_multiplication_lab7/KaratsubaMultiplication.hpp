#pragma once
#include "Multiplication.hpp"

class KaratsubaMultiplication : public Multiplication
{
public:
	KaratsubaMultiplication();
	KaratsubaMultiplication(std::vector<int> polynomial1, std::vector<int> polynomial2);
	void master();
	void worker();
};

