#include "records_cache.hpp"

using namespace ProjectStockMarket;

void RecordsCache::refresh(int limit) {}
void RecordsCache::addRecord(int price) {}
std::vector<Record> RecordsCache::getRecords() {
  return m_records;
}
RecordsCache::RecordsCache() {}
RecordsCache::RecordsCache(std::vector<Record> records) : m_records(records) {}
