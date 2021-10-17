#pragma once
#include <string>
#include <map>
#include <mutex>
#include "Bill.hpp"
#include "Inventory.hpp"

class Sale
{
private:
	std::string name;
	float earnedMoney;
	std::map<Product*, int> productsForSale;
	Inventory* inventory;
	std::mutex mutex;

public:
	Sale(std::string name, Inventory* inventory);
	void addProductToSale(Product* product, int requestedQuantity);
	void run();
	std::string getName();
	int getNoProductTypes();
	std::map<Product*, int> getProductsForSale();
}; 