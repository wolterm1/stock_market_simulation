# Stock Mechanics

[1] The .vscode file will only exist, as long as we are working on it to base our codingstyle on google.

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