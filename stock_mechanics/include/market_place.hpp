#pragma once
#include <vector>

#include "product_entry.hpp"

namespace ProjectStockMarket {

class MarketPlace {
 public:
  std::vector<ProductEntry> getInventory();
  std::vector<Product> getAllProduct();

  MarketPlace(int limit_record_entries);

 private:
  int m_limit_record_entries;
  std::vector<ProductEntry> inventory;
  std::vector<Product> all_products;
  void updateProductPrices();  // thread
};

}  // namespace ProjectStockMarket
