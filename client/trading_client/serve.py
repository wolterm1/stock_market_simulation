from textual_serve.server import Server

server = Server(
    "python -m textual_dev run trading_client.__main__:TradingApp --dev", port=8082
)

server.serve()
