#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>

#include <ctime>
#include <functional>
#include <string>

#include "account.hpp"
#include "product.hpp"
#include "product_entry.hpp"
#include "user.hpp"

namespace ProjectStockMarket {

/**
 * @class DBConnector
 * @brief Handles all database operations for the ProjectStockMarket application.
 */
class DBConnector {
 public:
  /**
   * @brief Constructs a new DBConnector object.
   */
  DBConnector();

  /**
   * @brief Default destructor, SQLiteCpp manages the database connection automatically.
   */
  ~DBConnector() = default;

  /**
   * @brief Adds a new user to the database.
   * If the account is already existing, it throws an error.
   *
   * @param user The user to be added.
   */
  static void addUser(User user);

  /**
   * @brief Updates an existing user in the database.
   * If the User isn't registered it throws an error.
   *
   */
  static void updateUser(User p_user, std::string p_name, int p_balance);

  /**
   * @brief Finds a user by their account information.
   * Throws error if the user was not found.
   *
   * @param account The account information to find the user by.
   * @return The user found.
   */
  static User findUser(Account account);

  /**
   * @brief Gets the user balance from the database.
   * Throws error if the user was not found.
   *
   * @param account The account information to find the user by.
   * @return The user's balance.
   */
  static int getUserBalance(Account account);

  /**
   * @brief Gets the user's name from the database.
   * Throws error if the user was not found.
   *
   * @param account The account information to find the user by.
   * @return The user's name.
   */
  static std::string getUserName(Account account);

  /**
   * @brief Gets the product ID from the database.
   * Throws error if not found.
   *
   * @param product The product to get the ID for.
   * @return The product ID.
   */
  static int getProductId(Product product);

  /**
   * @brief Gets the product price from the database.
   * Throws error if not found.
   *
   * @param product The product to get the price for.
   * @return The product price.
   */
  static int getProductPrice(Product product);

  /**
   * @brief Gets the user ID from the database.
   * Throws error if not found.
   *
   * @param user The user to get the ID for.
   * @return The user ID.
   */
  static int getUserId(User user);

  /**
   * @brief Gets a product by its ID from the database.
   * Throws error if not found.
   *
   * @param product_id The ID of the product to get.
   * @return The product found.
   */
  static Product getProduct(int product_id);

  /**
   * @brief Adds a product entry to the market.
   * Adds it to the product list if not already registered.
   * Adds it to the market new or will increase the count if registered.
   * Adds 1 Initial record with the starting price or with existing price.
   *
   * @param entry The product entry to add.
   */
  static void addProductEntryToMarket(ProductEntry entry);

  /**
   * @brief Removes a product entry from the market.
   *
   * @param entry The product entry to remove.
   */
  static void removeProductEntryFromMarket(ProductEntry entry);

  /**
   * @brief Gets all product entries from the market.
   *
   * @return A vector of all product entries in the market.
   */
  static std::vector<ProductEntry> getAllProductEntriesFromMarket();

  /**
   * @brief Gets all products from the database.
   *
   * @return A vector of all products.
   */
  static std::vector<Product> getAllProducts();
  /**
   * @brief Adds a product entry to a user's inventory.
   *
   * @param entry The product entry to add.
   * @param user The user to add the product entry to.
   */
  static void addProductEntryToInventory(ProductEntry entry, User user);

  /**
   * @brief Removes a product entry from a user's inventory.
   *
   * @param entry The product entry to remove.
   * @param user The user to remove the product entry from.
   */
  static void removeProductEntryFromInventory(ProductEntry entry, User user);

  /**
   * @brief Gets all product entries from a user's inventory.
   *
   * @param user The user to get the product entries for.
   * @return A vector of all product entries in the user's inventory.
   */
  static std::vector<ProductEntry> getAllProductEntriesFromInventory(User user);

  /**
   * @brief Adds a record for a product.
   *
   * @param product The product to add the record for.
   * @param record The record to add.
   */
  static void addRecord(Product product, Record record);

  /**
   * @brief Keeps the latest X records for a product.
   *
   * @param amount The number of latest records to keep.
   * @param product The product to keep the records for.
   */
  static void keepLatestXRecords(Product product, int amount);

  /**
   * @brief Gets the records for a product from oldest to newest.
   *
   * @param product The product to get the records for.
   * @return A vector of records from oldest to newest.
   */
  static std::vector<Record> getRecordsFromOldestToNewest(Product product);

  /**
   * @brief Gets the latest record price for a product.
   *
   * @param product The product to get the latest record price for.
   * @return The latest record price.
   */
  static int getLatestRecordPrice(Product product);

 private:
  /**
   * @brief Creates the necessary tables in the database.
   */
  static void createTables();

  /**
   * @brief Initializes the DBConnector.
   *
   * @return The initialized DBConnector.
   */
  static DBConnector initialize();

  static DBConnector init;             ///< Static instance for initialization.
  static SQLite::Database m_database;  ///< The database connection.
};

}  // namespace ProjectStockMarket
