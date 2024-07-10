#pragma once

#include <string>

namespace ProjectStockMarket {

struct Account {
  std::string username;
  std::string password;

  Account(std::string u, std::string p) : username(u), password(p) {}
};

}  // namespace ProjectStockMarket
