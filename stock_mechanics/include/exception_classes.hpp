#include <exception>
#include <string>

namespace ProjectStockMarket {

struct StockMarketException : public std::runtime_error {
  explicit StockMarketException(const std::string& msg)
      : std::runtime_error(msg) {}
  explicit StockMarketException(const char* msg) : std::runtime_error(msg) {}
};

struct UserNotFound : public StockMarketException {
  explicit UserNotFound(const std::string& msg) : StockMarketException(msg) {}
  explicit UserNotFound(const char* msg) : StockMarketException(msg) {}
};

struct ProductNotFound : public StockMarketException {
  explicit ProductNotFound(const std::string& msg)
      : StockMarketException(msg) {}
  explicit ProductNotFound(const char* msg) : StockMarketException(msg) {}
};

struct NotEnoughMoney : public StockMarketException {
  explicit NotEnoughMoney(const std::string& msg) : StockMarketException(msg) {}
  explicit NotEnoughMoney(const char* msg) : StockMarketException(msg) {}
};

struct OutOfStock : public StockMarketException {
  explicit OutOfStock(const std::string& msg) : StockMarketException(msg) {}
  explicit OutOfStock(const char* msg) : StockMarketException(msg) {}
};

struct IncorrectPassword : public StockMarketException {
  explicit IncorrectPassword(const std::string& msg)
      : StockMarketException(msg) {}
  explicit IncorrectPassword(const char* msg) : StockMarketException(msg) {}
};

struct InvalidToken : public StockMarketException {
  explicit InvalidToken(const std::string& msg) : StockMarketException(msg) {}
  explicit InvalidToken(const char* msg) : StockMarketException(msg) {}
};

struct AccountAlreadyExists : public StockMarketException {
  explicit AccountAlreadyExists(const std::string& msg)
      : StockMarketException(msg) {}
  explicit AccountAlreadyExists(const char* msg) : StockMarketException(msg) {}
};

struct NotInInventory : public StockMarketException {
  explicit NotInInventory(const std::string& msg) : StockMarketException(msg) {}
  explicit NotInInventory(const char* msg) : StockMarketException(msg) {}
};

}  // namespace ProjectStockMarket
