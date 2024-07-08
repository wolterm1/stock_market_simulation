#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time(0)
#include <iostream>

#include "account.hpp"
#include "db_connector.hpp"  // Stellen Sie sicher, dass dies Ihre Header-Datei ist
#include "user.hpp"

void TEST_ADDING_ACCOUNTS() {
  std::cout << "\nTEST: TEST_ADDING_ACCOUNTS" << std::endl;
  for (int i = 0; i < 10; i++) {
    ProjectStockMarket::Account a(std::to_string(i), "password");
    ProjectStockMarket::User user(a, "dummy_user", 100);
    ProjectStockMarket::DBConnector::addUser(user);
  }
}

void TEST_ADDING_PRODUCTENTRIES_TO_MARKET() {
  std::cout << "\nTEST: TEST_ADDING_PRODUCTENTRIES_TO_MARKET" << std::endl;
  ProjectStockMarket::Product p("ball", 30);
  ProjectStockMarket::DBConnector::addProductEntryToMarket({p, 1});
}

void TEST_ADDING_RECORD_TO_PRODUCT() {
  std::cout << "\nTEST: TEST_ADDING_RECORD_TO_PRODUCT" << std::endl;
  ProjectStockMarket::Product p("ball");

  for (int i = 0; i < 10; i++) {
    // Initialize random seed
    srand(time(0));
    // Generate a random number between 10 and 50
    int randomNumber = rand() % 41 + 10;  // rand() % (max - min + 1) + min
    ProjectStockMarket::Record ent(i, randomNumber);
    ProjectStockMarket::DBConnector::addRecord(p, ent);
  }
  std::cout << "CURRENT BALL PRICE: " << p.getPrice() << std::endl;
}

void TEST_READING_ALL_RECORDS() {
  std::cout << "\nTEST: TEST_READING_ALL_RECORDS" << std::endl;
  ProjectStockMarket::Product p("ball");
  std::vector<ProjectStockMarket::Record> records =
      ProjectStockMarket::DBConnector::getRecordsFromOldestToNewest(p);
}

void TEST_REDUCING_RECORDS() {
  std::cout << "\nTEST: TEST_REDUCING_RECORDS" << std::endl;
  ProjectStockMarket::Product p("ball");
  ProjectStockMarket::DBConnector::keepLatestXRecords(p, 5);
}

void TEST_GET_LATEST_PRICE_FROM_PRODUCT() {
  std::cout << "\nTEST: TEST_GET_LATEST_PRICE_FROM_PRODUCT" << std::endl;
  ProjectStockMarket::Product p("ball", 30);
  ProjectStockMarket::Record r(100, 1000);
  ProjectStockMarket::DBConnector::addRecord(p, r);
}

void TEST_UPDATING_USER(ProjectStockMarket::User user) {
  std::cout << "\nTEST: TEST_UPDATING_USER" << std::endl;
  ProjectStockMarket::DBConnector::updateUser(user, user.getName(), 999);
}

void TEST_FINDING_USER_AND_UPDATING_HIM() {
  std::cout << "\nTEST: TEST_FINDING_USER_AND_UPDATING_HIM" << std::endl;
  ProjectStockMarket::Account a("0", "password");
  ProjectStockMarket::User user = ProjectStockMarket::DBConnector::findUser(a);
  int id = user.getId();
  TEST_UPDATING_USER(user);
}

void TEST_REMOVING_AND_GETTING_ALL_PRODUCTENTRIES_FROM_MARKET() {
  std::cout << "\nTEST: TEST_REMOVING_AND_GETTING_ALL_PRODUCTENTRIES_FROM_MARKET" << std::endl;
  std::vector<ProjectStockMarket::ProductEntry> everyEntry =
      ProjectStockMarket::DBConnector::getAllProductEntriesFromMarket();
  for (auto e : everyEntry) {
    ProjectStockMarket::DBConnector::removeProductEntryFromMarket(e);
  }
}

void TEST_ADD_PRODUCTENTRIES_TO_INVENTORY() {
  std::cout << "\nTEST: TEST_ADD_PRODUCTENTRIES_TO_INVENTORY" << std::endl;
  ProjectStockMarket::Account a("0", "password");
  ProjectStockMarket::User user(a, "dummy_user", 100);
  ProjectStockMarket::Product ball("ball", 30);
  ProjectStockMarket::Product table("table", 30);
  ProjectStockMarket::ProductEntry addEntryBall(ball, 100);
  ProjectStockMarket::ProductEntry addEntryTable(table, 100);
  ProjectStockMarket::DBConnector::addProductEntryToMarket(addEntryBall);
  ProjectStockMarket::DBConnector::addProductEntryToMarket(addEntryTable);

  ProjectStockMarket::ProductEntry getEntryBall(ball, 10);
  ProjectStockMarket::ProductEntry getEntryTable(table, 10);
  ProjectStockMarket::DBConnector::removeProductEntryFromMarket(getEntryBall);
  ProjectStockMarket::DBConnector::removeProductEntryFromMarket(getEntryTable);
  ProjectStockMarket::DBConnector::addProductEntryToInventory(getEntryBall, user);
  ProjectStockMarket::DBConnector::addProductEntryToInventory(getEntryTable, user);
}

void TEST_GET_ALL_PRODUCTENTRIES_FROM_INVENTORY() {
  std::cout << "\nTEST: TEST_GET_ALL_PRODUCTENTRIES_FROM_INVENTORY" << std::endl;
  ProjectStockMarket::Account a("0", "password");
  ProjectStockMarket::User user(a, "dummy_user", 100);
  std::vector<ProjectStockMarket::ProductEntry> entries =
      ProjectStockMarket::DBConnector::getAllProductEntriesFromInventory(user);
}

void TEST_REMOVE_ALL_PRODUCTENTRIES_FROM_INVENTORY() {
  std::cout << "\nTEST: TEST_REMOVE_ALL_PRODUCTENTRIES_FROM_INVENTORY" << std::endl;
  ProjectStockMarket::Account a("0", "password");
  ProjectStockMarket::User user(a, "dummy_user", 100);
  std::vector<ProjectStockMarket::ProductEntry> entries =
      ProjectStockMarket::DBConnector::getAllProductEntriesFromInventory(user);
  for (auto e : entries) ProjectStockMarket::DBConnector::removeProductEntryFromInventory(e, user);
}

void TEST_GETTING_ALL_PRODUCTS() {
  std::cout << "\nTEST: TEST_GETTING_ALL_PRODUCTS" << std::endl;
  std::vector<ProjectStockMarket::Product> products =
      ProjectStockMarket::DBConnector::getAllProducts();
}

#include "market_place.hpp"

int main() {
  ProjectStockMarket::MarketPlace market(1000);
  market.startPriceUpdate();

  try {
    TEST_ADDING_ACCOUNTS();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_ADDING_PRODUCTENTRIES_TO_MARKET();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_ADDING_RECORD_TO_PRODUCT();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_READING_ALL_RECORDS();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_REDUCING_RECORDS();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_GET_LATEST_PRICE_FROM_PRODUCT();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_FINDING_USER_AND_UPDATING_HIM();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_REMOVING_AND_GETTING_ALL_PRODUCTENTRIES_FROM_MARKET();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_ADD_PRODUCTENTRIES_TO_INVENTORY();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_GET_ALL_PRODUCTENTRIES_FROM_INVENTORY();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_REMOVE_ALL_PRODUCTENTRIES_FROM_INVENTORY();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try {
    TEST_GETTING_ALL_PRODUCTS();
    std::cout << "-> PASSED!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "-> FAILED!" << std::endl;
    std::cout << e.what() << std::endl;
  }
  while (true) {
    TEST_ADDING_RECORD_TO_PRODUCT();
  }

  return 0;
}
