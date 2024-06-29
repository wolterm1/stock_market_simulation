from typing import TYPE_CHECKING, cast

if TYPE_CHECKING:
    from trading_client.__main__ import TradingApp
    from textual.widget import Widget


class AppType:
    app: "TradingApp"

    # def __init__(self, *args, **kwargs) -> None:
    #     super().__init__(*args, **kwargs)
    #     self.trading_app = cast("TradingApp", self.app)
