#include "market_place.hpp"

#include <ctime>
#include <mutex>
#include <random>
#include <thread>

#include "db_connector.hpp"

using namespace ProjectStockMarket;

std::vector<ProductEntry> MarketPlace::getInventory() {
  return DBConnector::getAllProductEntriesFromMarket();
}
std::vector<Product> MarketPlace::getAllProduct() {
  return DBConnector::getAllProducts();
}
void MarketPlace::addProductEntry(std::string p_name, int p_price, int p_count) {
  Product p(p_name, p_price);
  ProductEntry entry(p, p_count);
  DBConnector::addProductEntryToMarket(entry);
}
void MarketPlace::addProductEntry(ProductEntry p_entry) {
  DBConnector::addProductEntryToMarket(p_entry);
}

MarketPlace::MarketPlace(int limit_record_entries) : m_limit_record_entries(limit_record_entries) {
  timer.setCallback([this]() { updateProductPrices(); });
}
MarketPlace::~MarketPlace() {
  timer.stop();
}

void MarketPlace::startPriceUpdate() {
  timer.start();
}
int MarketPlace::getCurrentTime() {
  return timer.getCurrentTime();
}

void MarketPlace::updateProductPrices() {
  std::vector<ProductEntry> all_product_entries = DBConnector::getAllProductEntriesFromMarket();

  for (auto& entry : all_product_entries) {
    std::vector<Record> records;
    entry.product.reduceRecordCountToX(m_limit_record_entries);
    records = entry.product.getAllRecords();
    if (records.empty()) {
      continue;
    }

    // Calculate the weighted average price based on the recency of records
    int current_time = getCurrentTime();  // Assuming there's a function to get the current time
    double total_weight = 0;
    double weighted_sum = 0;

    for (const Record& record : records) {
      double time_diff = current_time - record.dateTime;
      if (time_diff == -1) time_diff = -2;
      double weight = 1.0 / (1.0 + time_diff);  // More recent records have higher weight
      total_weight += weight;
      weighted_sum += record.price * weight;
    }

    double new_price = weighted_sum / total_weight;
    while (true) {
      try {
        DBConnector::setProductCurrentPrice(entry.product, new_price);
        break;
      } catch (const std::exception& e) {
        // Log the exception and retry
      }
    }
  }
}
