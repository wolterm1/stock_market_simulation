from typing import TYPE_CHECKING, cast

if TYPE_CHECKING:
    from trading_client.__main__ import TradingApp
    from textual.widget import Widget


class AppType:
    """Mixin for typing the App instance."""

    app: "TradingApp"
