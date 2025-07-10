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
      addProduct("Video Game", 100).addRecord(Record(now, 100));
      addProduct("Job Application", 100).addRecord(Record(now, 100));
      addProduct("Baloney", 100).addRecord(Record(now, 100));
      addProduct("Toothbrush", 100).addRecord(Record(now, 100));
      addProduct("Trump Military Action", 100).addRecord(Record(now, 100));
      addProduct("Massi", 100).addRecord(Record(now, 100));
      addProduct("Aktien", 100).addRecord(Record(now, 100));
      addProduct("Rolex", 100).addRecord(Record(now, 100));
      addProduct("Gold", 100).addRecord(Record(now, 100));
      addProduct("Apple", 100).addRecord(Record(now, 100));
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
    double trend = 0.3 * (2.0 * ((double) std::rand() / RAND_MAX) - 1.0);
    Record record(now, randomWalk(current_price, trend, 0.8, 1));
    product.addRecord(record);
  }
}

int MarketPlace::randomWalk(int current_price, double trend, double streuung, double dt) {
  double sqdt = std::sqrt(dt);
  std::mt19937 generator(std::random_device{}());
  double Y = trendDistribution(generator);
  return current_price * (1 + trend * dt + streuung * sqdt * Y)+ randomNumber(generator);
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
