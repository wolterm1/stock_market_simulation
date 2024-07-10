#include "authenticator.hpp"

#include <array>
#include <exception>
#include <iostream>

#include "db_connector.hpp"
#include "user.hpp"

using namespace ProjectStockMarket;

std::string generateRandomToken(size_t p_size) {
  std::srand(std::time(0));
  std::string tmp;
  std::string table =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  int sizeTable = table.size() - 1;
  for (int i = 0; i < p_size; i++) {
    tmp += table[std::rand() % sizeTable];
  }
  return tmp;
}

// beim login existiert account und user schon also generieren wir token und
// speichern den
std::string Authenticator::login(const Account& account) {
  int account_id = DBConnector::verifyCredentials(account);
  std::cout << "Account ID: " << account_id << std::endl;
  std::string token = generateRandomToken(16);
  DBConnector::addToken(account_id, token);
  return token;
}

std::string Authenticator::registerAccount(const std::string& username,
                                           const std::string& password,
                                           const std::string& display_name) {
  Account account(username, password);
  DBConnector::registerAccount(account, display_name);
  return login(account);
}

void Authenticator::logout(std::string p_token) {
  DBConnector::removeToken(p_token);
}

User Authenticator::findUserByToken(std::string p_token) {
  return DBConnector::getUserByToken(p_token);
}
