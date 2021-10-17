#pragma once
#include <string>
#include <mutex>

class Product
{
private:
	std::string name;
	float price;
	int availableQuantity;
	std::mutex mutex;

public:
	Product(std::string name, float price, int availableQuantity);
	void setPrice(float price);
	float getPrice();
	void setName(std::string name);
	std::string getName();
	bool decreaseAvailableQuantity(int quantity);
	int getAvailableQuantity();
};
