#pragma once
#include <vector>

#include "product_entry.hpp"

namespace ProjectStockMarket {

class ProductContainer {
 public:
  ProductContainer(std::vector<ProductEntry> product_list);
  ProductContainer();

  bool hasProduct(Product p);
  bool hasProduct(int product_id);

  void addProduct(Product p);
  void addProduct(int product_id);
  void removeProduct(Product p, int count);
  void removeProduct(int product_id, int count);

  std::vector<ProductEntry> getProductCountEntriesWithGivenName(std::string product_name);
  std::vector<ProductEntry> getAllProductCountEntries();
  Product getProduct(int product_id);
  std::string getProductName(int product_id);
  int getProductPrize(int product_id);
  int getProductCount(Product p);
  int getProductCount(int product_id);
  std::vector<Product> GetProductsWithGivenCount(int count);
  int getSize();

 private:
  std::vector<ProductEntry> m_storage;
};

}  // namespace ProjectStockMarket
