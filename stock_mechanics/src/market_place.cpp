#include "market_place.hpp"

#include <ctime>
#include <random>

#include "db_connector.hpp"

using namespace ProjectStockMarket;

std::vector<ProductEntry> MarketPlace::getInventory() {
  return DBConnector::getAllProductEntriesFromMarket();
}
std::vector<Product> MarketPlace::getAllProduct() {
  return DBConnector::getAllProducts();
}

MarketPlace::MarketPlace(int limit_record_entries) : m_limit_record_entries(limit_record_entries) {}

void MarketPlace::updateProductPrices() {}

std::string generateRandomToken(size_t p_size) {
  std::srand(std::time(nullptr));
  static const char num[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  std::string tmp;
  for (int i = 0; i < p_size; i++) {
    tmp = tmp + num[std::rand() % sizeof(num) - 1];
  }
  return tmp;
}
