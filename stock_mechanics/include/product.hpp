#pragma once
#include <string>

namespace ProjectStockMarket {

class Product {
 public:
  Product(int id, std::string name);

  int id;
  int name;
  void getPrice();
};

}  // namespace ProjectStockMarket
