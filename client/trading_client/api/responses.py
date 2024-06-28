from typing import TypedDict


class InventoryItemResponse(TypedDict):
    product_id: int
    quantity: int


class UserResponse(TypedDict):
    user_id: int
    user_name: str
    balance: int
    inventory: list[InventoryItemResponse]


class ProductResponse(TypedDict):
    product_id: int
    product_name: str
    current_value: int
