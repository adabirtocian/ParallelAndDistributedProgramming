#include "MatrixMultiplication.hpp"
#include <iostream>
#include <thread>

MatrixMultiplication::MatrixMultiplication(Matrix& a, Matrix& b, Matrix& result, int noTasks): a(a), b(b), result(result), noTasks(noTasks)
{
}

void MatrixMultiplication::computeOneElement(int resultRow, int resultCol)
{
	int value = 0;
	for (int i = 0; i < this->a.getCols(); ++i)
	{
		value += this->a.getValue(resultRow, i) * this->b.getValue(i, resultCol);
	}
	this->result.setValue(resultRow, resultCol, value);
}

void MatrixMultiplication::multiplySection(int startIdx, int stopIdx)
{
	for (int i = startIdx; i <= stopIdx; ++i)
	{
		int row = i / this->result.getCols();
		int col = i % this->result.getCols();
		this->computeOneElement(row, col);
	}
}
