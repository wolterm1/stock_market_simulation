from pydantic import BaseModel, Field


class ProductNotFound(Exception):
    pass


class ProductModel(BaseModel):
    """Model that represents a static representation of an actively changing
    product, either in the market or in the user's inventory."""

    product_id: int = Field(examples=[1, 2])
    product_name: str = Field(examples=["Chocolate", "Ice Cream"])
    current_value: int = Field(examples=[100, 200])


class InventoryItem(BaseModel):
    product_id: int = Field(examples=[1, 2])
    quantity: int = Field(examples=[2, 1])


class UserModel(BaseModel):
    user_id: int = Field(examples=[1, 2])
    user_name: str = Field(examples=["Alice", "Bob"])
    money: int = Field(examples=[1000, 2000])
    inventory: list[InventoryItem]


class LoginToken(BaseModel):
    token: str = Field(
        examples=[
            "Bearer 5gxl2.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ"
        ]
    )
