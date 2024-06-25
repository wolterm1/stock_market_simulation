from pydantic import BaseModel


class LoginPayload(BaseModel):
    user_name: str
    password: str


class RegisterPayload(BaseModel):
    user_name: str
    password: str


class AmountPayload(BaseModel):
    amount: int
