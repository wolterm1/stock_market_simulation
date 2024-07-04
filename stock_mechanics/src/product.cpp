#include "product.hpp"

#include "db_connector.hpp"

using namespace ProjectStockMarket;

Product::Product(std::string n, int p) : m_name(n), m_starting_price(p) {}
Product::Product(std::string n) : m_name(n), m_starting_price(-1) {}
int Product::getPrice() {
  return DBConnector::getProductPrice(*this);
}

int Product::getId() {
  return DBConnector::getProductId(*this);
}

int Product::getStartingPrice() {
  return m_starting_price;
}

std::string Product::getName() {
  return m_name;
}

std::vector<Record> Product::getAllRecords() {
  return DBConnector::getRecordsFromOldestToNewest(*this);
}
void Product::addRecord(Record record) {
  DBConnector::addRecord(*this, record);
}
void Product::reduceRecordCountToX(int limit) {
  DBConnector::keepLatestXRecords(*this, limit);
}
