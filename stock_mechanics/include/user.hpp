#pragma once
#include <string>

#include "product_container.hpp"
namespace ProjectStockMarket {

class User {
 public:
  User();
  User(int id, std::string name, int b);
  User(int id, std::string name, int b, ProductContainer inv);

  bool buyProduct(int product_id, int count);
  bool buyProduct(Product product, int count);
  bool sellProduct(int product_id, int count);
  bool sellProduct(Product product, int count);
  int getId();
  std::string getName();
  int getBalance();
  std::vector<ProductEntry> getInventory();

 private:
  int m_id;
  std::string m_name;
  int m_balance;
  ProductContainer m_inventory;
};

}  // namespace ProjectStockMarket
