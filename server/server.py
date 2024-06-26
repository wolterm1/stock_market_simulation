from fastapi import FastAPI, HTTPException, Depends
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from pydantic import BaseModel

from typing import Annotated

from models import UserModel, ProductModel, LoginToken, ProductNotFound
from payloads import AmountPayload

from auth import authenticate_user, register, logout, find_user_by_token, UserNotFound, IncorrectPassword  # type: ignore
from market_logic import DBUser, get_db_user, get_product, get_products, OutOfStock, NotEnoughMoney  # type: ignore

app = FastAPI()


oauth2_scheme = OAuth2PasswordBearer(tokenUrl="login")


async def get_current_user_id(token: Annotated[str, Depends(oauth2_scheme)]) -> int:
    try:
        user_id = find_user_by_token(token)
    except UserNotFound as e:
        raise HTTPException(status_code=401, detail=f"User not found: {e}")

    return user_id


async def get_current_user(
    user_id: Annotated[int, Depends(get_current_user_id)]
) -> UserModel:
    try:
        user = get_db_user(user_id)
    except UserNotFound as e:
        raise HTTPException(status_code=401, detail=f"User not found: {e}")

    return UserModel(
        user_id=user.id, user_name=user.name, money=user.money, inventory=user.inventory
    )


async def get_current_db_user(
    user_id: Annotated[int, Depends(get_current_user_id)]
) -> DBUser:
    try:
        db_user = get_db_user(user_id)
    except UserNotFound as e:
        raise HTTPException(status_code=401, detail=f"User not found: {e}")

    return db_user


async def get_product_entry(product_id: int) -> ProductModel:
    try:
        product = get_product(product_id)
    except ProductNotFound as e:
        raise HTTPException(status_code=404, detail=f"Product not found: {e}")

    return ProductModel(**product)


@app.get("/", include_in_schema=False)
async def index_() -> dict[str, str]:
    return {"message": "Hello World"}


@app.post("/login")
async def authenticate_user_(
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()]
) -> LoginToken:
    try:
        token = authenticate_user(form_data.username, form_data.password)
    except IncorrectPassword as e:
        raise HTTPException(status_code=401, detail=f"Incorrect username or password")

    return LoginToken(token=token)


@app.post("/register", status_code=201)
async def register_(
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()]
) -> LoginToken:
    register(form_data.username, form_data.password)
    token = authenticate_user(form_data.username, form_data.password)
    return LoginToken(token=token)


@app.put("/logout")
async def logout_(
    current_user: Annotated[UserModel, Depends(get_current_user)]
) -> None:
    logout(current_user)


@app.get("/user")
async def get_user_(
    current_user: Annotated[UserModel, Depends(get_current_user)]
) -> UserModel:
    return current_user


@app.get("/product/{product_id}")
async def get_product_(
    product: Annotated[ProductModel, Depends(get_product_entry)]
) -> ProductModel:
    return product


@app.get("/products")
async def get_products_() -> list[ProductModel]:
    return [ProductModel(**product) for product in get_products()]


@app.put("/product/{product_id}/buy", status_code=204)
async def buy_product_(
    user: Annotated[DBUser, Depends(get_current_db_user)],
    product_id: Annotated[ProductModel, Depends(get_product_entry)],
    payload: AmountPayload,
) -> None:
    try:
        user.buy_product(product_id, payload.amount)
    except NotEnoughMoney as e:
        raise HTTPException(status_code=400, detail="Not enough money")
    except OutOfStock as e:
        raise HTTPException(status_code=400, detail="Not enough products in stock")


@app.put("/product/{product_id}/sell", status_code=204)
async def sell_product_(
    user: Annotated[DBUser, Depends(get_current_db_user)],
    product_id: Annotated[ProductModel, Depends(get_product_entry)],
    payload: AmountPayload,
) -> None:
    try:
        user.sell_product(product_id, payload.amount)
    except OutOfStock as e:
        raise HTTPException(status_code=400, detail="Not enough products to sell")
