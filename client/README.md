# Trading Client

## Installation
```bash
# clone project
git clone https://gitlab.informatik.uni-bonn.de/gamma-jmt-group/projectstockmarket.git
# install as editable module with your favorite python version (tested with 3.12.2)
pip install -e client
# run the client
python trading_client
# run in development mode
python -m textual_dev run trading_client.__main__:TradingApp --dev
```