#pragma once
#include <vector>
#include <string>


class Multiplication
{
protected:
	std::vector<int> polynomial1;
	std::vector<int> polynomial2;
	std::vector<int> result;

public:
	Multiplication();
	Multiplication(std::vector<int> polynomial1, std::vector<int> polynomial2);
	virtual void master() = 0;
	virtual void worker() = 0;
	std::string getPolynomialResult();


};

