#pragma once
#include <string>

#include "account.hpp"
#include "product_entry.hpp"
namespace ProjectStockMarket {

class User {
 public:
  User();
  User(Account a, std::string name, int b);
  User(Account a, std::string name, int b, std::vector<ProductEntry> inv);

  void buyProduct(ProductEntry p_entry);
  void sellProduct(ProductEntry p_entry);
  void increaseBalance(int amount);
  int getId();
  std::string getName();
  int getBalance();
  std::vector<ProductEntry> getInventory();
  Account getAccount();

 private:
  Account m_account;
  std::string m_name;
  int m_balance;
  std::vector<ProductEntry> m_inventory;
};

}  // namespace ProjectStockMarket
