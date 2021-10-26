#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>

const int NO_ELEMS = 100;
std::vector<int> vector1 = std::vector<int>(NO_ELEMS);
std::vector<int> vector2 = std::vector<int>(NO_ELEMS);
std::mutex mutex;
std::condition_variable condVar;
bool readyToSumUp;
bool endScalarProduct;
int productOnPos;
int scalarProduct;

// calculate scalar product
void productOnSamePositon()
{
	// iterate over the arrays 
	for (int i = 0; i < NO_ELEMS; ++i)
	{
		// make a unique lock and wait on the conditional variable for notification from consumer
		std::unique_lock<std::mutex> uniqueLock(mutex);
		condVar.wait(uniqueLock, [] { return !readyToSumUp;  });

		// make product on position i
		productOnPos = vector1[i] * vector2[i];
		readyToSumUp = true;
		std::cout << "Producer: " << productOnPos << std::endl;
		
		// unlock the lock and notify consumer to start summing up
		uniqueLock.unlock();
		condVar.notify_one();
	}

	endScalarProduct = true;
}

// sum up all the scalar products
void sumProducts()
{
	// check if we are at the end of the arrays
	while (!endScalarProduct)
	{
		// make a unique lock and wait on the conditional variable for notification from producer
		std::unique_lock<std::mutex> uniqueLock(mutex);
		condVar.wait(uniqueLock, [] { return readyToSumUp; });

		// sum up to global sum
		scalarProduct += productOnPos;
		readyToSumUp = false;
		std::cout << "Consumer: " << productOnPos << ", Scalar Product: " << scalarProduct << std::endl;

		// unlock lock and notify producer to continue
		uniqueLock.unlock();
		condVar.notify_one();
	}
}

int main()
{	
	scalarProduct = 0;
	readyToSumUp = endScalarProduct = false;
	
	srand(time(NULL));

	for (int i = 0; i < NO_ELEMS; ++i)
	{
		vector1[i] = rand() % 1000 + 1;
	}

	srand(time(NULL));
	for (int i = 0; i < NO_ELEMS; ++i)
	{
		vector2[i] = rand() % 1000 + 1;
	}

	std::thread producer = std::thread(productOnSamePositon);
	std::thread consumer = std::thread(sumProducts);
	producer.join();
	consumer.join();

	std::cout << "Scalar Product: " << scalarProduct;

	return 0;
}