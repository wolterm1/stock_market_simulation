#include <exception>
#include <string>

namespace ProjectStockMarket {

class UserNotFound : public std::exception {
 public:
  UserNotFound(const std::string s);

 private:
  std::string m_message;
};

class ProductNotFound : public std::exception {
 public:
  ProductNotFound(const std::string s);

 private:
  std::string m_message;
};

class NotEnoughMoney : public std::exception {
 public:
  NotEnoughMoney(const std::string s);

 private:
  std::string m_message;
};

class OutOfStock : public std::exception {
 public:
  OutOfStock(const std::string s);

 private:
  std::string m_message;
};

class IncorrectPassword : public std::exception {
 public:
  IncorrectPassword(const std::string s);

 private:
  std::string m_message;
};

class InvalidToken : public std::exception {
 public:
  InvalidToken(const std::string s);

 private:
  std::string m_message;
};

}  // namespace ProjectStockMarket
