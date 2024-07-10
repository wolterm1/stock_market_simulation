# ----------------------------------------------#
# -----Konfiguration des Buildsystems CMake-----#
# ----------------------------------------------#

# Minimale Version des Buildsystems
cmake_minimum_required(VERSION 3.16)

# Name des Projekts
project(ProjectStockMarket)

# ----------------------------------------------#
# -------------------Optionen-------------------#
# ----------------------------------------------#

# Setzte verwendeten C++-Standard auf C++17
set(CMAKE_CXX_STANDARD 17)

# 20 wäre schöner, sorgt aber für Probleme mit SQLiteCpp, somehow

# Optional: Baue mit Debugsymbolen / Optimierung
set(CMAKE_BUILD_TYPE Debug)

# Füge Includeverzeichnisse hinzu
include_directories(include)
include_directories(external)

# ----------------------------------------------#
# ----------Download and use GoogleTest---------#
# ----------------------------------------------#

# Use CMakes FetchContent
include(FetchContent)

# Workaround for CMake >= 3.24
# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

# Download GoogleTest (we use version 1.12.1)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Workaround for Windows:
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# GoogleTest soll nur lokal, in diesem Projekt verwendet werden.
option(INSTALL_GTEST "Enable installation of googletest." OFF)

# Include CMake configuration of GoogleTest
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# Enable GoogleTest
enable_testing()

# Blöde unnötige Fehler ignorieren
set(SQLITECPP_RUN_CPPLINT OFF)

# Externe Bibliotheken bauen
add_subdirectory(external/SQLiteCpp)
add_subdirectory(external/pybind11)

# Legt die Variable SRC_FILES an, die alle
# .cpp-Dateien des Projekts benennt,
# die in Verzeichnis src/ liegen.
file(GLOB SRC_FILES
  ${PROJECT_SOURCE_DIR}/src/*.cpp)

# add_executable(authmain
# examples/authmain.cpp
# ${SRC_FILES}
# )
add_executable(main
  examples/main.cpp
  ${SRC_FILES}
)

# ----------------------------------------------#
# -------------------Pybind---------------------#
# ----------------------------------------------#

# Für Projekte, die global gebaut werden wollen, forcen wir den Pfad
# set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/build/")
pybind11_add_module(market_logic
  pybindings/pybind_market_logic.cpp
  ${SRC_FILES}
)

# install(TARGETS market_logic
# COMPONENT python
# DESTINATION "."
# )
#
# install(TARGETS auth
# COMPONENT python
# DESTINATION "."
# )

# ##################################################################
install(TARGETS market_logic
  COMPONENT python
  DESTINATION "${CMAKE_SOURCE_DIR}/../server/trading_server/modules"
)

# Link libraries
target_link_libraries(market_logic PRIVATE SQLiteCpp)
target_link_libraries(main
  SQLiteCpp
  sqlite3
  pthread
  dl
)

# target_link_libraries(testDB SQLiteCpp)
# target_link_libraries(authmain SQLiteCpp)

# ----------------------------------------------#
# ---------m----Erzeuge Testprogramme------------#
# ----------------------------------------------#
add_executable(testDatabase tests/test_database.cpp ${SRC_FILES})
target_link_libraries(testDatabase PRIVATE SQLiteCpp GTest::gtest_main)
gtest_discover_tests(testDatabase)