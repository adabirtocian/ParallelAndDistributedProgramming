#pragma once
#include <vector>
#include <mutex>
#include "Product.hpp"
#include "Bill.hpp"

class Inventory
{
private:
	std::vector<Product*> products;
	std::vector<Bill*> bills;
	float buget;
	std::mutex mutex;
public:
	Inventory(std::vector<Product*> products);
	void buyProduct(Product* product, int soldQuantity, Bill& bill);
	int getAvailableQuantity(Product* product);
	bool checkStock();
	void addBill(Bill* bill);
	Product* getProduct(int index);
	int getTotalNoProductTypes();
	float getBuget();
	std::vector<Bill*> getBills();
};