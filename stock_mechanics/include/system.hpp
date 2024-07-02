// here will be code which controls the market, users, and accounts
#pragma once

#include <string>
#include <vector>

#include "account_manager.hpp"
#include "market_place.hpp"

namespace ProjectStockMarket {

class System {
 public:
  // System dependency
  void evaluateAllProductPrices();
  static int getPrice(int product_id);
  // Account
  void registerAccount(Account a);
  User signIn(Account a);
  User getUser(Account a);

 public:
  MarketPlace market;

 private:
  AccountManager acc_manager;

  int preisentwicklung;  // TODO
};

}  // namespace ProjectStockMarket
