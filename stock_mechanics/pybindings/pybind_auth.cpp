#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "account.hpp"
#include "authenticator.hpp"

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
  m.def("authenticate_user", &Authenticator::login);
  m.def("find_user_by_token", &Authenticator::getUserByToken);
  m.def("logout", &Authenticator::logout);
  m.def("register", &Authenticator::registerAcc);
}
