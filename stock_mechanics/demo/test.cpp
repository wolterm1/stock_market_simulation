#include <gtest/gtest.h>

#include "account.hpp"
#include "db_connector.hpp"
#include "user.hpp"

class StockMarketTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  void add_100_different_accounts() {
    for (int i = 0; i < 100; i++) {
      ProjectStockMarket::Account a(std::to_string(i), "password");
      ProjectStockMarket::User user(a, "dummy_user", 100);
      try {
        ProjectStockMarket::DBConnector::addUser(user);
      } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
      }
    }
  }

  void add_a_ball_with_price_30() {
    ProjectStockMarket::Product p("ball", 30);
    try {
      ProjectStockMarket::DBConnector::addProductToMarket({p, 1});
    } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

  void add_100_records_with_increasing_time_to_ball() {
    ProjectStockMarket::Product p("ball");

    for (int i = 0; i < 100; i++) {
      ProjectStockMarket::Record ent(i, 30);
      try {
        ProjectStockMarket::DBConnector::addRecord(p, ent);
      } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
      }
    }
  }

  void read_all_records_from_oldest_to_newest_from_ball() {
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

  void remove_all_records_until_the_newest_5_from_ball() {
    ProjectStockMarket::Product p("ball");
    try {
      ProjectStockMarket::DBConnector::keepLatestXRecords(5, p);
    } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }
};

TEST_F(StockMarketTest, Add100DifferentAccounts) {
  add_100_different_accounts();
  // Here you can add assertions to verify if 100 accounts have been added.
}

TEST_F(StockMarketTest, AddABallWithPrice30) {
  add_a_ball_with_price_30();
  // Here you can add assertions to verify if the ball product has been added with price 30.
}

TEST_F(StockMarketTest, Add100RecordsWithIncreasingTimeToBall) {
  add_100_records_with_increasing_time_to_ball();
  // Here you can add assertions to verify if 100 records have been added.
}

TEST_F(StockMarketTest, ReadAllRecordsFromOldestToNewestFromBall) {
  read_all_records_from_oldest_to_newest_from_ball();
  // Here you can add assertions to verify if records are read correctly.
}

TEST_F(StockMarketTest, RemoveAllRecordsUntilTheNewest5FromBall) {
  remove_all_records_until_the_newest_5_from_ball();
  // Here you can add assertions to verify if only the newest 5 records remain.
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
