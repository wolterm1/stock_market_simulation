#include "product_container.hpp"

using namespace ProjectStockMarket;

ProductContainer::ProductContainer(std::vector<ProductEntry> product_list)
    : m_storage(product_list) {}
ProductContainer::ProductContainer() {}
