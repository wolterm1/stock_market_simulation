#include "user.hpp"

#include "db_connector.hpp"

using namespace ProjectStockMarket;

User::User() {}
User::User(Account a, std::string name, int b) : m_account(a), m_name(name), m_balance(b) {}
User::User(Account a, std::string name, int b, std::vector<ProductEntry> inv)
    : m_account(a), m_name(name), m_balance(b), m_inventory(inv) {}

// TODO
bool User::buyProduct(int product_id, int count) {
  return true;
}
bool User::buyProduct(Product product, int count) {
  return true;
}
bool User::sellProduct(int product_id, int count) {
  return true;
}
bool User::sellProduct(Product product, int count) {
  return true;
}

Account User::getAccount() {
  return m_account;
}

int User::getId() {
  return ProjectStockMarket::DBConnector::getUserId(*this);
}
std::string User::getName() {
  try {
    m_name = DBConnector::getUserBalance(m_account);
  } catch (const std::exception& e) {
  }

  return m_name;
}
int User::getBalance() {
  try {
    m_balance = DBConnector::getUserBalance(m_account);
  } catch (const std::exception& e) {
  }
  return m_balance;
}
std::vector<ProductEntry> User::getInventory() {
  try {
    return DBConnector::getAllProductEntriesFromInventory(*this);
  } catch (const std::exception& e) {
    return {};
  }
}
