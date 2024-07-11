#include <gtest/gtest.h>

#include <authenticator.hpp>
#include <db_connector.hpp>
#include <market_place.hpp>
#include <string>

using namespace std::chrono_literals;

namespace sm = ProjectStockMarket;

TEST(TestDatabase, ProductsAuthBuySell) {
  sm::DBConnector::initDB(":memory:");

  sm::MarketPlace mp(3600, false);

  auto apple = mp.addProduct("Apple", 100);
  auto banana = mp.addProduct("Banana", 200);
  auto orange = mp.addProduct("Orange", 300);
  auto grape = mp.addProduct("Grape", 400);
  auto pineapple = mp.addProduct("Pineapple", 1);

  std::vector<sm::Product> addedProducts = {apple, banana, orange, grape,
                                            pineapple};

  std::vector<sm::Product> products = mp.getAllProducts();

  for (auto product : addedProducts) {
    bool found = false;
    for (auto p : products) {
      if (p.getName() == product.getName()) {
        found = true;
        break;
      }
    }
    ASSERT_TRUE(found) << "Product " << product.getName()
                       << " not found in the database";
  }

  apple.addRecord(sm::Record(std::chrono::system_clock::now(), 5000));
  ASSERT_EQ(apple.getCurrentPrice(), 5000) << "Apple price not updated";
  // cheap banana
  banana.addRecord(sm::Record(std::chrono::system_clock::now(), 1));
  pineapple.addRecord(sm::Record(std::chrono::system_clock::now(), 1));

  std::string token =
      sm::Authenticator::registerAccount("admin", "admin", "Admin");
  sm::User user = sm::Authenticator::findUserByToken(token);
  ASSERT_EQ(user.getName(), "Admin") << "User not found by token";

  ASSERT_GE(user.getBalance(), 10) << "User balance is really low";

  user.buyProduct(banana, 2);
  ASSERT_EQ(user.getBalance(), 998)
      << "User balance not updated after purchase";

  bool found = false;
  std::vector<sm::ProductEntry> inventory = user.getInventory();
  for (auto entry : inventory) {
    if (entry.product.getName() == banana.getName()) {
      ASSERT_EQ(entry.count, 2) << "Product count not updated after purchase";
      found = true;
      break;
    }
  }

  ASSERT_TRUE(found) << "Product not found in user inventory";

  std::vector<sm::ProductEntry> marketInventory = mp.getInventory();
  for (auto entry : marketInventory) {
    if (entry.product.getName() == banana.getName()) {
      ASSERT_EQ(entry.count, 198)
          << "Market inventory not updated after purchase";
      break;
    }
  }

  user.sellProduct(banana, 1);
  ASSERT_EQ(user.getBalance(), 999) << "User balance not updated after selling";

  inventory = user.getInventory();
  for (auto entry : inventory) {
    if (entry.product.getName() == banana.getName()) {
      ASSERT_EQ(entry.count, 1) << "Product count not updated after selling";
    }
  }

  marketInventory = mp.getInventory();
  for (auto entry : marketInventory) {
    if (entry.product.getName() == banana.getName()) {
      ASSERT_EQ(entry.count, 199)
          << "Market inventory not updated after selling";
    }
  }

  sm::Authenticator::logout(token);
  ASSERT_THROW(sm::Authenticator::findUserByToken(token), std::exception)
      << "User not logged out, Token still exists";

  sm::Account account("admin", "admin");
  token = sm::Authenticator::login(account);
  sm::User user3 = sm::Authenticator::findUserByToken(token);
  ASSERT_EQ(user3.getName(), "Admin")
      << "User not found by token after logging in";

  auto initial_inventory = user.getInventory();
  auto initial_market = mp.getInventory();

  ASSERT_THROW(user3.buyProduct(apple, 10), std::exception)
      << "User was able to buy product without enough balance";

  ASSERT_EQ(initial_inventory, user3.getInventory())
      << "User inventory changed after failed purchase (not enough balance)";
  ASSERT_EQ(initial_market, mp.getInventory())
      << "Market inventory changed after failed purchase (not enough balance)";

  ASSERT_THROW(user3.sellProduct(apple, 10), std::exception)
      << "User was able to sell product without having it in inventory";

  ASSERT_EQ(initial_inventory, user3.getInventory())
      << "User inventory changed after failed sell (not in inventory)";
  ASSERT_EQ(initial_market, mp.getInventory())
      << "Market inventory changed after failed sell (not in inventory)";

  ASSERT_THROW(user3.buyProduct(pineapple, 10), std::exception)
      << "User was able to buy product that is not in stock (enough)";

  ASSERT_EQ(initial_inventory, user3.getInventory())
      << "User inventory changed after failed purchase (not enough stock)";
  ASSERT_EQ(initial_market, mp.getInventory())
      << "Market inventory changed after failed purchase (not enough stock)";
}

TEST(TestDatabase, Records) {
  sm::DBConnector::initDB(":memory:");

  sm::MarketPlace mp(3600, false);

  auto apple = mp.addProduct("Apple", 100);
  sm::time_point now = std::chrono::system_clock::now();
  auto record1 = sm::Record(now, 5000);
  auto record2 = sm::Record(now + 1s, 6000);
  auto record3 = sm::Record(now + 2s, 7000);
  auto record4 = sm::Record(now + 3s, 8000);
  apple.addRecord(record1);
  ASSERT_EQ(apple.getCurrentPrice(), 5000) << "Apple price not updated";
  apple.addRecord(record2);
  ASSERT_EQ(apple.getCurrentPrice(), 6000) << "Apple price not updated";
  apple.addRecord(record3);
  ASSERT_EQ(apple.getCurrentPrice(), 7000) << "Apple price not updated";
  apple.addRecord(record4);
  ASSERT_EQ(apple.getCurrentPrice(), 8000) << "Apple price not updated";

  auto records = apple.getAllRecords();
  ASSERT_EQ(records.size(), 4) << "Records not added to the database";
  ASSERT_EQ(records[0].price, record1.price)
      << "Record 1 not added to the database";
  ASSERT_EQ(records[1].price, record2.price)
      << "Record 2 not added to the database";
  ASSERT_EQ(records[2].price, record3.price)
      << "Record 3 not added to the database";
  ASSERT_EQ(records[3].price, record4.price)
      << "Record 4 not added to the database";

  auto filtered_records = apple.getRecords(now + 1s, now + 3s);
  ASSERT_EQ(filtered_records.size(), 3) << "Records not filtered by time range";
  ASSERT_EQ(filtered_records[0].price, record2.price)
      << "Record 2 not filtered by time range";
  ASSERT_EQ(filtered_records[1].price, record3.price)
      << "Record 3 not filtered by time range";
  ASSERT_EQ(filtered_records[2].price, record4.price)
      << "Record 4 not filtered by time range";
}

TEST(TestDatabase, loginWhileGenerating) {
  sm::DBConnector::initDB(":memory:");
  sm::MarketPlace mp(3600, true);
  sm::Authenticator::registerAccount("admin", "admin", "Admin");
}