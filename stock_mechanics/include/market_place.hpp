#pragma once
#include <random>
#include <vector>

#include "product_entry.hpp"
#include "record.hpp"
#include "timer.hpp"

namespace ProjectStockMarket {

class MarketPlace {
 public:
  MarketPlace(int limit_record_entries, bool try_generate_products);
  ~MarketPlace();
  std::vector<ProductEntry> getInventory();
  std::vector<Product> getAllProducts();
  Product addProduct(const std::string& p_name, int p_count);
  void startPriceUpdate();

 private:
  int randomWalk(int current_price, double trend, double streuung, double dt);      

 private:
  std::random_device rd;
  std::mt19937 gen{rd()};

  std::uniform_real_distribution<> trendDistribution{-0.05, 0.04};
    std::uniform_real_distribution<> randomNumber{2, 4};


  Timer timer;
  int m_limit_record_entries = 3600;
  void updateProductPrices();  // thread
};

}  // namespace ProjectStockMarket
