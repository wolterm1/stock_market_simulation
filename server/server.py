from fastapi import FastAPI, HTTPException, Depends
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from pydantic import BaseModel

from typing import Annotated

from models import User, InventoryEntry, ProductEntry, LoginToken, ProductNotFound
from payloads import LoginPayload, RegisterPayload, AmountPayload

from auth import authenticate_user, register, logout, UserNotFound, IncorrectPassword  # type: ignore
from market_logic import get_user, get_product, get_products, buy_product, sell_product  # type: ignore

app = FastAPI()


oauth2_scheme = OAuth2PasswordBearer(tokenUrl="login")


async def get_current_user(token: Annotated[str, Depends(oauth2_scheme)]) -> User:
    try:
        user = get_user(token)
    except UserNotFound as e:
        raise HTTPException(status_code=401, detail=f"User not found: {e}")

    return User(**user)


async def get_product_entry(product_id: int) -> ProductEntry:
    try:
        product = get_product(product_id)
    except ProductNotFound as e:
        raise HTTPException(status_code=404, detail=f"Product not found: {e}")

    return ProductEntry(**product)


@app.get("/", include_in_schema=False)
async def index() -> dict[str, str]:
    return {"message": "Hello World"}


@app.post("/login")
async def authenticate_user(
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()]
) -> LoginToken:
    try:
        token = authenticate_user(form_data.username, form_data.password)
    except IncorrectPassword as e:
        raise HTTPException(status_code=401, detail=f"Incorrect username or password")

    return LoginToken(token=token)


@app.post("/register")
async def register(
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()]
) -> LoginToken:
    register(form_data.username, form_data.password)
    token = authenticate_user(form_data.username, form_data.password)
    return LoginToken(token=token)


@app.put("/logout")
async def logout(current_user: Annotated[User, Depends(get_current_user)]) -> None:
    pass


@app.get("/user")
async def get_user(current_user: Annotated[User, Depends(get_current_user)]) -> User:
    pass


@app.get("/product/{product_id}")
async def get_product(
    product_id: Annotated[ProductEntry, Depends(get_product_entry)]
) -> ProductEntry:
    pass


@app.get("/products")
async def get_products() -> list[ProductEntry]:
    pass


@app.put("/product/{product_id}/buy")
async def buy_product(
    product_id: Annotated[ProductEntry, Depends(get_product_entry)],
    payload: AmountPayload,
) -> None:
    raise HTTPException(status_code=403, detail="Not enough money")


@app.put("/product/{product_id}/sell")
async def sell_product(
    product_id: Annotated[ProductEntry, Depends(get_product_entry)],
    payload: AmountPayload,
) -> None:
    raise HTTPException(status_code=403, detail="Not enough products to sell")
