#include "market_place.hpp"

#include <cmath>
#include <ctime>
#include <mutex>
#include <random>
#include <thread>

#include "db_connector.hpp"

namespace ProjectStockMarket {

MarketPlace::MarketPlace(int limit_record_entries, bool try_generate_products)
    : m_limit_record_entries(limit_record_entries) {
  DBConnector::addPriceRecordLimitTrigger(m_limit_record_entries);
  timer.setCallback([this]() { updateProductPrices(); });

  if (try_generate_products) {
    std::vector<Product> products = DBConnector::getAllProducts();
    if (products.empty()) {
      time_point now = std::chrono::system_clock::now();
      addProduct("Apple", 100).addRecord(Record(now, 100));
      addProduct("Banana", 100).addRecord(Record(now, 100));
      addProduct("Orange", 100).addRecord(Record(now, 100));
      addProduct("Pineapple", 100).addRecord(Record(now, 100));
      addProduct("Mango", 100).addRecord(Record(now, 100));
      addProduct("Peach", 100).addRecord(Record(now, 100));
      addProduct("Strawberry", 100).addRecord(Record(now, 100));
      addProduct("Blueberry", 100).addRecord(Record(now, 100));
      addProduct("Raspberry", 100).addRecord(Record(now, 100));
      addProduct("Blackberry", 100).addRecord(Record(now, 100));
    }
    timer.start();
  }
}

MarketPlace::~MarketPlace() { timer.stop(); }

std::vector<ProductEntry> MarketPlace::getInventory() {
  return DBConnector::getMarketInventory();
}
std::vector<Product> MarketPlace::getAllProducts() {
  return DBConnector::getAllProducts();
}
Product MarketPlace::addProduct(const std::string& p_name, int p_count) {
  return DBConnector::addProduct(p_name, p_count);
}

void MarketPlace::startPriceUpdate() { timer.start(); }

void MarketPlace::updateProductPrices() {
  time_point now = std::chrono::system_clock::now();
  for (auto product : getAllProducts()) {
    int current_price = product.getCurrentPrice();
    // in the future trend should be based on supply and demand
    double trend = trendDistribution(gen);
    Record record(now, randomWalk(current_price, trend, 1, 0.8));
    product.addRecord(record);
  }
}

int MarketPlace::randomWalk(int current_price, double trend, int timeDelta,
                            double standardDeviation) {
  double random_number = randomDistribution(gen);

  double second_part = (trend * timeDelta * current_price);
  double third_part = (standardDeviation * std::sqrt(timeDelta) *
                       random_number * current_price);
  int new_price = std::round(current_price + second_part + third_part);
  return new_price;
}

// void MarketPlace::updateProductPrices() {
//   std::vector<ProductEntry> all_product_entries =
//       DBConnector::getMarketInventory();

//   for (auto& entry : all_product_entries) {
//     std::vector<Record> records;
//     entry.product.reduceRecordCountToX(m_limit_record_entries);
//     records = entry.product.getAllRecords();
//     if (records.empty()) {
//       continue;
//     }

//     // Calculate the weighted average price based on the recency of records
//     int current_time = getCurrentTime();  // Assuming there's a function to
//     get
//                                           // the current time
//     double total_weight = 0;
//     double weighted_sum = 0;

//     for (const Record& record : records) {
//       double time_diff = current_time - record.dateTime;
//       if (time_diff == -1) time_diff = -2;
//       double weight =
//           1.0 / (1.0 + time_diff);  // More recent records have higher weight
//       total_weight += weight;
//       weighted_sum += record.price * weight;
//     }

//     double new_price = weighted_sum / total_weight;
//     while (true) {
//       try {
//         DBConnector::setProductCurrentPrice(entry.product, new_price);
//         break;
//       } catch (const std::exception& e) {
//         // Log the exception and retry
//       }
//     }
//   }
// }

}  // namespace ProjectStockMarket
