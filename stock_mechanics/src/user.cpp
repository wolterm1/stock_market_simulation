#include "user.hpp"

#include "db_connector.hpp"

using namespace ProjectStockMarket;

User::User() {}
User::User(Account a, std::string name, int b) : m_account(a), m_name(name), m_balance(b) {}
User::User(Account a, std::string name, int b, std::vector<ProductEntry> inv)
    : m_account(a), m_name(name), m_balance(b), m_inventory(inv) {}

void User::buyProduct(ProductEntry p_entry) {
  int product_total_price = DBConnector::getLatestRecordPrice(p_entry.product) * p_entry.count;
  if (m_balance < product_total_price)
    throw std::runtime_error("not enough balance to buy these product amount");
  m_balance = getBalance();
  m_balance -= product_total_price;
  DBConnector::removeProductEntryFromMarket(p_entry);
  DBConnector::addProductEntryToInventory(p_entry, *this);
  DBConnector::updateUser(*this, getName(), m_balance);
}
void User::sellProduct(ProductEntry p_entry) {
  int product_total_price_earned =
      DBConnector::getLatestRecordPrice(p_entry.product) * p_entry.count;
  m_balance = getBalance();
  m_balance += product_total_price_earned;
  DBConnector::removeProductEntryFromInventory(p_entry, *this);
  DBConnector::addProductEntryToMarket(p_entry);
  DBConnector::updateUser(*this, getName(), m_balance);
}

Account User::getAccount() {
  return m_account;
}

void User::increaseBalance(int amount) {
  int newBalance = getBalance() + amount;
  DBConnector::updateUser(*this, getName(), newBalance);
  m_balance = getBalance();
}

int User::getId() {
  return DBConnector::getUserId(*this);
}
std::string User::getName() {
  try {
    m_name = DBConnector::getUserName(m_account);
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
