#pragma once
#include <string>
#include <vector>

#include "record.hpp"
namespace ProjectStockMarket {

class Product {
 public:
  Product(std::string n, int start_price);
  Product(std::string n);
  Product() {}

  std::string getName();
  std::vector<Record> getAllRecords();
  void addRecord(Record record);
  void reduceRecordCountToX(int limit);
  int getPrice();
  int getStartingPrice();
  int getId();

 private:
  std::string m_name;
  int m_starting_price;  // only used at the beginning wont be used further
};

}  // namespace ProjectStockMarket
