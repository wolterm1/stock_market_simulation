#include "market_place.hpp"

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

MarketPlace::MarketPlace(int limit_record_entries) : m_limit_record_entries(limit_record_entries) {}

void MarketPlace::updateProductPrices() {}
