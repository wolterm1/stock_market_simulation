#include "user.hpp"

#include "db_connector.hpp"
#include "exception_classes.hpp"

using namespace ProjectStockMarket;

void User::buyProduct(const Product& p_product, int p_amount) {
  int current_price = p_product.getCurrentPrice();
  int product_total_price = current_price * p_amount;
  if (m_balance < product_total_price)
    throw NotEnoughMoney("Insufficient balance to buy these products. " +
                         std::to_string(product_total_price) + " needed.");
  DBConnector::updateMarketProductEntry(p_product, -p_amount);
  DBConnector::updateUserProductEntry(*this, p_product, p_amount);
  m_balance -= product_total_price;
  DBConnector::updateUser(*this);
}

void User::sellProduct(const Product& p_product, int p_amount) {
  int current_price = p_product.getCurrentPrice();
  int product_total_price = current_price * p_amount;
  DBConnector::updateUserProductEntry(*this, p_product, -p_amount);
  DBConnector::updateMarketProductEntry(p_product, p_amount);
  m_balance += product_total_price;
  DBConnector::updateUser(*this);
}

int User::getId() const { return m_id; }

std::string User::getName() const { return m_name; }

int User::getBalance() const { return m_balance; }

std::vector<ProductEntry> User::getInventory() {
  return DBConnector::getUserInventory(*this);
}
