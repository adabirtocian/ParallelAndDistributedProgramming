#include "Sale.hpp"
#include <iostream>
#include "Inventory.hpp"

Sale::Sale(std::string name, Inventory* inventory): name { name }, inventory { inventory }, earnedMoney(0.0f)
{}

void Sale::addProductToSale(Product* product, int requestedQuantity)
{
	this->productsForSale.insert(std::pair<Product*, int>(product, requestedQuantity));
}

void Sale::run()
{
	//std::cout << "Start sale " << this->name << "\n";
	Bill* bill = new Bill();
	for (std::pair<Product*, int> pair : this->productsForSale)
	{
		Product* product = pair.first;
		int requestedQuantity = pair.second;
		this->inventory->buyProduct(product, requestedQuantity, *bill);
	}
	this->inventory->addBill(bill);
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


