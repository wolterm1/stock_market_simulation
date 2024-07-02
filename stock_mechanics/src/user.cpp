#include "user.hpp"

using namespace ProjectStockMarket;

User::User() {}
User::User(int id, std::string name, int b) : m_id(id), m_name(name), m_balance(b) {}
User::User(int id, std::string name, int b, ProductContainer inv)
    : m_id(id), m_name(name), m_balance(b), m_inventory(inv) {}

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

int User::getId() {
  return m_id;
}
std::string User::getName() {
  return m_name;
}
int User::getBalance() {
  return m_balance;
}
// TODO
std::vector<ProductEntry> User::getInventory() {
  return {};
}
