#include "Hash_Table.h"

#include <iostream>
#include <string>

int main()
{
	HashTable<int, std::string> a(5);
	a.Add(19,"Anna");
	a.Add(18, "Lilit");
	a.Add(20, "Hayk");
	a.Add(23, "Lilit");
	a.Add(72, "Vazgen");
	a.Add(7, "Petros");
	a.Add(24, "Tatev");
	a.Add(56, "Lusine");
	a.Add(43, "Nare");

	std::cout << "Table:" << std::endl;
	a.print_table();
	a.Remove("Karine");
	a.Remove("Vazgen");

	std::cout << "Table after removing elements:" << std::endl;
	a.print_table();

	std::cout <<"Number of items in key Anna are: "<<a.NumberOfItemsInKey("Anna") << std::endl;;
	std::cout<<"Nare - " << a["Nare"] << std::endl;
	std::cout <<"Lilit - "<< a["Lilit"] << std::endl;
	std::cout <<"Lusine - "<< a["Lusine"] << std::endl;

	HashTable<int, std::string> b;
	b = a;

	std::cout << std::endl;
	std::cout << "assignment operator used:" << std::endl;
	b.print_table();

	b.clear();
	std::cout << "cleared table: " << std::endl;
	b.print_table();
	std::cout << b.empty() << std::endl;
}
