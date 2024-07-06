#include <iostream>
#include <vector>

#include "authenticator.hpp"
#include "db_connector.hpp"
#include "user.hpp"

/**
 * hiervor die andere main callen lol, dann sind
 * die accounts schon drinne sonst findet man in LOGIN die User von
 * accounts nicht da diese nicht existieren, war faul
 */

void TEST_LOGIN() {
  std::cerr << "LOGIN: " << std::endl;
  std::vector<ProjectStockMarket::Account> accs;
  ProjectStockMarket::Authenticator auth;

  accs.push_back(ProjectStockMarket::Account("0", "password"));
  accs.push_back(ProjectStockMarket::Account("1", "password"));
  accs.push_back(ProjectStockMarket::Account("2", "password"));
  accs.push_back(ProjectStockMarket::Account("3", "password"));
  accs.push_back(ProjectStockMarket::Account("4", "password"));
  accs.push_back(ProjectStockMarket::Account("5", "password"));
  accs.push_back(ProjectStockMarket::Account("6", "password"));
  accs.push_back(ProjectStockMarket::Account("7", "password"));
  accs.push_back(ProjectStockMarket::Account("8", "password"));
  accs.push_back(ProjectStockMarket::Account("9", "password"));

  for (int i = 0; i <= 9; i++) {
    std::string tok = auth.login(accs[i]);
  }
}

void TEST_REGISTER() {
  ProjectStockMarket::Authenticator auth;

  // create a new account with random string as username, weil unique sein muss
  ProjectStockMarket::Account acc_that_doesnt_exist(std::string(auth.generateRandomToken(7)),
                                                    "password");

  auth.registerAcc("i dont exist", acc_that_doesnt_exist);
}

void TEST_LOGOUT() {
  ProjectStockMarket::Authenticator auth;
  std::string input;
  std::string logout_input;
  std::cout << "Do you want to logout? y/n:";
  std::cin >> logout_input;
  if (logout_input == "y") {
    std::cin >> input;
    auth.logout(input);
  }
}

int main() {
  TEST_REGISTER();
  TEST_LOGIN();
  TEST_LOGOUT();
}
