from dataclasses import dataclass

from .product import Product


@dataclass
class MarketItem:
    """ """

    product: Product
    quantity: int


Supply = dict[int, int]


@dataclass
class Market:
    """ """

    supply: Supply
