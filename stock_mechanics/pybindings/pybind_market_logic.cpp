#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "account.hpp"
#include "db_connector.hpp"
#include "exception_classes.hpp"
#include "market_place.hpp"
#include "product.hpp"
#include "product_entry.hpp"
#include "record.hpp"
#include "user.hpp"

namespace py = pybind11;
namespace sm = ProjectStockMarket;

PYBIND11_MODULE(market_logic, m) {
  m.doc() = "market_logic";
  py::class_<sm::Record>(m, "Record")
      .def(py::init<sm::time_point, int>())
      .def_readwrite("date", &sm::Record::dateTime)
      .def_readwrite("value", &sm::Record::price);

  py::class_<sm::Product>(m, "Product")
      .def(py::init<int, std::string>())
      .def_property_readonly("name", &sm::Product::getName)
      .def_property_readonly("id", &sm::Product::getId)
      .def("get_all_records", &sm::Product::getAllRecords)
      .def("get_records", &sm::Product::getRecords);

  py::class_<sm::ProductEntry>(m, "ProductEntry")
      .def(py::init<sm::Product, int>())
      .def_readwrite("product", &sm::ProductEntry::product)
      .def_readwrite("amount", &sm::ProductEntry::count);

  py::class_<sm::User>(m, "User")
      .def(py::init<int, std::string, int>())
      .def_property_readonly("id", &sm::User::getId)
      .def_property_readonly("name", &sm::User::getName)
      .def_property_readonly("balance", &sm::User::getBalance)
      .def("get_inventory", &sm::User::getInventory)
      .def("buy_product", &sm::User::buyProduct)
      .def("sell_product", &sm::User::sellProduct);

  py::class_<sm::MarketPlace>(m, "MarketPlace")
      .def(py::init<int, bool>())
      .def("get_inventory", &sm::MarketPlace::getInventory)
      .def("get_all_products", &sm::MarketPlace::getAllProducts);

  // hier kein "&" vor DBConnector weil statische Funktionen ka, ob das klappt .
  // wenn irgendwas bricht dann wahrscheinlich hier
  m.def("get_product", sm::DBConnector::getProduct);
  m.def("get_user", sm::DBConnector::getUser);
  m.def("init_database", sm::DBConnector::initDB);

  py::class_<sm::Account>(m, "Account")
      .def(py::init<std::string, std::string>())
      .def_readwrite("username", &sm::Account::username)
      .def_readwrite("password", &sm::Account::password);

  m.def("db_verify_credentials", sm::DBConnector::verifyCredentials);
  m.def("db_register_account", sm::DBConnector::registerAccount);
  m.def("db_add_token", sm::DBConnector::addToken);
  m.def("db_remove_token", sm::DBConnector::removeToken);
  m.def("db_get_user_by_token", sm::DBConnector::getUserByToken);

  // py::class_<sm::ProductNotFound>(m, "ProductNotFound")
  //     .def(py::init<std::string>());

  // py::class_<sm::NotEnoughMoney>(m, "NotEnoughMoney")
  //     .def(py::init<std::string>());

  // py::class_<sm::NotInInventory>(m, "NotInInventory")
  //     .def(py::init<std::string>());

  // py::class_<sm::OutOfStock>(m, "OutOfStock").def(py::init<std::string>());

  // py::class_<sm::IncorrectPassword>(m, "IncorrectPassword")
  //     .def(py::init<std::string>());

  // py::class_<sm::InvalidToken>(m,
  // "InvalidToken").def(py::init<std::string>());

  // py::class_<sm::AccountAlreadyExists>(m, "AccountAlreadyExists")
  //     .def(py::init<std::string>());

  py::register_exception<sm::ProductNotFound>(m, "ProductNotFound");
  py::register_exception<sm::NotEnoughMoney>(m, "NotEnoughMoney");
  py::register_exception<sm::NotInInventory>(m, "NotInInventory");
  py::register_exception<sm::OutOfStock>(m, "OutOfStock");
  py::register_exception<sm::IncorrectPassword>(m, "IncorrectPassword");
  py::register_exception<sm::InvalidToken>(m, "InvalidToken");
  py::register_exception<sm::AccountAlreadyExists>(m, "AccountAlreadyExists");
}