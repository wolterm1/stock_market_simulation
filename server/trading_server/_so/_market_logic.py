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
    Product(id=1, name="Tetrahydrocannabinol"),
    Product(id=2, name="(5R,6S,9R,13S,14R)-4,5-Epoxy-N-methylmorphinan-7-en-3,6-diol"),
    Product(
        id=3,
        name="Methyl(1R,2R,3S,5S)-3-(benzoyloxy)-8-methyl-8-azabicyclo[3.2.1]octan-2-carboxylat",
    ),
    Product(id=4, name="d-Lysergsäurediethylamid"),
    Product(id=5, name="N-Methylamphetamin"),
    Product(id=6, name="Methamphetamin"),
    Product(id=7, name="N-(1-Phenethyl-4-piperidyl)propionanilid"),
    Product(id=8, name="Distickstoffoxid"),
    Product(id=9, name="Meeresfrüchte"),
    Product(id=10, name="lebendes Rind"),
    Product(id=11, name="M6 Sechskantschrauben"),
    Product(id=12, name="Bleistift Set 2H-6B"),
]


def get_product(product_id: int) -> Product:
    # Returns a dictionary with product details
    return products[product_id - 1]


def get_products() -> list[Product]:
    # (id, name)
    return products


def get_market() -> list[tuple[int, int]]:
    # (product, quantity)
    return [(product.id, random.randint(1, 100)) for product in get_products()]


def generate_records(
    start_value=None, num_records=3600, max_price_change=10
) -> list[tuple[datetime, int]]:
    if start_value is None:
        start_value = random.randint(100, 200)

    records: list[tuple[datetime, int]] = []
    start_time = datetime.now() - timedelta(seconds=num_records / 2)
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

    time = datetime.now()
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

# records_db: deque[tuple[int, datetime, int]] = deque([], maxlen=60*60+3)

records_db: dict[int, deque[tuple[datetime, int]]] = {
    product.id: deque([], maxlen=3600) for product in get_products()
}


def pregenerate_records():
    for product in get_products():
        now = datetime.now() - timedelta(hours=1)
        current_price = random.randint(100, 200)
        for i in range(3600):
            records_db[product.id].append((now, current_price))
            now += timedelta(seconds=1)
            price_change = random.randint(-10, 10)
            current_price += price_change


def get_current_price(product_id: int) -> int:
    # assumes records are sorted by date
    return records_db[product_id][-1][1]


async def generate_new_records(product_id: int):
    global records_db
    current_price = get_current_price(product_id)
    while True:
        time = datetime.now()
        records_db[product_id].append((time, current_price))
        price_change = random.randint(-10, 10)
        current_price += price_change
        await asyncio.sleep(0.5)


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
        (date, value) for date, value in records_db[product_id] if from_ <= date <= to_
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
