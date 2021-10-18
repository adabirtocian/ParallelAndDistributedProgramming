#include "Inventory.hpp"
#include <iostream>
#include <mutex>


Inventory::Inventory(std::vector<Product*> products): products {products}, buget(0.0f), bills{ std::vector<Bill*>() }
{}

void Inventory::buyProduct(Product* product, int quantity, Bill& bill)
{
	if (!product->decreaseAvailableQuantity(quantity))
	{
		return;
	}
	this->buget += quantity * product->getPrice();

	bill.addProductToBill(product, quantity);
	//std::cout << "Successfully sold " << quantity << " from product " << product->getName() << " !\n";
}

int Inventory::getAvailableQuantity(Product* product)
{
	return product->getAvailableQuantity();
}

bool Inventory::checkStock()
{
	float moneyBills = 0;
	for(auto bill : this->bills)
	{
		moneyBills += bill->getTotalPriceOfBill();
	}
	return this->buget == moneyBills;
}

void Inventory::addBill(Bill* bill)
{
	this->mutex.lock();
	this->bills.push_back(bill);
	this->mutex.unlock();
}

Product* Inventory::getProduct(int index)
{
	return this->products.at(index);
}

int Inventory::getTotalNoProductTypes()
{
	return this->products.size();
}

float Inventory::getBuget()
{
	return this->buget;
}

std::vector<Bill*> Inventory::getBills()
{
	return this->bills;
}

