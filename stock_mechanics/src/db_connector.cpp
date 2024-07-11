#include "db_connector.hpp"

#include <chrono>

#include "exception_classes.hpp"

// #include <format>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace ProjectStockMarket {

std::string to_iso_string(const time_point& tp) {
  auto in_time_t = std::chrono::system_clock::to_time_t(tp);
  std::tm tm = *std::gmtime(&in_time_t);
  char buffer[30];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S%z", &tm);
  return std::string(buffer);
}

time_point from_iso_string(const std::string& iso_string) {
  std::istringstream in{iso_string};
  std::tm tm = {};
  in >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S%z");
  return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::unique_ptr<SQLite::Database> DBConnector::m_database = nullptr;
// statically initializes the class without having an instance of it
void DBConnector::initDB(std::string path) {
  std::cout << "trying to initialize database...";
  m_database = std::make_unique<SQLite::Database>(
      path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  std::cout << " -> " << path << " Initialized!" << std::endl;
  m_database->exec("PRAGMA busy_timeout = 5000;");
  m_database->exec("PRAGMA journal_mode = WAL;");
  createTables();
}

void DBConnector::createTables() {
  try {
    // Account
    m_database->exec(
        "CREATE TABLE IF NOT EXISTS Account ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL UNIQUE, "
        "password TEXT NOT NULL, "
        "token TEXT);");
    // User
    m_database->exec(
        "CREATE TABLE IF NOT EXISTS User ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT NOT NULL, "
        "balance INTEGER NOT NULL, "
        "FOREIGN KEY(id) REFERENCES Account(id));");
    // Product
    m_database->exec(
        "CREATE TABLE IF NOT EXISTS Product ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE);");
    // Marketplace
    m_database->exec(
        "CREATE TABLE IF NOT EXISTS Marketplace ("
        "product_id INTEGER, "
        "count INTEGER NOT NULL, "
        "FOREIGN KEY(product_id) REFERENCES Product(id));");
    // Inventory
    m_database->exec(
        "CREATE TABLE IF NOT EXISTS Inventory ("
        "user_id INTEGER, "
        "product_id INTEGER, "
        "count INTEGER NOT NULL, "
        "FOREIGN KEY(user_id) REFERENCES User(id), "
        "FOREIGN KEY(product_id) REFERENCES Product(id), "
        "PRIMARY KEY(user_id, product_id));");
    // PriceRecord
    // datetime in ISO 8601 format
    m_database->exec(
        "CREATE TABLE IF NOT EXISTS PriceRecord ("
        "entry_num INTEGER PRIMARY KEY AUTOINCREMENT,"
        "product_id INTEGER, "
        "date_time TEXT NOT NULL, "
        "price INTEGER NOT NULL, "
        "FOREIGN KEY(product_id) REFERENCES Product(id));");
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to create tables: " +
                             std::string(e.what()));
  }
}

void DBConnector::addPriceRecordLimitTrigger(int limit) {
  try {
    SQLite::Statement query(*m_database,
                            "CREATE TRIGGER IF NOT EXISTS prune_records "
                            "AFTER INSERT ON PriceRecord "
                            "FOR EACH ROW "
                            "BEGIN "
                            "DELETE FROM PriceRecord WHERE rowid IN ("
                            "SELECT entry_num FROM PriceRecord "
                            "WHERE product_id = NEW.product_id "
                            "ORDER BY date_time DESC "
                            "LIMIT -1 OFFSET " +
                                std::to_string(limit) +
                                ");"
                                "END;");
    query.exec();
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to add trigger: " + std::string(e.what()));
  }
}

void DBConnector::registerAccount(const Account& account,
                                  const std::string& display_name) {
  // Check if account already exists
  try {
    SQLite::Statement check_account(
        *m_database, "SELECT id FROM Account WHERE username = ?");
    check_account.bind(1, account.username);
    if (check_account.executeStep()) {
      throw AccountAlreadyExists("Account already exists");
    }
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to check for existing account: " +
                             std::string(e.what()));
  }
  // Register account
  try {
    SQLite::Statement query(
        *m_database, "INSERT INTO Account (username, password) VALUES (?, ?)");
    query.bind(1, account.username);
    query.bind(2, account.password);
    query.exec();
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to register account: " +
                             std::string(e.what()));
  }

  // add corresponding user
  try {
    SQLite::Statement query(
        *m_database, "INSERT INTO User (id, name, balance) VALUES (?, ?, ?)");
    query.bind(1, m_database->getLastInsertRowid());
    query.bind(2, display_name);
    query.bind(3, 1000);  // INITIAL_BALANCE
    query.exec();
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to add user: " + std::string(e.what()));
  }
}

User DBConnector::getUser(int p_user_id) {
  try {
    SQLite::Statement query(*m_database,
                            "SELECT name, balance FROM User WHERE id = ?");
    query.bind(1, p_user_id);

    if (query.executeStep()) {
      std::string name = query.getColumn(0).getText();
      int balance = query.getColumn(1).getInt();
      return User(p_user_id, name, balance);
    } else {
      throw UserNotFound("User not found for account ID " +
                         std::to_string(p_user_id));
    }
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to find user: " + std::string(e.what()));
  }
}

void DBConnector::updateUser(const User& p_user) {
  try {
    SQLite::Statement query(
        *m_database, "UPDATE User SET name = ?, balance = ? WHERE id = ?");
    query.bind(1, p_user.getName());
    query.bind(2, p_user.getBalance());
    query.bind(3, p_user.getId());
    query.exec();
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to update user: " + std::string(e.what()));
  }
}

Product DBConnector::addProduct(const std::string& p_product_name,
                                int p_count) {
  try {
    SQLite::Statement query(*m_database,
                            "INSERT INTO Product (name) VALUES (?)");
    query.bind(1, p_product_name);
    query.exec();

    SQLite::Statement addProductToMarket(
        *m_database,
        "INSERT INTO Marketplace (product_id, count) VALUES (?, ?)");
    addProductToMarket.bind(1, m_database->getLastInsertRowid());
    addProductToMarket.bind(2, p_count);
    addProductToMarket.exec();

    return Product(m_database->getLastInsertRowid(), p_product_name);
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to add product: " + std::string(e.what()));
  }
}

Product DBConnector::getProduct(int p_product_id) {
  SQLite::Statement findProductName(*m_database,
                                    "SELECT name FROM Product WHERE id = ?");
  findProductName.bind(1, p_product_id);
  if (!findProductName.executeStep()) {
    throw ProductNotFound("Product not found for ID " +
                          std::to_string(p_product_id));
  }
  return Product(p_product_id, findProductName.getColumn(0).getText());
}

void DBConnector::updateMarketProductEntry(const Product& p_product,
                                           int p_change) {
  // Get Current Entry
  SQLite::Statement query(
      *m_database, "SELECT count FROM Marketplace WHERE product_id = ? ");

  query.bind(1, p_product.getId());

  if (!query.executeStep()) {
    throw ProductNotFound("Product " + p_product.getName() +
                          " not found in market");
  }

  int currentAmount = query.getColumn(0).getInt();
  int newAmount = currentAmount + p_change;
  if (newAmount < 0) {  // Not Enough Products
    throw OutOfStock("Tried to buy " + std::to_string(std::abs(p_change)) +
                     " products, but only " + std::to_string(currentAmount) +
                     " are available");
  }

  SQLite::Statement updateQuery(
      *m_database, "UPDATE Marketplace SET count = ? WHERE product_id = ?");
  updateQuery.bind(1, newAmount);
  updateQuery.bind(2, p_product.getId());
  updateQuery.exec();
}

std::vector<ProductEntry> DBConnector::getMarketInventory() {
  try {
    std::vector<ProductEntry> productEntries;
    SQLite::Statement query(*m_database,
                            "SELECT product_id, count FROM Marketplace");
    while (query.executeStep()) {
      int id = query.getColumn(0).getInt();
      int count = query.getColumn(1).getInt();
      productEntries.emplace_back(getProduct(id), count);
    }
    return productEntries;
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to get all productentries from market: " +
                             std::string(e.what()));
  }
}

std::vector<Product> DBConnector::getAllProducts() {
  try {
    std::vector<Product> products;
    SQLite::Statement query(*m_database, "SELECT id FROM Product");
    while (query.executeStep()) {
      int id = query.getColumn(0).getInt();
      products.emplace_back(getProduct(id));
    }
    return products;
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to get all products: " +
                             std::string(e.what()));
  }
}

void DBConnector::updateUserProductEntry(const User& p_user,
                                         const Product& p_product,
                                         int p_change) {
  // Get Current Entry
  SQLite::Statement query(*m_database,
                          "SELECT count FROM Inventory WHERE user_id = ? AND "
                          "product_id = ?");
  query.bind(1, p_user.getId());
  query.bind(2, p_product.getId());

  int currentAmount = 0;
  if (query.executeStep()) {
    currentAmount = query.getColumn(0).getInt();
  }
  int newAmount = currentAmount + p_change;
  if (newAmount < 0) {  // Not Enough Products
    throw NotInInventory("Tried to sell " + std::to_string(std::abs(p_change)) +
                         " products, but only " +
                         std::to_string(currentAmount) + " are available");
  } else if (currentAmount == 0 && newAmount == 0) {  // No Entry and no change
    return;
  } else if (newAmount == 0) {  // Entry exists, but new amount is 0, so delete
    SQLite::Statement deleteQuery(
        *m_database,
        "DELETE FROM Inventory WHERE user_id = ? AND product_id = ?");
    deleteQuery.bind(1, p_user.getId());
    deleteQuery.bind(2, p_product.getId());
    deleteQuery.exec();
  } else if (currentAmount ==
             0) {  // Entry does not exist, new amount is more than 0, so insert
    SQLite::Statement insertQuery(
        *m_database,
        "INSERT INTO Inventory (user_id, product_id, count) VALUES (?, ?, ?)");
    insertQuery.bind(1, p_user.getId());
    insertQuery.bind(2, p_product.getId());
    insertQuery.bind(3, newAmount);
    insertQuery.exec();
  } else {  // Entry exists, new amount is more than 0, so update
    SQLite::Statement updateQuery(
        *m_database,
        "UPDATE Inventory SET count = ? WHERE user_id = ? AND product_id = "
        "?");
    updateQuery.bind(1, newAmount);
    updateQuery.bind(2, p_user.getId());
    updateQuery.bind(3, p_product.getId());
    updateQuery.exec();
  }
}

std::vector<ProductEntry> DBConnector::getUserInventory(User p_user) {
  try {
    std::vector<ProductEntry> products;
    SQLite::Statement query(
        *m_database,
        "SELECT product_id, count FROM Inventory WHERE user_id = ?");
    query.bind(1, p_user.getId());
    std::vector<ProductEntry> product_entries;
    while (query.executeStep()) {
      int id = query.getColumn(0).getInt();
      int count = query.getColumn(1).getInt();
      products.emplace_back(getProduct(id), count);
    }
    return products;
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to get all products from inventory: " +
                             std::string(e.what()));
  }
}

void DBConnector::addRecord(const Product& p_product, const Record& p_record) {
  try {
    SQLite::Statement query(*m_database,
                            "INSERT INTO PriceRecord (product_id, date_time, "
                            "price) VALUES (?, ?, ?)");
    query.bind(1, p_product.getId());
    query.bind(2, to_iso_string(p_record.dateTime));
    query.bind(3, p_record.price);
    query.exec();
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to add record: " + std::string(e.what()));
  }
}

std::vector<Record> DBConnector::getAllRecords(const Product& product) {
  SQLite::Statement query(*m_database,
                          "SELECT date_time, price FROM PriceRecord WHERE "
                          "product_id = ? ORDER BY date_time ASC;");
  query.bind(1, product.getId());
  std::vector<Record> records;
  try {
    while (query.executeStep()) {
      std::string dateTimeText = query.getColumn(0).getString();
      time_point dateTime = from_iso_string(dateTimeText);
      int price = query.getColumn(1).getInt();
      records.emplace_back(dateTime, price);
    }
    return records;
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to get records: " + std::string(e.what()));
  }
}

std::vector<Record> DBConnector::getRecords(const Product& product,
                                            const time_point& from,
                                            const time_point& to) {
  SQLite::Statement query(
      *m_database,
      "SELECT date_time, price FROM PriceRecord WHERE product_id = ? AND "
      "date_time >= ? AND date_time <= ? ORDER BY date_time ASC;");
  query.bind(1, product.getId());
  query.bind(2, to_iso_string(from));
  query.bind(3, to_iso_string(to));
  std::vector<Record> records;
  try {
    while (query.executeStep()) {
      std::string dateTimeText = query.getColumn(0).getString();
      time_point dateTime = from_iso_string(dateTimeText);
      int price = query.getColumn(1).getInt();
      records.emplace_back(dateTime, price);
    }
    return records;
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to get records: " + std::string(e.what()));
  }
}

int DBConnector::getLatestRecordPrice(const Product& p_product) {
  try {
    SQLite::Statement query(
        *m_database,
        "SELECT price FROM PriceRecord WHERE product_id = ? ORDER BY date_time "
        "DESC LIMIT 1");
    query.bind(1, p_product.getId());

    if (query.executeStep()) {
      return query.getColumn(0).getInt();
    } else {
      throw std::runtime_error("No Record found");
    }
  } catch (const SQLite::Exception& e) {
    throw e;
  }
}

int DBConnector::verifyCredentials(const Account& account) {
  SQLite::Statement query(
      *m_database,
      "SELECT id FROM Account WHERE username = ? AND password = ?");
  query.bind(1, account.username);
  query.bind(2, account.password);

  if (query.executeStep()) {
    return query.getColumn(0).getInt();
  } else {
    throw IncorrectPassword(std::string(account.username) +
                            " Invalid Username or Password");
  }
}

std::string DBConnector::addToken(int user_id, const std::string& token) {
  SQLite::Statement query(*m_database,
                          "UPDATE Account "
                          "SET token = ? "
                          "WHERE id = ?;");
  query.bind(1, token);
  query.bind(2, user_id);
  try {
    query.exec();
    return token;
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("Failed to add token: " + std::string(e.what()));
  }
}

void DBConnector::removeToken(const std::string& p_token) {
  SQLite::Statement query(*m_database,
                          "UPDATE Account "
                          "SET token = NULL "
                          "WHERE token = ?;");
  query.bind(1, p_token);
  try {
    query.exec();
    int changes = m_database->getChanges();
    if (changes <= 0) {
      throw InvalidToken("token not valid");
    }
  } catch (const SQLite::Exception& e) {
    throw std::runtime_error("" + std::string(e.what()));
  }
}

User DBConnector::getUserByToken(const std::string& p_token) {
  SQLite::Statement query(*m_database,
                          "SELECT User.id, name, balance FROM User "
                          "JOIN Account ON User.id = Account.id "
                          "WHERE Account.token = ?");
  query.bind(1, p_token);
  if (query.executeStep()) {
    int id = query.getColumn(0).getInt();
    std::string name = query.getColumn(1).getText();
    int balance = query.getColumn(2).getInt();
    return User(id, name, balance);
  } else {
    throw InvalidToken("Token not valid");
  }
}

}  // namespace ProjectStockMarket
