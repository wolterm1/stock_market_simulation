#pragma once
#include <vector>

#include "product_entry.hpp"
#include "timer.hpp"

namespace ProjectStockMarket {

class MarketPlace {
 public:
  std::vector<ProductEntry> getInventory();
  std::vector<Product> getAllProduct();
  void addProductEntry(std::string name, int price, int count);
  void addProductEntry(ProductEntry p_entry);
  void startPriceUpdate();
  int getCurrentTime();

  ~MarketPlace();
  MarketPlace(int limit_record_entries);

 private:
  Timer timer;
  int m_limit_record_entries = 100;
  std::vector<ProductEntry> inventory;
  std::vector<Product> all_products;
  void updateProductPrices();  // thread
};

}  // namespace ProjectStockMarket
