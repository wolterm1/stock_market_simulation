#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <account.hpp>
#include <authenticator.hpp>
#include <exception_classes.hpp>

namespace py = pybind11;
namespace sm = ProjectStockMarket;

PYBIND11_MODULE(auth, m) {
  m.doc() = "auth";
  py::class_<sm::Account>(m, "Account")
      .def(py::init<std::string, std::string>())
      .def_readwrite("username", &sm::Account::username)
      .def_readwrite("password", &sm::Account::password);

  py::class_<sm::Authenticator>(m, "Authenticator")
      .def_static("authenticate_user", &sm::Authenticator::login)
      .def_static("register", &sm::Authenticator::registerAccount)
      .def_static("logout", &sm::Authenticator::logout)
      .def_static("findUserByToken", &sm::Authenticator::findUserByToken);

  py::register_exception<sm::IncorrectPassword>(m, "IncorrectPassword");
  py::register_exception<sm::InvalidToken>(m, "InvalidToken");
  py::register_exception<sm::AccountAlreadyExists>(m, "AccountAlreadyExists");
}
