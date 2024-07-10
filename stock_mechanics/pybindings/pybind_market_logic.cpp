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
namespace sm = ProjectStockMarket;

PYBIND11_MODULE(market_logic, m) {
  m.doc() = "market_logic";
  py::class_<sm::Record>(m, "Record")
      .def(py::init<sm::time_point, int>())
      .def_readwrite("datetime", &sm::Record::dateTime)
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
      .def_readwrite("asm::mount", &sm::ProductEntry::count);

  py::class_<sm::User>(m, "User")
      .def(py::init<int, std::string, int>())
      .def_property_readonly("id", &sm::User::getId)
      .def_property_readonly("name", &sm::User::getName)
      .def_property_readonly("balance", &sm::User::getBalance)
      .def("get_inventory", &sm::User::getInventory)
      .def("buy_product", &sm::User::buyProduct)
      .def("sell_product", &sm::User::sellProduct);

  py::class_<sm::MarketPlace>(m, "MarketPlace")
      .def(py::init<int>())
      .def("get_inventory", &sm::MarketPlace::getInventory)
      .def("get_all_products", &sm::MarketPlace::getAllProducts);

  // hier kein "&" vor DBConnector weil statische Funktionen ka, ob das klappt .
  // wenn irgendwas bricht dann wahrscheinlich hier
  m.def("get_product", sm::DBConnector::getProduct);
  m.def("get_user", sm::DBConnector::getUser);
}
