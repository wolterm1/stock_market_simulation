from pydantic import BaseModel


class ProductEntry(BaseModel):
    """Model that represents a static representation of an actively changing
    product"""

    product_id: int
    product_name: str
    current_value: int


class InventoryEntry(BaseModel):
    product_id: int
    quantity: int


class User(BaseModel):
    user_id: int
    user_name: str
    money: int
    inventory: list[InventoryEntry]


class LoginToken(BaseModel):
    token: str
