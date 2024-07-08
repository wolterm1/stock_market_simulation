#include <gtest/gtest.h>

#include "db_connector.hpp"

using namespace ProjectStockMarket;
class DBConnectorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    std::filesystem::remove("test.db");
    db = new DBConnector("test.db");
  }

  void TearDown() override {
    delete db;
    std::filesystem::remove("test.db");
  }

  DBConnector* db;
};

TEST_F(DBConnectorTest, PreventDuplicateMarketEntries) {
  std::string productName = "TestProduct";
  int initialAmount = 10;
  int initialPrice = 100;

  // Add product to market
  db->addProductToMarket(productName, initialAmount, initialPrice);

  // Check if the product was added with the correct amount
  SQLite::Statement checkMarket(db->getDatabase(),
                                "SELECT amount FROM Market WHERE product_id = ?;");
  int product_id = custom_hash(productName);
  checkMarket.bind(1, product_id);
  ASSERT_TRUE(checkMarket.executeStep());
  int amount = checkMarket.getColumn(0).getInt();
  EXPECT_EQ(amount, initialAmount);

  // Add the same product again
  db->addProductToMarket(productName, initialAmount, initialPrice);

  // Check if the amount has been updated correctly
  checkMarket.reset();
  checkMarket.bind(1, product_id);
  ASSERT_TRUE(checkMarket.executeStep());
  amount = checkMarket.getColumn(0).getInt();
  EXPECT_EQ(amount, initialAmount * 2);  // Amount should be doubled
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
