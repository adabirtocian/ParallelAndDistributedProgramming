#include <iostream>
#include <thread>
#include <chrono>
#include "Inventory.hpp"
#include "Sale.hpp"
#include "Bill.hpp"
#include "Product.hpp"


void runSale(Sale* sale)
{
	sale->run();
}

int main()
{
	int const static NUMBER_THREADS = 1000;
	int const static NUMBER_PRODUCTS = 10000;

	std::vector<Product*> allAvailableProducts;
	char letters[35] = "abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < NUMBER_PRODUCTS; ++i)
	{
		std::string name;
		name.append(1, letters[i % strlen(letters)]);
		name += std::to_string(rand() % 10);

		float price = rand() % 10;
		int quantity = rand() % 100;
		Product* product = new Product(name, price, quantity);
		allAvailableProducts.push_back(product);
	}

	//for (int i=0; i<allAvailableProducts.size(); ++i)
	//{
	//	std::cout << allAvailableProducts[i]->getName() << " " << allAvailableProducts[i]->getAvailableQuantity() << std::endl;
	//}
	Inventory* inventory = new Inventory(allAvailableProducts);
	std::vector<Sale*> sales;

	for (int i = 0; i < NUMBER_THREADS; ++i)
	{

		std::string name = "sale_" + std::to_string(rand() % 10);
		Sale* sale = new Sale(name, inventory);
		int numberProductsSale = rand() % 10;

		for (int j = 0; j < numberProductsSale; ++j)
		{
			Product* product = inventory->getProduct(j % inventory->getTotalNoProductTypes());
			int quantity = rand() % 100;
			sale->addProductToSale(product, quantity);
		}
		sales.push_back(sale);
	}

	//std::cout << "\nSales\n";
	//for (auto sale : sales)
	//{
	//	std::cout << sale->getName() << "\n";
	//	for (auto product: sale->getProductsForSale())
	//	{
	//		std::cout << product.first->getName() << " " << product.second << std::endl;
	//	}
	//}
	 
	std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::vector<std::thread> threads;
	for (int i = 0; i < NUMBER_THREADS; ++i)
	{
		threads.push_back(std::thread(runSale, sales[i]));
	}

	for (int i = 0; i < NUMBER_THREADS; ++i)
	{
		threads[i].join();
	}
	std::chrono::milliseconds end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

	std::cout << "Inventory status: " << inventory->checkStock() << " buget=" << inventory->getBuget() << "\n";
	std::cout << "Time: " << (end - start).count() << "\n";
	//std::cout<<"\nAfter sales\n";

	//for (int i = 0; i < allAvailableProducts.size(); ++i)
	//{
	//	std::cout << allAvailableProducts[i]->getName() << " " << allAvailableProducts[i]->getAvailableQuantity() << std::endl;
	//}
	//for (auto b : inventory->getBills())
	//{
	//	std::cout << b->getTotalPriceOfBill() << " " << b->getNoProducts() << std::endl;
	//}
	return 0;
}