# Stock Mechanics

## Installation

```bash
# clone repository
git clone https://gitlab.informatik.uni-bonn.de/gamma-jmt-group/projectstockmarket.git
cd stock_mechanics

# Cmake Installation
cmake -S . -B build -D CMAKE_INSTALL_PREFIX=./build/local_libs
cmake --build build

# install pybindings
cmake --install build

#start server
pip install -e server
cd server 
python3 -m trading_server

#start client
cd client
pip install -e .
python3 -m trading_client

```
