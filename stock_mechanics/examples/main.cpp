#include <iostream>

#include "account.hpp"
#include "db_connector.hpp"  // Stellen Sie sicher, dass dies Ihre Header-Datei ist
#include "user.hpp"

void TEST_ADDING_ACCOUNTS() {
  std::cout << "\nTEST: TEST_ADDING_ACCOUNTS" << std::endl;
  for (int i = 0; i < 10; i++) {
    ProjectStockMarket::Account a(std::to_string(i), "password");
    ProjectStockMarket::User user(a, "dummy_user", 100);
    try {
      ProjectStockMarket::DBConnector::addUser(user);
    } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }
  std::cout << "added 10 accounts" << std::endl;
}

void TEST_ADDING_PRODUCTENTRIES_TO_MARKET() {
  std::cout << "\nTEST: TEST_ADDING_PRODUCT_TO_MARKET" << std::endl;
  ProjectStockMarket::Product p("ball", 30);
  try {
    ProjectStockMarket::DBConnector::addProductEntryToMarket({p, 1});
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  std::cout << "added ball 1 time to market" << std::endl;
}

void TEST_ADDING_RECORD_TO_PRODUCT() {
  std::cout << "\nTEST: TEST_ADDING_RECORD_TO_PRODUCT" << std::endl;
  ProjectStockMarket::Product p("ball");

  for (int i = 0; i < 10; i++) {
    ProjectStockMarket::Record ent(i, 30);
    try {
      ProjectStockMarket::DBConnector::addRecord(p, ent);
    } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }
  std::cout << "added 10 records to ball" << std::endl;
}

void TEST_READING_ALL_RECORDS() {
  std::cout << "\nTEST: TEST_READING_ALL_RECORDS" << std::endl;
  ProjectStockMarket::Product p("ball");
  try {
    std::vector<ProjectStockMarket::Record> records =
        ProjectStockMarket::DBConnector::getRecordsFromOldestToNewest(p);
    for (auto e : records) {
      std::cout << "Time:" << e.dateTime << ", " << "price: " << e.price << std::endl;
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
}

void TEST_REDUCING_RECORDS() {
  std::cout << "\nTEST: TEST_REDUCING_RECORDS" << std::endl;
  ProjectStockMarket::Product p("ball");
  try {
    ProjectStockMarket::DBConnector::keepLatestXRecords(p, 5);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  std::cout << "reduce records to only 5 for ball" << std::endl;
}

void TEST_GET_LATEST_PRICE_FROM_PRODUCT() {
  std::cout << "\nTEST: TEST_GET_LATEST_PRICE_FROM_PRODUCT" << std::endl;
  ProjectStockMarket::Product p("ball", 30);
  ProjectStockMarket::Record r(100, 1000);
  ProjectStockMarket::DBConnector::addRecord(p, r);
  std::cout << "pice should be 1000" << std::endl;
  std::cout << "PRICE:" << ProjectStockMarket::DBConnector::getLatestRecordPrice(p) << std::endl;
}

void TEST_UPDATING_USER(ProjectStockMarket::User user) {
  std::cout << "\nTEST: TEST_UPDATING_USER" << std::endl;
  ProjectStockMarket::User newUser(user.getAccount(), user.getName(), 999);
  ProjectStockMarket::DBConnector::updateUser(user, newUser);
  std::cout << "Updated dummy_user with id 1 balance to 999" << std::endl;
}

void TEST_FINDING_USER_AND_UPDATING_HIM() {
  std::cout << "\nTEST: TEST_FINDING_USER" << std::endl;
  ProjectStockMarket::Account a("0", "password");
  ProjectStockMarket::User user = ProjectStockMarket::DBConnector::findUser(a);
  int id = user.getId();
  std::cout << "found user: " << user.getName() << " with id " << id << std::endl;
  TEST_UPDATING_USER(user);
}

void TEST_REMOVING_AND_GETTING_ALL_PRODUCTENTRIES_FROM_MARKET() {
  std::cout << "\nTEST: TEST_REMOVING_AND_GETTING_ALL_PRODUCTS_FROM_MARKET" << std::endl;
  std::vector<ProjectStockMarket::ProductEntry> everyEntry =
      ProjectStockMarket::DBConnector::getAllProductEntriesFromMarket();
  for (auto e : everyEntry) {
    ProjectStockMarket::DBConnector::removeProductEntryFromMarket(e);
    std::cout << "removed " << e.count << " times product " << e.product.getName() << "with id "
              << e.product.getId() << std::endl;
  }
  std::cout << "every product removed from the market (all 0 count)" << std::endl;
}

void TEST_ADD_PRODUCTENTRIES_TO_INVENTORY() {
  std::cout << "\nTEST: TEST_ADD_PRODUCTS_TO_INVENTORY" << std::endl;
  ProjectStockMarket::Account a("0", "password");
  ProjectStockMarket::User user(a, "dummy_user", 100);
  ProjectStockMarket::Product ball("ball", 30);
  ProjectStockMarket::Product table("table", 30);
  ProjectStockMarket::ProductEntry addEntryBall(ball, 100);
  ProjectStockMarket::ProductEntry addEntryTable(table, 100);
  ProjectStockMarket::DBConnector::addProductEntryToMarket(addEntryBall);
  ProjectStockMarket::DBConnector::addProductEntryToMarket(addEntryTable);

  std::cout << "added 100 ballls to market" << std::endl;
  ProjectStockMarket::ProductEntry getEntryBall(ball, 10);
  ProjectStockMarket::ProductEntry getEntryTable(table, 10);
  ProjectStockMarket::DBConnector::removeProductEntryFromMarket(getEntryBall);
  ProjectStockMarket::DBConnector::removeProductEntryFromMarket(getEntryTable);
  std::cout << "removed 10 ballls from market" << std::endl;
  ProjectStockMarket::DBConnector::addProductEntryToInventory(getEntryBall, user);
  ProjectStockMarket::DBConnector::addProductEntryToInventory(getEntryTable, user);
  std::cout << "added 10 ballls to dummy_user with id 1" << std::endl;
}

void TEST_GET_ALL_PRODUCTENTRIES_FROM_INVENTORY() {
  std::cout << "\nTEST: TEST_GET_ALL_PRODUCTS_FROM_INVENTORY" << std::endl;
  ProjectStockMarket::Account a("0", "password");
  ProjectStockMarket::User user(a, "dummy_user", 100);
  std::vector<ProjectStockMarket::ProductEntry> entries =
      ProjectStockMarket::DBConnector::getAllProductEntriesFromInventory(user);
  for (auto e : entries)
    std::cout << "Product: " << e.product.getName() << ", count = " << e.count
              << ", price = " << e.product.getPrice() << std::endl;
}

void TEST_REMOVE_ALL_PRODUCTENTRIES_FROM_INVENTORY() {
  std::cout << "\nTEST: TEST_REMOVE_ALL_PRODUCTS_FROM_INVENTORY" << std::endl;
  ProjectStockMarket::Account a("0", "password");
  ProjectStockMarket::User user(a, "dummy_user", 100);
  std::vector<ProjectStockMarket::ProductEntry> entries =
      ProjectStockMarket::DBConnector::getAllProductEntriesFromInventory(user);
  for (auto e : entries) ProjectStockMarket::DBConnector::removeProductEntryFromInventory(e, user);
  std::cout << "all entries removed from dummy_user with id 1" << std::endl;
}

void TEST_GETTING_ALL_PRODUCTS() {
  std::cout << "\nTEST: TEST_GETTING_ALL_PRODUCTS" << std::endl;
  std::vector<ProjectStockMarket::Product> products =
      ProjectStockMarket::DBConnector::getAllProducts();
  for (auto p : products)
    std::cout << "Product " << p.getName() << " with id " << p.getId() << std::endl;
}

int main() {
  TEST_ADDING_ACCOUNTS();
  TEST_ADDING_PRODUCTENTRIES_TO_MARKET();
  TEST_ADDING_RECORD_TO_PRODUCT();
  TEST_READING_ALL_RECORDS();
  TEST_REDUCING_RECORDS();
  TEST_GET_LATEST_PRICE_FROM_PRODUCT();
  TEST_FINDING_USER_AND_UPDATING_HIM();
  TEST_REMOVING_AND_GETTING_ALL_PRODUCTENTRIES_FROM_MARKET();
  TEST_ADD_PRODUCTENTRIES_TO_INVENTORY();
  TEST_GET_ALL_PRODUCTENTRIES_FROM_INVENTORY();
  TEST_REMOVE_ALL_PRODUCTENTRIES_FROM_INVENTORY();
  TEST_GETTING_ALL_PRODUCTS();
  return 0;
}
