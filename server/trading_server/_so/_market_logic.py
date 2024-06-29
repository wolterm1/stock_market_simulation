"""THIS IS JUST A DEBUG FILE GOING TO BE REPLACED BY A .SO LIBRARY"""

from dataclasses import dataclass
from typing import TypedDict
from datetime import datetime, timedelta
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


def get_product(product_id: int) -> Product:
    # Returns a dictionary with product details
    return Product(id=product_id, name="Chocolate")


def get_products() -> list[Product]:
    # (id, name)
    return [
        Product(id=1, name="Chocolate"),
        Product(id=2, name="Schmutz"),
        Product(id=3, name="Reis"),
    ]


def get_market_supply() -> list[tuple[Product, int]]:
    # (product, quantity)
    return list(zip(get_products(), [10, 20, 30]))


def generate_records(
    product_id: int, start_value=None, num_records=3600, max_price_change=10
) -> list[tuple[int, datetime, int]]:
    if start_value is None:
        start_value = random.randint(100, 200)

    records: list[tuple[int, datetime, int]] = []
    start_time = datetime.now() - timedelta(seconds=num_records / 2)
    current_price = start_value

    for _ in range(num_records):
        # Append the current record
        records.append((product_id, start_time, current_price))

        # Increment the time by 1 second for the next record
        start_time += timedelta(seconds=1)

        # Randomly change the price by up to `max_price_change` in either direction
        price_change = random.randint(-max_price_change, max_price_change)
        current_price += price_change

    return records


def get_records(
    product_id: int, from_: datetime, to_: datetime
) -> list[tuple[datetime, int]]:
    filtered_records = [
        (date, value)
        for id, date, value in generate_records(product_id)
        if id == product_id and from_ <= date <= to_
    ]
    return filtered_records


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
