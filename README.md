# projectstockmarket

## Installation

### Dependencies: 
- A C++ Compiler
- CMake
- Python + pip
- SQLite: https://www.sqlite.org/

```bash
git clone https://gitlab.informatik.uni-bonn.de/gamma-jmt-group/projectstockmarket.git

cd projectstockmarket

cmake -S stock_mechanics -B stock_mechanics/build
cmake --build stock_mechanics/build

cmake --install stock_mechanics/build

pip install -e server
pip install -e client

```

## Starting Server and Graphical Client

```bash
#start server
python3 -m server/trading_server
#start client
python3 -m client/trading_client
```
