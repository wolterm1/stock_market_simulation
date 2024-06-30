from dataclasses import dataclass

from .product import Product


@dataclass
class MarketItem:
    """ """

    product: Product
    quantity: int


@dataclass
class Market:
    """ """

    supply: list[MarketItem]
