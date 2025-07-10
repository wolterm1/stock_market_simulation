# projectstockmarket

## Installation

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

## Preview

![Bild0](docs/pictures/picture1.png)
![Bild1](docs/pictures/picture3.png)
