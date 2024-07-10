#pragma once
#include <string>
#include <vector>

#include "record.hpp"

namespace ProjectStockMarket {

class Product {
 public:
  Product(int id, std::string name) : m_id(id), m_name(name) {}

  std::string getName() const;
  std::vector<Record> getAllRecords() const;
  std::vector<Record> getRecords(time_point from, time_point to) const;
  void addRecord(Record record);
  void reduceRecordCountToX(int limit);
  int getCurrentPrice() const;
  int getId() const;

  bool operator==(const Product& other) const;

 private:
  int m_id;
  std::string m_name;
  int m_starting_price;  // only used at the beginning wont be used further
};

}  // namespace ProjectStockMarket
