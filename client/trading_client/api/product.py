from dataclasses import dataclass
from datetime import datetime


@dataclass
class Product:
    """ """

    product_id: int
    product_name: str


@dataclass
class PriceRecord:
    """ """

    date: datetime
    value: int
