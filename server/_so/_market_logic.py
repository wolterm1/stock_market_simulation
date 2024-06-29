"""THIS IS JUST A DEBUG FILE GOING TO BE REPLACED BY A .SO LIBRARY"""

from dataclasses import dataclass
from typing import TypedDict
import random


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


mock_products = [
    Product(id=1, name="Chocolate", value=100, amount=4),
    Product(id=2, name="Schmutz", value=5, amount=32),
    Product(id=3, name="Reis", value=8, amount=9),
    Product(id=4, name="Milk", value=3, amount=10),
    Product(id=5, name="Bread", value=2, amount=20),
    Product(id=6, name="Eggs", value=1, amount=12),
]


def gen_mock_products():
    mock_products = [
        Product(id=1, name="Chocolate", value=random.randint(50, 60), amount=4),
        Product(id=2, name="Schmutz", value=random.randint(50, 60), amount=32),
        Product(id=3, name="Reis", value=random.randint(50, 60), amount=9),
        Product(id=4, name="Milk", value=random.randint(50, 60), amount=10),
        Product(id=5, name="Bread", value=random.randint(50, 60), amount=20),
        Product(id=6, name="Eggs", value=random.randint(50, 60), amount=12),
    ]
    return mock_products


def get_product(product_id: int) -> Product:
    # Returns a dictionary with product details
    return gen_mock_products()[product_id - 1]


def get_products() -> list[Product]:
    # (id, name, value, amount)
    return gen_mock_products()


class InventoryItem(TypedDict):
    product_id: int
    quantity: int


@dataclass
class User:

    id: int
    name: str
    money: int
    inventory: list[InventoryItem]
    test = "w"

    def buy_product(self, product_id: int, amount: int):
        raise NotEnoughMoney("Not enough money!!!!")

    def sell_product(self, product_id: int, amount: int):
        return True


def get_user(user_id: int) -> User:
    # Returns a fake user object from the database
    return User(
        id=user_id,
        name=f"user{user_id}",
        money=1000,
        inventory=[InventoryItem(product_id=1, quantity=2)],
    )
