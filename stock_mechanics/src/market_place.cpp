#include "market_place.hpp"

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
