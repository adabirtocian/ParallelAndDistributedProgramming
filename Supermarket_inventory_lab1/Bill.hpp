#pragma once
#include <map>
#include "Product.hpp"

class Bill
{
private:
	std::map<Product*, int> soldProducts;
	float totalPrice;

public:
	Bill();
	void addProductToBill(Product* product, int quantity);
	float getTotalPriceOfBill();
	int getNoProducts();
};