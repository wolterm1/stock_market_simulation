# Stock Mechanics

## Installation

```bash
# clone repository
git clone https://gitlab.informatik.uni-bonn.de/gamma-jmt-group/projectstockmarket.git

# Cmake Installation
cmake -S stock_mechanics -B stock_mechanics/build
cmake --build stock_mechanics/build

# install pybindings
cmake --install stock_mechanics/build

pip install -e server
pip install -e client

#start server
python3 -m server/trading_server
#start client
python3 -m client/trading_client

```
