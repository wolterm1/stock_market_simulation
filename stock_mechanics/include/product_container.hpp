#pragma once
#include <vector>

#include "product.hpp"

namespace ProjectStockMarket {

class ProductContainer {
 private:
  typedef struct {
    Product product;
    int count;
  } ProductCountEntry;

 public:
  ProductContainer(std::vector<Product> product_list);
  ProductContainer();

  bool hasProduct(Product p);
  bool hasProduct(int product_id);

  void addProduct(Product p);
  void addProduct(int product_id);
  void removeProduct(Product p, int count);
  void removeProduct(int product_id, int count);

  std::vector<ProductCountEntry> getProductCountEntriesWithGivenName(std::string product_name);
  std::vector<ProductCountEntry> getAllProductCountEntries();
  Product getProduct(int product_id);
  std::string getProductName(int product_id);
  int getProductPrize(int product_id);
  int getProductCount(Product p);
  int getProductCount(int product_id);
  std::vector<Product> GetProductsWithGivenCount(int count);
  int getSize();

 private:
  std::vector<ProductCountEntry> m_storage;
};

}  // namespace ProjectStockMarket
