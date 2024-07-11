#pragma once
#include <string>
#include <vector>

#include "product.hpp"


namespace ProjectStockMarket {

struct ProductEntry {
  Product product;
  int count;
  ProductEntry(Product p, int c) : product(p), count(c) {}

  bool operator==(const ProductEntry& other) const {
    return product == other.product && count == other.count;
  }

  operator std::string() const {
    return "Product: " + product.getName() + " Count: " + std::to_string(count);
  }
};

}  // namespace ProjectStockMarket
