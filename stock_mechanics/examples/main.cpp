#include <iostream>

#include "db_connector.hpp"
#include "product_entry.hpp"
#include "user.hpp"

int main() {
  ProjectStockMarket::DBConnector db("test.db");
  ProjectStockMarket::User user = db.getUserFromAccount("jestry", "1234");
  std::cout << "Found Entry: " << user.getName() << " with id: " << user.getId()
            << " and balance of " << user.getBalance() << std::endl;
  db.addProductToMarket("handball", 10, 1000);
  std::vector<ProjectStockMarket::ProductEntry> p = db.getAllMarketProducts();
  for (auto entry : p) {
    std::cout << "Productname: " << entry.product.name << " id: " << entry.product.id
              << " and price of " << db.getProductPrice(entry.product.id);
  }
}
