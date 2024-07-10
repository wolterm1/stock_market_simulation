#include "product.hpp"

#include "db_connector.hpp"

using namespace ProjectStockMarket;

int Product::getId() const { return m_id; }

bool ProjectStockMarket::Product::operator==(const Product& other) const {
  return m_id == other.m_id && m_name == other.m_name;
}

std::string Product::getName() const { return m_name; }

int Product::getCurrentPrice() const {
  return DBConnector::getLatestRecordPrice(*this);
}

std::vector<Record> Product::getAllRecords() const {
  return DBConnector::getAllRecords(*this);
}

std::vector<Record> Product::getRecords(time_point from, time_point to) const {
  return DBConnector::getRecords(*this, from, to);
}

void Product::addRecord(Record record) {
  DBConnector::addRecord(*this, record);
}
