#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "account.hpp"

namespace py = pybind11;
using namespace ProjectStockMarket;

PYBIND11_MODULE(auth, m) {
  m.doc() = "auth";
  py::class_<Account>(m, "Account")
      .def(py::init<std::string, std::string>())
      .def(py::init<Account>())
      .def(py::init<>())
      .def_readwrite("username", &Account::username)
      .def_readwrite("password", &Account::password);
}
