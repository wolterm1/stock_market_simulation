#pragma once

#include <string>
#include <vector>

#include "account.hpp"
#include "user.hpp"

namespace ProjectStockMarket {

class AccountManager {
 public:
  bool isAccountRegistered();
  void addAccount(Account a);
  void removeAccount(Account a);
  User getUser(Account a);
  Account getAccount(User u);

 private:
  std::vector<Account> m_accounts;
};

}  // namespace ProjectStockMarket
