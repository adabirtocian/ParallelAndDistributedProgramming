#include "Product.hpp"
#include <iostream>
#include <mutex>

Product::Product(std::string name, float price, int availableQuantity): name {name}, price {price}, availableQuantity {availableQuantity}
{
}

void Product::setPrice(float price)
{
	this->price = price;
}

float Product::getPrice()
{
	return this->price;
}

void Product::setName(std::string name)
{
	this->name = name;
}

std::string Product::getName()
{
	return this->name;
}

bool Product::decreaseAvailableQuantity(int quantity)
{
	this->mutex.lock();
	if (this->availableQuantity >= quantity)
	{
		this->availableQuantity -= quantity;
		this->mutex.unlock();
		return true;
	}
	
	//std::cout<<"Cannot withdraw quantity of unexisting product " << this->name << " !\n";
	this->mutex.unlock();
	return false;
}

int Product::getAvailableQuantity()
{
	return this->availableQuantity;
}
