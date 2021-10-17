#include "Bill.hpp"
#include <iostream>

Bill::Bill()
{
	this->soldProducts = std::map<Product*, int>();
	this->totalPrice = 0.0f;
}

void Bill::addProductToBill(Product* product, int quantity)
{
	auto findResult = this->soldProducts.find(product);
	if (findResult == this->soldProducts.end()) // does not exist in current bill
	{
		this->soldProducts.insert(std::pair<Product*, int>(product, quantity));
		this->totalPrice += product->getPrice() * quantity;
		return;
	}
	// already added to bill
	this->totalPrice += product->getPrice() * quantity;
	int updatedQuantity = findResult->second + quantity;
	findResult->second = updatedQuantity;
}

float Bill::getTotalPriceOfBill()
{
	return this->totalPrice;
}

int Bill::getNoProducts()
{
	return this->soldProducts.size();
}

