from fastapi import FastAPI, HTTPException, Depends, Request
from fastapi.responses import JSONResponse
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from pydantic import BaseModel

from typing import Annotated

from models import UserModel, ProductModel, Token
from payloads import AmountPayload
from exception_handlers import (
    invalid_token_handler,
    product_not_found_handler,
    not_enough_money_handler,
    out_of_stock_handler,
    incorrect_password_handler,
)

from _auth import users, authenticate_user, register, logout, find_user_by_token, InvalidToken, IncorrectPassword  # type: ignore
from _market_logic import DBUser, get_db_user, get_product, get_products, OutOfStock, NotEnoughMoney, ProductNotFound  # type: ignore

app = FastAPI()

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="login")


async def get_current_user_id(token: Annotated[str, Depends(oauth2_scheme)]) -> int:
    user_id = find_user_by_token(token)
    return user_id


async def get_current_user(
    user_id: Annotated[int, Depends(get_current_user_id)]
) -> UserModel:
    user = get_db_user(user_id)
    return UserModel(
        user_id=user.id, user_name=user.name, money=user.money, inventory=user.inventory
    )


async def get_current_db_user(
    user_id: Annotated[int, Depends(get_current_user_id)]
) -> DBUser:
    db_user = get_db_user(user_id)
    return db_user


async def get_product_entry(product_id: int) -> ProductModel:
    product = get_product(product_id)
    return ProductModel(
        product_id=product.id, product_name=product.name, current_value=product.value
    )


app.exception_handlers = {
    InvalidToken: invalid_token_handler,
    ProductNotFound: product_not_found_handler,
    NotEnoughMoney: not_enough_money_handler,
    OutOfStock: out_of_stock_handler,
    IncorrectPassword: incorrect_password_handler,
}


@app.get("/", include_in_schema=False)
async def index_() -> dict[str, str]:
    return {"message": "Hello World"}


@app.post("/login")
async def authenticate_user_(
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()]
) -> Token:
    token = authenticate_user(form_data.username, form_data.password)

    return Token(access_token=token)


@app.post("/register", status_code=201)
async def register_(
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()]
) -> Token:
    register(form_data.username, form_data.password)
    token = authenticate_user(form_data.username, form_data.password)
    return Token(access_token=token)


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
    return [
        ProductModel(
            product_id=product.id,
            product_name=product.name,
            current_value=product.value,
        )
        for product in get_products()
    ]


@app.put("/product/{product_id}/buy", status_code=204)
async def buy_product_(
    user: Annotated[DBUser, Depends(get_current_db_user)],
    product_id: Annotated[ProductModel, Depends(get_product_entry)],
    payload: AmountPayload,
) -> None:
    user.buy_product(product_id, payload.amount)


@app.put("/product/{product_id}/sell", status_code=204)
async def sell_product_(
    user: Annotated[DBUser, Depends(get_current_db_user)],
    product_id: Annotated[ProductModel, Depends(get_product_entry)],
    payload: AmountPayload,
) -> None:
    user.sell_product(product_id, payload.amount)


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, port=8080)
