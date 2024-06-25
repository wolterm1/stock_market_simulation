from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

from models import User, InventoryEntry, ProductEntry, LoginToken
from payloads import LoginPayload, RegisterPayload, AmountPayload

from auth import login, register, logout  # type: ignore
from market_logic import get_user, get_product, get_products, buy_product, sell_product  # type: ignore

app = FastAPI()


@app.post("/login")
async def login(payload: LoginPayload) -> LoginToken:
    raise HTTPException(status_code=401, detail="Invalid password")
    pass


@app.post("/register")
async def register(payload: RegisterPayload) -> LoginToken:
    raise HTTPException(status_code=400, detail="User already exists")
    pass


@app.get("/logout")
async def logout() -> None:
    raise HTTPException(status_code=401, detail="Not authorizatized")
    pass


@app.get("/user/{user_id}")
async def get_user(user_id: int) -> User:
    raise HTTPException(status_code=401, detail="Not authorizatized")
    pass


@app.get("/product/{product_id}")
async def get_product(product_id: int) -> ProductEntry:
    raise HTTPException(status_code=404, detail="Product not found")
    pass


@app.get("/products")
async def get_products() -> list[ProductEntry]:
    pass


@app.put("/product/{product_id}/buy")
async def buy_product(payload: AmountPayload) -> None:
    raise HTTPException(status_code=401, detail="Not authorizatized")
    raise HTTPException(status_code=404, detail="Product not found")
    raise HTTPException(status_code=403, detail="Not enough money")
    pass


@app.put("/product/{product_id}/sell")
async def sell_product(payload: AmountPayload) -> None:
    raise HTTPException(status_code=401, detail="Not authorizatized")
    raise HTTPException(status_code=404, detail="Product not found")
    raise HTTPException(status_code=403, detail="Not enough products to sell")
    pass
