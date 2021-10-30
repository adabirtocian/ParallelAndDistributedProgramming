#include <iostream>
#include <Windows.h>
#include "Matrix.hpp"
#include "MatrixMultiplication.hpp"
#include <thread>


void multiplyWithThreads(int startIdx, int stopIdx, MatrixMultiplication multiplication)
{
	multiplication.multiplySection(startIdx, stopIdx);
}
int main()
{
	const int ROWS_1 = 3;
	const int COLS_1 = 3;

	const int ROWS_2 = COLS_1;
	const int COLS_2 = 3;

	const int ROWS_RESULT = ROWS_1;
	const int COLS_RESULT = COLS_2;

	const int NO_TASKS = 4;

	Matrix a = Matrix(ROWS_1, COLS_1);
	a.initializeWithValues();
	Sleep(2000);
	Matrix b = Matrix(ROWS_2, COLS_2);
	b.initializeWithValues();

	a.print();
	std::cout << std::endl;
	b.print();

	Matrix result = Matrix(ROWS_RESULT, COLS_RESULT);
	MatrixMultiplication multiplication = MatrixMultiplication(a, b, result, NO_TASKS);

	std::vector<std::thread> threads;
	int noElemsPerTasks = (ROWS_RESULT * COLS_RESULT) / NO_TASKS;

	for (int i = 0; i < NO_TASKS; ++i)
	{
		if (i == NO_TASKS - 1)
		{
			threads.push_back(std::thread(multiplyWithThreads, i * noElemsPerTasks, ROWS_RESULT * COLS_RESULT - 1, multiplication));
		}
		else {
			threads.push_back(std::thread(multiplyWithThreads, i * noElemsPerTasks, (i + 1) * noElemsPerTasks - 1, multiplication));
		}
	}

	for (int i = 0; i < NO_TASKS; ++i)
	{
		threads[i].join();
	}

	result.print();

	return 0;
}