#include "Sale.hpp"
#include <iostream>
#include "Inventory.hpp"

Sale::Sale(std::string name, Inventory* inventory): name { name }, inventory { inventory }
{
	this->earnedMoney = 0.0f;
}

void Sale::addProductToSale(Product* product, int requestedQuantity)
{
	this->productsForSale.insert(std::pair<Product*, int>(product, requestedQuantity));
}

void Sale::run()
{
	this->mutex.lock();
	//std::cout << "Start sale " << this->name << "\n";
	Bill* bill = new Bill();
	for (std::pair<Product*, int> pair : this->productsForSale)
	{
		Product* product = pair.first;
		int requestedQuantity = pair.second;
		this->inventory->buyProduct(product, requestedQuantity, *bill);
		//std::cout << "bill: " << bill->getNoProducts() << std::endl;
	}
	this->inventory->addBill(bill);
	this->mutex.unlock();
}

std::string Sale::getName()
{
	return this->name;
}

int Sale::getNoProductTypes()
{
	return this->productsForSale.size();
}

std::map<Product*, int> Sale::getProductsForSale()
{
	return this->productsForSale;
}


