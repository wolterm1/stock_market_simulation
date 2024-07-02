#include "db_connector.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace ProjectStockMarket {
DBConnector::DBConnector(const std::string& dbName)
    : m_database(dbName, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
  createTables();
}

void DBConnector::createTables() {
  std::string createUserTable =
      "CREATE TABLE IF NOT EXISTS User ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "name TEXT NOT NULL, "
      "balance REAL NOT NULL, "
      "account_id INTEGER, "
      "FOREIGN KEY(account_id) REFERENCES Account(id));";

  std::string createAccountTable =
      "CREATE TABLE IF NOT EXISTS Account ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "username TEXT NOT NULL, "
      "password TEXT NOT NULL);";

  std::string createProductTable =
      "CREATE TABLE IF NOT EXISTS Product ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "name TEXT NOT NULL);";

  std::string createUserInventoryTable =
      "CREATE TABLE IF NOT EXISTS UserInventory ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "user_id INTEGER, "
      "product_id INTEGER, "
      "amount INTEGER, "
      "FOREIGN KEY(user_id) REFERENCES User(id), "
      "FOREIGN KEY(product_id) REFERENCES Product(id));";

  std::string createProductPriceTable =
      "CREATE TABLE IF NOT EXISTS ProductPrice ("
      "product_id INTEGER PRIMARY KEY, "
      "price REAL NOT NULL, "
      "FOREIGN KEY(product_id) REFERENCES Product(id));";

  std::string createMarketTable =
      "CREATE TABLE IF NOT EXISTS Market ("
      "product_id INTEGER UNIQUE, "
      "amount INTEGER, "
      "FOREIGN KEY(product_id) REFERENCES Product(id));";

  m_database.exec(createAccountTable);
  m_database.exec(createUserTable);
  m_database.exec(createProductTable);
  m_database.exec(createUserInventoryTable);
  m_database.exec(createProductPriceTable);
  m_database.exec(createMarketTable);
}

void DBConnector::addAccount(Account p_account, const std::string& p_name, int p_balance) {
  try {
    SQLite::Transaction transaction(m_database);

    // Insert into Account
    SQLite::Statement insertAccount(m_database,
                                    "INSERT INTO Account (username, password) VALUES (?, ?);");
    insertAccount.bind(1, p_account.username);
    insertAccount.bind(2, p_account.password);
    insertAccount.exec();
    int account_id = static_cast<int>(m_database.getLastInsertRowid());

    // Insert into User
    SQLite::Statement insertUser(m_database,
                                 "INSERT INTO User (name, balance, account_id) VALUES (?, ?, ?);");
    insertUser.bind(1, p_name);
    insertUser.bind(2, p_balance);
    insertUser.bind(3, account_id);
    insertUser.exec();

    transaction.commit();
  } catch (const std::exception& e) {
    throw std::runtime_error("Error adding account and user: " + std::string(e.what()));
  }
}

void DBConnector::addProductToMarket(const std::string& p_product_name, int p_amount, int p_price) {
  try {
    std::cout << "Starting transaction for adding product to market..." << std::endl;
    SQLite::Transaction transaction(m_database);

    // Check if the product exists in the Product table
    SQLite::Statement checkProduct(m_database, "SELECT id FROM Product WHERE name = ?;");
    checkProduct.bind(1, p_product_name);
    int product_id = -1;

    if (checkProduct.executeStep()) {
      product_id = checkProduct.getColumn(0).getInt();
      std::cout << "Product exists with id: " << product_id << std::endl;
    } else {
      // Insert product if it doesn't exist
      std::cout << "Product does not exist, inserting new product..." << std::endl;
      SQLite::Statement insertProduct(m_database, "INSERT INTO Product (name) VALUES (?);");
      insertProduct.bind(1, p_product_name);
      insertProduct.exec();
      product_id = static_cast<int>(m_database.getLastInsertRowid());
      std::cout << "Inserted new product with id: " << product_id << std::endl;
    }

    // Check if the product price exists
    SQLite::Statement checkProductPrice(m_database,
                                        "SELECT COUNT(*) FROM ProductPrice WHERE product_id = ?;");
    checkProductPrice.bind(1, product_id);
    checkProductPrice.executeStep();
    int count = checkProductPrice.getColumn(0).getInt();

    if (count == 0) {
      // Insert product price if it doesn't exist
      std::cout << "Product price does not exist, inserting new price..." << std::endl;
      SQLite::Statement insertProductPrice(
          m_database, "INSERT INTO ProductPrice (product_id, price) VALUES (?, ?);");
      insertProductPrice.bind(1, product_id);
      insertProductPrice.bind(2, static_cast<double>(p_price));  // Ensure price is bound as REAL
      insertProductPrice.exec();
      std::cout << "Inserted new product price for product id: " << product_id << std::endl;
    }

    // Insert into Market or update amount if it already exists
    std::cout << "Inserting/Updating market entry..." << std::endl;
    // Try to insert. If there's a conflict, do nothing due to INSERT OR IGNORE.
    SQLite::Statement insertMarket(
        m_database, "INSERT OR IGNORE INTO Market (product_id, amount) VALUES (?, ?);");
    insertMarket.bind(1, product_id);
    insertMarket.bind(2, p_amount);
    insertMarket.exec();

    // Now update the amount if a conflict occurred (product_id already exists).
    SQLite::Statement updateMarket(m_database,
                                   "UPDATE Market SET amount = amount + ? WHERE product_id = ?;");
    updateMarket.bind(1, p_amount);
    updateMarket.bind(2, product_id);
    updateMarket.exec();
    std::cout << "Market entry inserted/updated for product id: " << product_id << std::endl;

    transaction.commit();
    std::cout << "Transaction committed successfully." << std::endl;
  } catch (const SQLite::Exception& e) {
    std::cerr << "SQLite error: " << e.getErrorStr() << " - " << e.what() << std::endl;
    throw std::runtime_error("Error adding product to market: " + std::string(e.what()));
  } catch (const std::exception& e) {
    std::cerr << "Standard exception: " << e.what() << std::endl;
    throw std::runtime_error("Error adding product to market: " + std::string(e.what()));
  } catch (...) {
    std::cerr << "Unknown error occurred." << std::endl;
    throw std::runtime_error("Unknown error adding product to market.");
  }
}

void DBConnector::addProductToInventory(int p_user_id, int p_product_id, int p_amount) {
  try {
    SQLite::Transaction transaction(m_database);

    // Insert into UserInventory or update amount if it already exists
    SQLite::Statement insertInventory(
        m_database,
        "INSERT INTO UserInventory (user_id, product_id, amount) VALUES (?, ?, ?) "
        "ON CONFLICT(user_id, product_id) DO UPDATE SET amount = amount + "
        "excluded.amount;");
    insertInventory.bind(1, p_user_id);
    insertInventory.bind(2, p_product_id);
    insertInventory.bind(3, p_amount);
    insertInventory.exec();

    transaction.commit();
  } catch (const std::exception& e) {
    throw std::runtime_error("Error adding product to user inventory: " + std::string(e.what()));
  }
}

std::vector<ProductEntry> DBConnector::getInventory(int p_user_id) {
  try {
    SQLite::Statement query(
        m_database,
        "SELECT Product.id, Product.name, UserInventory.amount FROM UserInventory "
        "JOIN Product ON UserInventory.product_id = Product.id "
        "WHERE UserInventory.user_id = ?;");
    query.bind(1, p_user_id);

    std::vector<ProductEntry> inventory;
    while (query.executeStep()) {
      int product_id = query.getColumn(0).getInt();
      std::string productName = query.getColumn(1).getString();
      Product p(product_id, productName);
      int amount = query.getColumn(2).getInt();
      inventory.emplace_back(ProductEntry(p, amount));
    }
    return inventory;
  } catch (const std::exception& e) {
    throw std::runtime_error("Error retrieving user inventory: " + std::string(e.what()));
  }
}

int DBConnector::getProductPrice(int p_product_id) {
  try {
    SQLite::Statement query(m_database, "SELECT price FROM ProductPrice WHERE product_id = ?;");
    query.bind(1, p_product_id);

    if (query.executeStep()) {
      double price = query.getColumn(0).getDouble();
      return static_cast<int>(price);  // Ensure the price is returned as an integer if needed
    } else {
      throw std::runtime_error("Product price not found");
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("Error retrieving product price: " + std::string(e.what()));
  }
}

std::vector<ProductEntry> DBConnector::getAllMarketProducts() {
  try {
    SQLite::Statement query(m_database,
                            "SELECT Product.id, Product.name, Market.amount FROM Market "
                            "JOIN Product ON Market.product_id = Product.id;");

    std::vector<ProductEntry> market_products;
    while (query.executeStep()) {
      int product_id = query.getColumn(0).getInt();
      std::string productName = query.getColumn(1).getString();
      Product p(product_id, productName);
      int amount = query.getColumn(2).getInt();
      market_products.emplace_back(ProductEntry(p, amount));
    }
    return market_products;
  } catch (const std::exception& e) {
    throw std::runtime_error("Error retrieving market products: " + std::string(e.what()));
  }
}

User DBConnector::getUserFromAccount(const std::string& p_username, const std::string& p_password) {
  try {
    SQLite::Statement query(m_database,
                            "SELECT User.id, User.name, User.balance FROM User "
                            "JOIN Account ON User.account_id = Account.id "
                            "WHERE Account.username = ? AND Account.password = ?;");
    query.bind(1, p_username);
    query.bind(2, p_password);

    if (query.executeStep()) {
      int userId = query.getColumn(0).getInt();
      std::string name = query.getColumn(1).getString();
      double balance = query.getColumn(2).getDouble();  // Ensure balance is retrieved as REAL
      return User(userId, name, balance);
    } else {
      throw std::runtime_error("User not found");
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("Error retrieving user from account: " + std::string(e.what()));
  }
}

}  // namespace ProjectStockMarket
