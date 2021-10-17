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

//void generateProducts(int numberProducts, std::vector<Product> products)
//{
//	char letters[35] = "abcdefghijklmnopqrstuvwxyz";
//	for (int i = 0; i < numberProducts; ++i)
//	{
//		std::string name;
//		name.append(1, letters[i % 32]);
//		name += std::to_string(rand() % 10);
//
//		float price = rand() % 1000;
//		int quantity = rand() % 1000;
//		Product product(name, price, quantity);
//		products.push_back(product);
//	}
//}

//void generateSales(int numberSales, Inventory* inventory, std::vector<Sale*> sales)
//{
//	char letters[35] = "abcdefghijklmnopqrstuvwxyz";
//	for (int i = 0; i < numberSales; ++i)
//	{
//		std::string name = "sale_" + letters[i % 32]+ std::to_string(rand() % 10);
//		Sale* sale = new Sale(name, inventory);
//		int numberProductsSale = rand() % 20;
//		
//		for (int j = 0; j < numberProductsSale; ++j)
//		{
//			Product* product = inventory->getProduct(j % inventory->getTotalNoProductTypes());
//			int quantity = rand() % 1000;
//			sale->addProductToSale(product, quantity);
//		}
//		sales.push_back(sale);
//	}
//}

int main()
{
	int const static NUMBER_THREADS = 2;
	int const static NUMBER_PRODUCTS = 10;

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
	/*Product* p1 = new Product{ "milk", 5, 100 };
	Product* p2 = new Product{ "apple", 2, 10 };
	Product* p3 = new Product{ "bread", 5, 505 };
	Product* p4 = new Product{ "honey", 30, 400 };
	Product* p5 = new Product{ "cheese", 37, 200 };*/

	for (int i=0; i<allAvailableProducts.size(); ++i)
	{
		std::cout << allAvailableProducts[i]->getName() << " " << allAvailableProducts[i]->getAvailableQuantity() << std::endl;
	}
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
			int quantity = rand() % 10;
			sale->addProductToSale(product, quantity);
		}
		sales.push_back(sale);
	}
	//allAvailableProducts.push_back(p1);
	//allAvailableProducts.push_back(p2);
	//allAvailableProducts.push_back(p3);
	//allAvailableProducts.push_back(p4);
	//allAvailableProducts.push_back(p5);
	//Sale* sale1 = new Sale("s1", inventory);
	//sale1->addProductToSale(p1, 50);
	//sale1->addProductToSale(p2, 5);
	//Sale* sale2 = new Sale("s2", inventory);
	//sale2->addProductToSale(p1, 40);
	//Sale* sale3 = new Sale("s3", inventory);
	//sale3->addProductToSale(p1, 20);
	//sale3->addProductToSale(p4, 401);
	//sales.push_back(sale1);
	//sales.push_back(sale2);
	//sales.push_back(sale3);

	std::cout << "\nSales\n";
	for (auto sale : sales)
	{
		std::cout << sale->getName() << "\n";
		for (auto product: sale->getProductsForSale())
		{
			std::cout << product.first->getName() << " " << product.second << std::endl;
		}
	}
	 
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
	std::cout << "Time: " << (end - start).count() << "\n\nAfter sales\n";

	for (int i = 0; i < allAvailableProducts.size(); ++i)
	{
		std::cout << allAvailableProducts[i]->getName() << " " << allAvailableProducts[i]->getAvailableQuantity() << std::endl;
	}
	for (auto b : inventory->getBills())
	{
		std::cout << b->getTotalPriceOfBill() << " " << b->getNoProducts() << std::endl;
	}
	return 0;
}