#pragma once
#include <vector>

#include "record.hpp"

namespace ProjectStockMarket {

class RecordsCache {
 public:
  void refresh(int limit);
  void addRecord(int price);
  std::vector<Record> getRecords();
  RecordsCache();

 private:
  std::vector<Record> m_records;
};

}  // namespace ProjectStockMarket
