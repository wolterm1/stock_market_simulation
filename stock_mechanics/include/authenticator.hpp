#pragma once

#include <string>
#include <vector>

#include "account.hpp"
#include "user.hpp"

namespace ProjectStockMarket {

struct Authenticator {
  /**
   * @brief checks if Account is in db
   * @return a string token
   */
  std::string login(Account a);
  /**
   * @brief adds Accout and new User to db and logs them in
   * @return a string token
   */
  std::string registerAcc(std::string displayName, Account a);
  /**
   * @brief removes token from db
   * @return nothing
   */
  void logout(std::string token);
  /**
   * @brief finds the User in db by token
   * @return  returns the User
   */
  User getUserByToken(std::string token);

  // remove these
  std::string generateRandomToken(size_t length);
};

}  // namespace ProjectStockMarket
