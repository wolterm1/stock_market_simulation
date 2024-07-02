#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>

#include <string>

#include "account.hpp"
#include "product_entry.hpp"
#include "user.hpp"

namespace ProjectStockMarket {

class DBConnector {
 public:
  DBConnector(const std::string& dbName);
  ~DBConnector() = default;  // SQLiteCpp manages the database connection automatically

  void createTables();

  void addAccount(Account a, const std::string& name, int balance);
  void addProductToMarket(const std::string& product_name, int amount, int price);
  void addProductToInventory(int user_id, int product_id, int amount);

  std::vector<ProductEntry> getInventory(int user_id);
  int getProductPrice(int product_id);
  std::vector<ProductEntry> getAllMarketProducts();
  User getUserFromAccount(const std::string& username, const std::string& password);

 private:
  SQLite::Database m_database;
};

}  // namespace ProjectStockMarket
