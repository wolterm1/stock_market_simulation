#pragma once

#include <string>
#include <vector>

#include "account.hpp"
#include "user.hpp"

namespace ProjectStockMarket {

struct Authenticator {
  std::string login(Account a);
  std::string registerAcc(Account a);
  void logout(std::string token);
  User getUserByToken(std::string token);
};

}  // namespace ProjectStockMarket
