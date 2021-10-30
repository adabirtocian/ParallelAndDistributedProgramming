#pragma once
#include <vector>

class Matrix
{

private:
	std::vector<int> matrix;
	int rows;
	int cols;

public:
	Matrix(int rows, int cols);
	void initializeWithValues();
	int getRows();
	int getCols();
	int getValue(int row, int col);
	void setValue(int row, int col, int value);
	void print();
};

