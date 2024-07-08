#include "authenticator.hpp"

#include <array>
#include <exception>
#include <iostream>

#include "db_connector.hpp"
#include "user.hpp"

using namespace ProjectStockMarket;

// beim login existiert account und user schon also generieren wir token und speichern den
std::string Authenticator::login(Account p_a) {
  std::string token = generateRandomToken(16);
  DBConnector::addAccountToken(p_a, token);
  return token;
}

// beim register existiert der Account und User noch nicht, also speichern diesen und wenn alles,
// klappt, login
std::string Authenticator::registerAcc(std::string p_displayname, Account p_a) {
  User newUser(p_a, p_displayname, 0);
  DBConnector::addUser(newUser);
  std::string token = generateRandomToken(16);
  DBConnector::addAccountToken(p_a, token);
  return token;
}

void Authenticator::logout(std::string p_token) {
  DBConnector::removeTokenFromAccountDB(p_token);
}

User Authenticator::getUserByToken(std::string p_token) {
  return DBConnector::getUser(p_token);
}

std::string Authenticator::generateRandomToken(size_t p_size) {
  std::srand(std::time(0));
  std::string tmp;
  std::string table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  int sizeTable = table.size() - 1;
  for (int i = 0; i < p_size; i++) {
    tmp += table[std::rand() % sizeTable];
  }
  return tmp;
}
