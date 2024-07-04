# ----------------------------------------------#
# -----Konfiguration des Buildsystems CMake-----#
# ----------------------------------------------#

# Minimale Version des Buildsystems
cmake_minimum_required(VERSION 3.16)

# Name des Projekts
project(PrjectStockMarket)

# ----------------------------------------------#
# -------------------Optionen-------------------#
# ----------------------------------------------#

# Setzte verwendeten C++-Standard auf C++17
set(CMAKE_CXX_STANDARD 17)

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
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
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

# Legt die Variable SRC_FILES an, die alle
# .cpp-Dateien des Projekts benennt,
# die in Verzeichnis src/ liegen.
file(GLOB SRC_FILES
  ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Baue das Programm 'demo1' aus den Quellen
# examples/demo1.cpp und allen Quelldateien in
# SRC_FILES (also alle im Verzeichnis src/)
add_executable(main
  examples/main.cpp
  ${SRC_FILES}
)

# ----------------------------------------------#
# -------------------Pybind---------------------#
# ----------------------------------------------#
add_subdirectory(external/pybind11)
add_subdirectory(external/SQLiteCpp)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/external/SQLiteCpp/include)

# Link libraries
target_link_libraries(main SQLiteCpp)

# Baue pybind11-Schnittstelle

# ----------------------------------------------#
# ---------m----Erzeuge Testprogramme------------#
# ----------------------------------------------#
