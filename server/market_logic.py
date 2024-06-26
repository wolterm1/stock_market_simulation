"""THIS IS JUST A DEBUG FILE GOING TO BE REPLACED BY A .SO LIBRARY"""

from dataclasses import dataclass
from typing import TypedDict


class NotEnoughMoney(Exception):
    pass


class OutOfStock(Exception):
    pass


class Product(TypedDict):
    id: int
    name: str
    value: int
    amount: int


def get_product(product_id: int) -> Product:
    # Returns a dictionary with product details
    return {"id": 1, "name": "product1", "value": 100, "amount": 4}


def get_products() -> list[Product]:
    # (id, name, value, amount)
    return [
        {"id": 1, "name": "product1", "value": 100, "amount": 4},
        {"id": 2, "name": "product2", "value": 200, "amount": 8},
        {"id": 3, "name": "product3", "value": 300, "amount": 2},
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
        return True

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
