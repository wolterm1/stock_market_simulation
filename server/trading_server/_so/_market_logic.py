"""THIS IS JUST A DEBUG FILE GOING TO BE REPLACED BY A .SO LIBRARY"""

from collections import deque
from dataclasses import dataclass
from typing import Generator, TypedDict
from datetime import datetime, timedelta
import random
import asyncio


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


products = [
    Product(id=1, name="Chocolate"),
    Product(id=2, name="Schmutz"),
    Product(id=3, name="Reis"),
]


def get_product(product_id: int) -> Product:
    # Returns a dictionary with product details
    return products[product_id - 1]


def get_products() -> list[Product]:
    # (id, name)
    return products


def get_market() -> list[tuple[int, int]]:
    # (product, quantity)
    return list(zip((product.id for product in get_products()), [10, 20, 30]))


def generate_records(
    start_value=None, num_records=3600, max_price_change=10
) -> list[tuple[datetime, int]]:
    if start_value is None:
        start_value = random.randint(100, 200)

    records: list[tuple[datetime, int]] = []
    start_time = datetime.now().replace(microsecond=0) - timedelta(
        seconds=num_records / 2
    )
    current_price = start_value

    for _ in range(num_records):
        # Append the current record
        records.append((start_time, current_price))

        # Increment the time by 1 second for the next record
        start_time += timedelta(seconds=1)

        # Randomly change the price by up to `max_price_change` in either direction
        price_change = random.randint(-max_price_change, max_price_change)
        current_price += price_change

    return records


def new_records(
    start_value=None, max_price_change=10
) -> Generator[tuple[datetime, int], None, None]:
    if start_value is None:
        start_value = random.randint(100, 200)

    time = datetime.now().replace(microsecond=0)
    current_price = start_value

    while True:
        # Append the current record
        yield (time, current_price)

        # Increment the time by 1 second for the next record
        time += timedelta(seconds=1)

        # Randomly change the price by up to `max_price_change` in either direction
        price_change = random.randint(-max_price_change, max_price_change)
        current_price += price_change


records = {product.id: generate_records() for product in get_products()}

# write a asyncio script that will generate new records every second and save it globally

records_db: deque[tuple[int, datetime, int]] = deque([], maxlen=10000)


async def generate_new_records(product_id: int):
    global records_db
    current_price = 150
    while True:
        time = datetime.now()
        records_db.append((product_id, time, current_price))
        price_change = random.randint(-10, 10)
        current_price += price_change
        await asyncio.sleep(1)


# def get_records(
#     product_id: int, from_: datetime, to_: datetime
# ) -> list[tuple[datetime, int]]:
#     return [
#         (date, value) for date, value in records[product_id] if from_ <= date <= to_
#     ]


def get_records(
    product_id: int, from_: datetime, to_: datetime
) -> list[tuple[datetime, int]]:
    return [
        (date, value)
        for product, date, value in records_db
        if product == product_id and from_ <= date <= to_
    ]


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
