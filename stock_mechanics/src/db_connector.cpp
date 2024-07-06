#include "db_connector.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace ProjectStockMarket {

SQLite::Database DBConnector::m_database("stockmarket.db",
                                         SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
DBConnector::DBConnector() {
  m_database.exec("PRAGMA busy_timeout = 5000;");
  m_database.exec("PRAGMA journal_mode = WAL;");
  std::cout << " -> database.db Initialized!" << std::endl;
}

DBConnector DBConnector::init = DBConnector::initialize();

// statically initializes the class without having ot have an instance of it
DBConnector DBConnector::initialize() {
  std::cout << "trying to initialize database...";
  createTables();
  return DBConnector();
}

void DBConnector::createTables() {
  try {
    // Account
    m_database.exec(
        "CREATE TABLE IF NOT EXISTS Account ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL UNIQUE, "
        "password TEXT NOT NULL);");
    // User
    m_database.exec(
        "CREATE TABLE IF NOT EXISTS User ("
        "id INTEGER PRIMARY KEY, "
        "name TEXT NOT NULL, "
        "balance INTEGER NOT NULL, "
        "FOREIGN KEY(id) REFERENCES Account(id));");
    // Product
    m_database.exec(
        "CREATE TABLE IF NOT EXISTS Product ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE);");
    // Marketplace
    m_database.exec(
        "CREATE TABLE IF NOT EXISTS Marketplace ("
        "product_id INTEGER, "
        "count INTEGER NOT NULL, "
        "FOREIGN KEY(product_id) REFERENCES Product(id));");
    // Inventory
    m_database.exec(
        "CREATE TABLE IF NOT EXISTS Inventory ("
        "user_id INTEGER, "
        "product_id INTEGER, "
        "count INTEGER NOT NULL, "
        "FOREIGN KEY(user_id) REFERENCES User(id), "
        "FOREIGN KEY(product_id) REFERENCES Product(id), "
        "PRIMARY KEY(user_id, product_id));");
    // PriceRecord
    m_database.exec(
        "CREATE TABLE IF NOT EXISTS PriceRecord ("
        "entry_num INTEGER PRIMARY KEY AUTOINCREMENT,"
        "product_id INTEGER, "
        "date_time INTEGER NOT NULL, "
        "price INTEGER NOT NULL, "
        "FOREIGN KEY(product_id) REFERENCES Product(id));");
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to create tables: " + std::string(e.what()));
  }
}

void DBConnector::addUser(User p_user) {
  try {
    SQLite::Statement check_account_query(m_database, "SELECT id FROM Account WHERE username = ?");
    check_account_query.bind(1, p_user.getAccount().username);
    if (check_account_query.executeStep()) {
      throw std::runtime_error("Username already exists. Choose other username for account.");
    }
    SQLite::Statement account_query(m_database,
                                    "INSERT INTO Account (username, password) VALUES (?, ?)");
    account_query.bind(1, p_user.getAccount().username);
    account_query.bind(2, p_user.getAccount().password);
    account_query.exec();
    int user_id = m_database.getLastInsertRowid();

    SQLite::Statement query(m_database, "INSERT INTO User (id, name, balance) VALUES (?, ?, ?)");
    query.bind(1, user_id);
    query.bind(2, p_user.getName());
    query.bind(3, p_user.getBalance());
    query.exec();

  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to add user: " + std::string(e.what()));
  }
}

int DBConnector::getUserId(User p_user) {
  try {
    SQLite::Statement findAccountId(m_database,
                                    "SELECT id FROM Account WHERE username = ? AND password = ?");
    findAccountId.bind(1, p_user.getAccount().username);
    findAccountId.bind(2, p_user.getAccount().password);
    findAccountId.executeStep();
    return findAccountId.getColumn(0);
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to find user id: " + std::string(e.what()));
  }
}

User DBConnector::findUser(Account p_account) {
  try {
    SQLite::Statement findAccountId(m_database,
                                    "SELECT id FROM Account WHERE username = ? AND password = ?");
    findAccountId.bind(1, p_account.username);
    findAccountId.bind(2, p_account.password);

    if (findAccountId.executeStep()) {
      int acc_id = findAccountId.getColumn(0).getInt();
      SQLite::Statement query(m_database, "SELECT name, balance FROM User WHERE id = ?");
      query.bind(1, acc_id);

      if (query.executeStep()) {
        std::string name = query.getColumn(0).getText();
        int balance = query.getColumn(1).getInt();
        return User(p_account, name, balance);
      } else {
        throw std::runtime_error("User not found for account ID");
      }
    } else {
      throw std::runtime_error("Account not found");
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to find user: " + std::string(e.what()));
  }
}

void DBConnector::updateUser(User p_user, std::string p_name, int p_balance) {
  try {
    SQLite::Statement query(m_database, "UPDATE User SET name = ?, balance = ? WHERE id = ?");
    query.bind(1, p_name);
    query.bind(2, p_balance);
    query.bind(3, p_user.getId());
    query.exec();
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to update user: " + std::string(e.what()));
  }
}

int DBConnector::getUserBalance(Account p_account) {
  try {
    SQLite::Statement findAccountId(m_database,
                                    "SELECT id FROM Account WHERE username = ? AND password = ?");
    findAccountId.bind(1, p_account.username);
    findAccountId.bind(2, p_account.password);

    if (findAccountId.executeStep()) {
      int acc_id = findAccountId.getColumn(0).getInt();
      SQLite::Statement query(m_database, "SELECT name, balance FROM User WHERE id = ?");
      query.bind(1, acc_id);

      if (query.executeStep()) {
        std::string name = query.getColumn(0).getText();
        int balance = query.getColumn(1).getInt();
        return balance;
      } else {
        throw std::runtime_error("User not found for account ID");
      }
    } else {
      throw std::runtime_error("Account not found");
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to find user: " + std::string(e.what()));
  }
}
std::string DBConnector::getUserName(Account p_account) {
  try {
    SQLite::Statement findAccountId(m_database,
                                    "SELECT id FROM Account WHERE username = ? AND password = ?");
    findAccountId.bind(1, p_account.username);
    findAccountId.bind(2, p_account.password);

    if (findAccountId.executeStep()) {
      int acc_id = findAccountId.getColumn(0).getInt();
      SQLite::Statement query(m_database, "SELECT name, balance FROM User WHERE id = ?");
      query.bind(1, acc_id);

      if (query.executeStep()) {
        std::string name = query.getColumn(0).getText();
        int balance = query.getColumn(1).getInt();
        return name;
      } else {
        throw std::runtime_error("User not found for account ID");
      }
    } else {
      throw std::runtime_error("Account not found");
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to find user: " + std::string(e.what()));
  }
}

int DBConnector::getProductId(Product p_product) {
  SQLite::Statement findProductId(m_database, "SELECT id FROM Product WHERE name = ?");
  findProductId.bind(1, p_product.getName());
  if (findProductId.executeStep()) {
    return findProductId.getColumn(0).getInt();
  } else {
    throw std::runtime_error("No id found for product name" + std::to_string(p_product.getId()));
  }
}

int DBConnector::getProductPrice(Product p_product) {
  SQLite::Statement findProductId(
      m_database,
      "SELECT price FROM PriceRecord WHERE product_id = ? ORDER BY entry_num DESC LIMIT 1");
  findProductId.bind(1, p_product.getId());

  if (findProductId.executeStep()) {
    return findProductId.getColumn(0).getInt();
  } else {
    throw std::runtime_error("No price found for product ID " + std::to_string(p_product.getId()));
  }
}

Product DBConnector::getProduct(int p_product_id) {
  SQLite::Statement findProductName(m_database, "SELECT name FROM Product WHERE id = ?");
  findProductName.bind(1, p_product_id);
  findProductName.executeStep();
  return Product(findProductName.getColumn(0).getText());
}

void DBConnector::addProductEntryToMarket(ProductEntry p_entry) {
  try {
    if (p_entry.count < 0) throw std::runtime_error("count cant be below 0");
    SQLite::Statement findProductId(m_database, "SELECT id FROM Product WHERE name = ?");
    findProductId.bind(1, p_entry.product.getName());

    if (findProductId.executeStep()) {
    } else {
      SQLite::Statement registerProduct(m_database, "INSERT INTO Product (name) VALUES (?)");
      registerProduct.bind(1, p_entry.product.getName());
      registerProduct.exec();
    }
    int product_id = p_entry.product.getId();

    SQLite::Statement findProductCount(m_database,
                                       "SELECT count FROM Marketplace WHERE product_id = ?");
    findProductCount.bind(1, product_id);
    int newCount = p_entry.count;
    if (findProductCount.executeStep()) {
      int countToAdd = findProductCount.getColumn(0).getInt();
      newCount += static_cast<int>(countToAdd);

      SQLite::Statement updateProduct(m_database,
                                      "UPDATE Marketplace SET count = ? WHERE product_id = ?");
      updateProduct.bind(1, newCount);
      updateProduct.bind(2, product_id);
      updateProduct.exec();
      Record newRecord(p_entry.product.getPrice());
      addRecord(p_entry.product, newRecord);
    } else {
      SQLite::Statement query(m_database,
                              "INSERT INTO Marketplace (product_id, count) VALUES (?, ?)");
      query.bind(1, product_id);
      query.bind(2, newCount);
      query.exec();
      Record newRecord(p_entry.product.getStartingPrice());
      addRecord(p_entry.product, newRecord);
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to add product to market: " + std::string(e.what()));
  }
}

void DBConnector::removeProductEntryFromMarket(ProductEntry p_entry) {
  if (p_entry.count < 0) throw std::runtime_error("You cant remove negative amount");
  try {
    SQLite::Statement setNewCount(m_database, "SELECT count FROM Marketplace WHERE product_id = ?");
    setNewCount.bind(1, p_entry.product.getId());
    setNewCount.executeStep();
    int newCount = setNewCount.getColumn(0).getInt();
    newCount -= p_entry.count;
    if (newCount < 0) {
      throw std::runtime_error("More entities than available was being tried to remove");
    } else if (newCount == 0) {
      // SQLite::Statement deleteQuery(m_database, "DELETE FROM Marketplace WHERE product_id = ?");
      // deleteQuery.bind(1, entry.getId());
      // deleteQuery.exec();
    }
    SQLite::Statement deleteQuery(m_database,
                                  "UPDATE Marketplace SET count = ? WHERE product_id = ?");
    deleteQuery.bind(1, newCount);
    deleteQuery.bind(2, p_entry.product.getId());
    deleteQuery.exec();

  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to remove product amount from market: " +
                             std::string(e.what()));
  }
}

std::vector<ProductEntry> DBConnector::getAllProductEntriesFromMarket() {
  try {
    std::vector<ProductEntry> productentries;
    SQLite::Statement query(m_database, "SELECT product_id, count FROM Marketplace");
    while (query.executeStep()) {
      int id = query.getColumn(0).getInt();
      int count = query.getColumn(1).getInt();
      productentries.emplace_back(ProductEntry(getProduct(id), count));
    }
    return productentries;
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to get all productentries from market: " +
                             std::string(e.what()));
  }
}

std::vector<Product> DBConnector::getAllProducts() {
  try {
    std::vector<Product> products;
    SQLite::Statement query(m_database, "SELECT id FROM Product");
    while (query.executeStep()) {
      int id = query.getColumn(0).getInt();
      products.emplace_back(getProduct(id));
    }
    return products;
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to get all products: " + std::string(e.what()));
  }
}

void DBConnector::addProductEntryToInventory(ProductEntry p_entry, User p_user) {
  if (p_entry.count <= 0) throw std::runtime_error("count cant be <= 0");
  try {
    int product_id = p_entry.product.getId();
    int user_id = p_user.getId();
    SQLite::Statement checkIfEntryAlreadyExists(
        m_database, "SELECT count FROM Inventory WHERE product_id = ? AND user_id = ?");
    checkIfEntryAlreadyExists.bind(1, product_id);
    checkIfEntryAlreadyExists.bind(2, user_id);
    if (checkIfEntryAlreadyExists.executeStep()) {
      int newCount = checkIfEntryAlreadyExists.getColumn(0).getInt();
      newCount += p_entry.count;
      SQLite::Statement addCount(
          m_database, "UPDATE Inventory SET count = ? WHERE product_id = ? AND user_id = ?");
      addCount.bind(1, newCount);
      addCount.bind(2, product_id);
      addCount.bind(3, user_id);
      addCount.exec();
    } else {
      SQLite::Statement query(
          m_database, "INSERT INTO Inventory (user_id, product_id, count) VALUES (?, ?, ?)");
      query.bind(1, user_id);
      query.bind(2, product_id);
      query.bind(3, p_entry.count);  // Assuming count is 1 for simplicity
      query.exec();
    }

  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to add product to inventory: " + std::string(e.what()));
  }
}

void DBConnector::removeProductEntryFromInventory(ProductEntry p_entry, User p_user) {
  try {
    if (p_entry.count < 0) throw std::runtime_error("count cant be <= 0");
    int product_id = p_entry.product.getId();
    int user_id = p_user.getId();
    SQLite::Statement checkIfEntryAlreadyExists(
        m_database, "SELECT count FROM Inventory WHERE product_id = ? AND user_id = ?");
    checkIfEntryAlreadyExists.bind(1, product_id);
    checkIfEntryAlreadyExists.bind(2, user_id);
    if (checkIfEntryAlreadyExists.executeStep()) {
      int newCount = checkIfEntryAlreadyExists.getColumn(0).getInt();
      newCount -= p_entry.count;
      if (newCount < 0) throw std::runtime_error("cant remove more than available");
      if (newCount == 0) {
        SQLite::Statement deleteQuery(m_database,
                                      "DELETE FROM Inventory WHERE product_id = ? AND user_id =?");
        deleteQuery.bind(1, product_id);
        deleteQuery.bind(2, user_id);
        deleteQuery.exec();
      } else {
        SQLite::Statement addCount(
            m_database, "UPDATE Inventory SET count = ? WHERE product_id = ? AND user_id = ?");
        addCount.bind(1, newCount);
        addCount.bind(2, product_id);
        addCount.bind(3, user_id);
        addCount.exec();
      }
    } else {
      throw std::runtime_error("User doesnt have this product in this inventory");
    }
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to remove product from inventory: " + std::string(e.what()));
  }
}

std::vector<ProductEntry> DBConnector::getAllProductEntriesFromInventory(User p_user) {
  try {
    std::vector<ProductEntry> products;
    SQLite::Statement query(m_database,
                            "SELECT product_id, count FROM Inventory WHERE user_id = ?");
    query.bind(1, p_user.getId());
    std::vector<ProductEntry> product_enries;
    while (query.executeStep()) {
      int id = query.getColumn(0).getInt();
      int count = query.getColumn(1).getInt();
      ProjectStockMarket::ProductEntry entry(getProduct(id), count);
      products.emplace_back(entry);
    }
    return products;
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to get all products from inventory: " + std::string(e.what()));
  }
}

void DBConnector::addRecord(Product p_product, Record p_record) {
  try {
    SQLite::Statement query(
        m_database, "INSERT INTO PriceRecord (product_id, date_time, price) VALUES (?, ?, ?)");
    query.bind(1, p_product.getId());
    query.bind(2, p_record.dateTime);
    query.bind(3, p_record.price);
    query.exec();
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to add record: " + std::string(e.what()));
  }
}

std::vector<Record> DBConnector::getRecordsFromOldestToNewest(Product p_product) {
  try {
    std::vector<Record> records;
    SQLite::Statement query(m_database,
                            "SELECT date_time, price FROM PriceRecord WHERE product_id = ?");
    query.bind(1, p_product.getId());
    while (query.executeStep()) {
      std::time_t date_time = query.getColumn(0).getInt();
      int price = query.getColumn(1).getInt();
      records.emplace_back(Record(date_time, price));
    }
    return records;
  } catch (const std::exception& e) {
    throw std::runtime_error("Failed to get record cache: " + std::string(e.what()));
  }
}
void DBConnector::keepLatestXRecords(Product p_product, int p_amount) {
  try {
    SQLite::Statement query(m_database,
                            "SELECT entry_num FROM PriceRecord WHERE product_id = ? ORDER BY "
                            "date_time DESC, entry_num DESC LIMIT ?");
    query.bind(1, p_product.getId());
    query.bind(2, p_amount);
    std::vector<int> latest_entry_nums;
    while (query.executeStep()) {
      latest_entry_nums.push_back(query.getColumn(0).getInt());
    }
    if (latest_entry_nums.size() < p_amount) {
      return;
    }
    std::string entry_nums_to_keep;
    for (size_t i = 0; i < latest_entry_nums.size(); ++i) {
      entry_nums_to_keep += std::to_string(latest_entry_nums[i]);
      if (i < latest_entry_nums.size() - 1) {
        entry_nums_to_keep += ",";
      }
    }
    std::string delete_query_str =
        "DELETE FROM PriceRecord WHERE product_id = ? AND entry_num NOT IN (" + entry_nums_to_keep +
        ")";
    SQLite::Statement deleteQuery(m_database, delete_query_str);
    deleteQuery.bind(1, p_product.getId());
    deleteQuery.exec();

  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}

int DBConnector::getLatestRecordPrice(Product p_product) {
  try {
    SQLite::Statement query(m_database,
                            "SELECT price FROM PriceRecord WHERE product_id = ? ORDER BY date_time "
                            "DESC, entry_num DESC LIMIT 1");
    query.bind(1, p_product.getId());

    if (query.executeStep()) {
      return query.getColumn(0).getInt();
    } else {
      throw std::runtime_error("No Record found");
    }
  } catch (const std::exception& e) {
    throw e;
  }
}

}  // namespace ProjectStockMarket
