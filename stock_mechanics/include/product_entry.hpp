#pragma once
#include <vector>

#include "product.hpp"

namespace ProjectStockMarket {

struct ProductEntry {
  Product product;
  int count;
  ProductEntry(Product p, int c) : product(p), count(c) {}
};

}  // namespace ProjectStockMarket
