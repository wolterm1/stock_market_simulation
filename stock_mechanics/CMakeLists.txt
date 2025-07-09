cmake_minimum_required(VERSION 3.24)
project(ProjectStockMarket)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# Lokaler Install-Pfad für alle install()-Befehle (verhindert /usr/local)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/local-install)

include_directories(include)
include_directories(external)

include(FetchContent)

# ------------------ GTEST ------------------ #
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
option(INSTALL_GTEST "Enable installation of googletest." OFF)
include(GoogleTest)
enable_testing()

# ------------------ PYBIND11 ------------------ #
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.12.0
)
FetchContent_MakeAvailable(pybind11)

# ------------------ SQLiteCpp ------------------ #
FetchContent_Declare(
  SQLiteCpp
  GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
  GIT_TAG master
)
set(SQLITECPP_INTERNAL_SQLITE OFF)
FetchContent_MakeAvailable(SQLiteCpp)


# ------------------ SOURCES ------------------ #
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# ------------------ Executable ------------------ #
add_executable(main
  examples/main.cpp
  ${SRC_FILES}
)
target_link_libraries(main
  SQLiteCpp
  sqlite3
  pthread
  dl
)

# ------------------ Python Modul ------------------ #
pybind11_add_module(market_logic
  pybindings/pybind_market_logic.cpp
  ${SRC_FILES}
)
target_link_libraries(market_logic PRIVATE SQLiteCpp sqlite3)

# Lokales Zielverzeichnis beim Installieren
install(TARGETS market_logic
  COMPONENT python
  DESTINATION ${PROJECT_SOURCE_DIR}/../server/trading_server/modules
)

# ------------------ Tests ------------------ #
add_executable(testDatabase tests/test_database.cpp ${SRC_FILES})
target_link_libraries(testDatabase PRIVATE SQLiteCpp GTest::gtest_main)
gtest_discover_tests(testDatabase)

