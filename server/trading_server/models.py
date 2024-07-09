from pydantic import BaseModel, Field

from datetime import datetime


class ProductModel(BaseModel):
    """Model that represents a Product than is traded in the market."""

    product_id: int = Field(examples=[1, 2])
    product_name: str = Field(examples=["Chocolate", "Ice Cream"])


class ProductRecordModel(BaseModel):
    """Model that represents a record of a product in the market."""

    date: datetime = Field(examples=["2024-6-29T12:00:00", "2024-6-30T12:00:00"])
    value: int = Field(examples=[100, 200])


class ProductRecordsModel(BaseModel):
    """Model that represents a collection of records of a product in the market."""

    product_id: int = Field(examples=[1, 2])
    records: list[ProductRecordModel]
    start_date: datetime = Field(examples=["2024-6-28T12:00:00", "2024-6-29T12:00:00"])
    end_date: datetime = Field(examples=["2024-6-29T12:00:00", "2024-6-30T12:00:00"])


class InventoryItemModel(BaseModel):
    """Model that represents an item and its corresponding amount
    in the user's inventory."""

    product_id: int = Field(examples=[1, 2])
    quantity: int = Field(examples=[2, 1])


class UserModel(BaseModel):
    """Model that represents a User in the trading system."""

    user_id: int = Field(examples=[1, 2])
    user_name: str = Field(examples=["Alice", "Bob"])
    balance: int = Field(examples=[1000, 2000])
    inventory: list[InventoryItemModel]


class Token(BaseModel):
    """Model that represents a token used for authentication."""

    access_token: str = Field(
        examples=[
            "gxl2.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ"
        ]
    )
    token_type: str = Field(default="bearer", examples=["bearer"])
