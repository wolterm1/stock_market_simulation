from re import T
from typing import TypedDict


class InventoryItemResponse(TypedDict):
    """ """

    product_id: int
    quantity: int


class UserResponse(TypedDict):
    """ """

    user_id: int
    user_name: str
    balance: int
    inventory: list[InventoryItemResponse]


class ProductResponse(TypedDict):
    """ """

    product_id: int
    product_name: str


class PriceRecordListEntryResponse(TypedDict):
    """ """

    date: str
    value: int


class PriceRecordResponse(TypedDict):
    """ """

    product_id: int
    records: list[PriceRecordListEntryResponse]
    date: str
    value: int
