#pragma once
#include "Matrix.hpp"

class MatrixMultiplication
{
private:
	Matrix& a;
	Matrix& b;
	Matrix& result;
	int noTasks;

public:
	MatrixMultiplication(Matrix& a, Matrix& b, Matrix& result, int noTasks);
	void computeOneElement(int resultRow, int resultCol);
	void multiplySection(int startIdx, int stopIdx);
};

