#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "account.hpp"
#include "authenticator.hpp"

namespace py = pybind11;
namespace sm = ProjectStockMarket;

PYBIND11_MODULE(auth, m) {
  m.doc() = "auth";
  py::class_<sm::Account>(m, "Account")
      .def(py::init<std::string, std::string>())
      .def_readwrite("username", &sm::Account::username)
      .def_readwrite("password", &sm::Account::password);
  m.def("authenticate_user", &sm::Authenticator::login);
  m.def("find_user_by_token", &sm::Authenticator::findUserByToken);
  m.def("logout", &sm::Authenticator::logout);
  m.def("register", &sm::Authenticator::registerAccount);
}
