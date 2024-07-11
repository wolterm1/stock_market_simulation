#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>

#include <ctime>
#include <functional>
#include <memory>
#include <string>

#include "account.hpp"
#include "product.hpp"
#include "product_entry.hpp"
#include "record.hpp"
#include "user.hpp"

namespace ProjectStockMarket {

/**
 * @class DBConnector
 * @brief Handles all database operations for the ProjectStockMarket
 * application.
 */
class DBConnector {
 public:
  /**
   * @brief Initializes the database connection. Needs to be called before
   * using any Database function else we will work on nullptr.
   *
   * @param path The path to the database file. :memory: for in-memory database.
   */
  static void initDB(std::string path);

  /**
   * @brief Adds a trigger to the database that deletes on insert
   * any pricerecord that is over the the limit. Deletes old records first.
   * Each product has its own limit
   *
   * @param limit The limit of records for each product to keep.
   */
  static void addPriceRecordLimitTrigger(int limit);

  /**
   * @brief Default destructor, SQLiteCpp manages the database connection
   * automatically.
   */
  ~DBConnector() = default;

  /**
   * @brief Registers a new user in the database. Also creates the user row with
   * initial balance.
   *
   * @param account The account to register.
   * @param display_name The display name of the user.
   */
  static void registerAccount(const Account& account,
                              const std::string& display_name);

  /**
   * @brief Gets a user from the database.
   * Throws error if the user was not found.
   *
   * @param p_user_id The user_id to get.
   * @return The user found.
   */
  static User getUser(int p_user_id);

  /**
   * @brief Updates an existing user in the database.
   * If the User isn't registered it throws an error.
   *
   * @param p_user The user to update.
   */
  static void updateUser(const User& p_user);

  /**
   * @brief Adds a new Product to the database and market.
   *
   * @param p_product_name The name of the product to add.
   * @param p_count The amount of the product to add.
   *
   * @return The product added.
   */
  static Product addProduct(const std::string& p_product_name, int p_count);

  /**
   * @brief Gets a product by its ID from the database.
   * Throws error if not found.
   *
   * @param product_id The ID of the product to get.
   * @return The product found.
   */
  static Product getProduct(int product_id);

  /**
   * @brief Updates the amount of a product in the market.
   *
   * @param p_product The product to update
   * @param p_change The amount to change the product by.
   */
  static void updateMarketProductEntry(const Product& p_product, int p_change);

  /**
   * @brief Gets all product entries from the market.
   *
   * @return A vector of all product entries in the market.
   */
  static std::vector<ProductEntry> getMarketInventory();

  /**
   * @brief Gets all products from the database.
   *
   * @return A vector of all products.
   */
  static std::vector<Product> getAllProducts();

  /**
   * @brief Updates the amount of a product in a user's inventory.
   *
   * @param p_user The user to update the product for.
   * @param p_product The product to update.
   * @param p_amount The amount to change the product by.
   */
  static void updateUserProductEntry(const User& p_user,
                                     const Product& p_product, int p_amount);

  /**
   * @brief Gets all product entries from a user's inventory.
   *
   * @param user The user to get the product entries for.
   * @return A vector of all product entries in the user's inventory.
   */
  static std::vector<ProductEntry> getUserInventory(User user);

  /**
   * @brief Adds a record for a product.
   *
   * @param product The product to add the record for.
   * @param record The record to add.
   */
  static void addRecord(const Product& p_product, const Record& p_record);

  /**
   * @brief Keeps the latest X records for a product.
   *
   * @param product The product to keep the records for.
   * @param amount The number of latest records to keep.
   */
  static void pruneRecords(Product product, int amount);

  /**
   * @brief Gets the records for a product from oldest to newest.
   *
   * @param product The product to get the records for.
   * @return A vector of records from oldest to newest.
   */
  static std::vector<Record> getAllRecords(const Product& product);

  /**
   * @brief Gets all records for a product in the given time range.
   *
   * @param product THe product to get the records for.
   * @param from The start of the time range.
   * @param to The end of the time range.
   * @return A vector of records in the given time range.
   */
  static std::vector<Record> getRecords(const Product& product,
                                        const time_point& from,
                                        const time_point& to);

  /**
   * @brief Gets the latest record price for a product.
   *
   * @param product The product to get the latest record price for.
   * @return The latest record price, aka the current price.
   */
  static int getLatestRecordPrice(const Product& product);

  /**
   * @brief Checks if the given credentials are valid. Returns the account id if
   * they are. Raises an exception if they are not.
   * @param account The account to verify.
   * @returns The account id if the credentials are valid.
   * @throw std::runtime_error if the credentials are invalid.
   */
  static int verifyCredentials(const Account& account);

  /**
   * @brief Adds a token to the database for a user.
   * @param user_id The user id to add the token for.
   * @param token The token to add.
   * @returns The token added.
   */
  static std::string addToken(int user_id, const std::string& token);

  /**
   * @brief Logs out a user by removing the token from the database.
   * @param p_token The token to remove.
   */
  static void removeToken(const std::string& p_token);

  /**
   * @brief Gets a user by their token.
   * @param p_token The token to get the user by.
   * @returns The user found.
   */
  static User getUserByToken(const std::string& p_token);

 private:
  /**
   * @brief Creates the necessary tables in the database.
   */
  static void createTables();

  /**
   * @brief Pointer to our database connection
   */
  static std::unique_ptr<SQLite::Database>
      m_database;  ///< The database connection.
};

}  // namespace ProjectStockMarket
