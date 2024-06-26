"""THIS IS JUST A DEBUG FILE GOING TO BE REPLACED BY A .SO LIBRARY"""

from dataclasses import dataclass
from typing import TypedDict


class NotEnoughMoney(Exception):
    pass


class OutOfStock(Exception):
    pass


class ProductNotFound(Exception):
    pass


@dataclass
class Product:
    id: int
    name: str
    value: int
    amount: int


def get_product(product_id: int) -> Product:
    # Returns a dictionary with product details
    return Product(id=product_id, name="Chocolate", value=100, amount=4)


def get_products() -> list[Product]:
    # (id, name, value, amount)
    return [
        Product(id=1, name="Chocolate", value=100, amount=4),
        Product(id=2, name="Schmutz", value=5, amount=32),
        Product(id=3, name="Reis", value=8, amount=9),
    ]


# OR (discussion needed)


class InventoryItem(TypedDict):
    product_id: int
    quantity: int


@dataclass
class DBUser:

    id: int
    name: str
    money: int
    inventory: list[InventoryItem]
    test = "w"

    def buy_product(self, product_id: int, amount: int):
        raise NotEnoughMoney("Not enough money!!!!")

    def sell_product(self, product_id: int, amount: int):
        return True


def get_db_user(user_id: int) -> DBUser:
    # Returns a user object from the database
    return DBUser(
        id=1,
        name="user1",
        money=1000,
        inventory=[InventoryItem(product_id=1, quantity=2)],
    )
