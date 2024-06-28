from dataclasses import dataclass


@dataclass
class Product:
    product_id: int
    product_name: str
    current_value: int
