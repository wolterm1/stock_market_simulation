#pragma once

#include <string>
#include <vector>

#include "account.hpp"
#include "user.hpp"

namespace ProjectStockMarket {

struct Authenticator {
  /**
   * @brief Generates a login token used for authentication
   * @returns the generated token
   */
  static std::string login(const Account& account);
  static std::string registerAccount(const std::string& username,
                                     const std::string& password,
                                     const std::string& display_name);
  /**
   * @brief removes token from db
   * @return nothing
   */
  static void logout(std::string token);

  /**
   * @brief finds the User in db by token
   * @return  returns the User
   */
  static User findUserByToken(std::string p_token);
};

}  // namespace ProjectStockMarket
