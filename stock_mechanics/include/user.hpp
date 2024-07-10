#pragma once
#include <string>

#include "account.hpp"
#include "product_entry.hpp"
namespace ProjectStockMarket {

class User {
 public:
  User(int id, std::string name, int balance)
      : m_id(id), m_name(name), m_balance(balance) {}

  int getId() const;
  std::string getName() const;
  int getBalance() const;
  std::vector<ProductEntry> getInventory();
  void buyProduct(const Product& p_product, int p_amount);
  void sellProduct(const Product& p_product, int p_amount);

 private:
  int m_id;
  std::string m_name;
  int m_balance;
};

}  // namespace ProjectStockMarket
