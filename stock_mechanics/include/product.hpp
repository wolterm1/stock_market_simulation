#pragma once
#include <string>

namespace ProjectStockMarket {

class Product {
 public:
  Product(int i, std::string n);

  int id;
  std::string name;
  int getPrice();
};

}  // namespace ProjectStockMarket
