#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "account.hpp"
#include "db_connector.hpp"
#include "market_place.hpp"
#include "product.hpp"
#include "product_entry.hpp"
#include "record.hpp"
#include "user.hpp"

namespace py = pybind11;
using namespace ProjectStockMarket;

PYBIND11_MODULE(market_logic, m) {
  m.doc() = "market_logic";
  py::class_<Record>(m, "Record")
      .def(py::init<int, int>())
      .def(py::init<int>())
      .def(py::init<>())
      .def_readwrite("datetime", &Record::dateTime)
      .def_readwrite("value", &Record::price);

  py::class_<Product>(m, "Product")
      .def(py::init<std::string, int>())
      .def(py::init<std::string>())
      .def(py::init<>())
      .def_property_readonly("name", &Product::getName)
      .def("get_id", &Product::getId)
      .def("get_records", &Product::getAllRecords);

  py::class_<ProductEntry>(m, "ProductEntry")
      .def(py::init<Product, int>())
      .def_readwrite("product", &ProductEntry::product)
      .def_readwrite("amount", &ProductEntry::count);

  py::class_<User>(m, "User")
      .def(py::init<Account, std::string, int>())
      .def(py::init<>())
      .def("get_id", &User::getId)
      .def("get_name", &User::getName)
      .def("buy_product", &User::buyProduct)
      .def("sell_product", &User::sellProduct)
      .def("get_balance", &User::getBalance)
      .def("get_inventory", &User::getInventory);

  py::class_<MarketPlace>(m, "MarketPlace")
      .def(py::init<int>())
      .def("get_inventory", &MarketPlace::getInventory)
      .def("get_all_products", &MarketPlace::getAllProduct);

  // hier kein "&" vor DBConnector weil statische Funktionen ka, ob das klappt .
  // wenn irgendwas bricht dann wahrscheinlich hier
  m.def("get_product", DBConnector::getProduct);
  m.def("get_user", DBConnector::getUser);
}
