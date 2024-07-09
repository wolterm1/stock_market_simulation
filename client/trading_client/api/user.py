from dataclasses import dataclass

from .product import Product


@dataclass
class InventoryItem:
    """ """
    product: Product
    quantity: int


@dataclass
class User:
    """ """
    user_id: int
    user_name: str
    balance: int
    inventory: list[InventoryItem]
