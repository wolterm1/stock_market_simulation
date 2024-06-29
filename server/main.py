from fastapi import FastAPI, HTTPException, Depends, Request
from fastapi.responses import JSONResponse
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from pydantic import BaseModel

from typing import Annotated

from server.models import InventoryItemModel, UserModel, ProductModel, Token
from server.payloads import AmountPayload
from server.exception_handlers import (
    invalid_token_handler,
    product_not_found_handler,
    not_enough_money_handler,
    out_of_stock_handler,
    incorrect_password_handler,
)

from logging import getLogger

from server._so._auth import authenticate_user, register, logout, find_user_by_token, InvalidToken, IncorrectPassword  # type: ignore
from server._so._market_logic import User, get_user, get_product, get_products, OutOfStock, NotEnoughMoney, ProductNotFound  # type: ignore

app = FastAPI()
logger = getLogger("uvicorn")

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="login")


async def get_current_user_id(token: Annotated[str, Depends(oauth2_scheme)]) -> int:
    """Find the user id by the token

    Args:
        token (Annotated[str, Depends): Dependency that gets the token from the request

    Returns:
        int: id of the user sending the Request
    """
    user_id = find_user_by_token(token)
    return user_id


async def get_current_user_model(
    user_id: Annotated[int, Depends(get_current_user_id)]
) -> UserModel:
    """Get the current user as a UserModel

    Args:
        user_id (Annotated[int, Depends): Dependency that gets the user id

    Returns:
        UserModel: Model describing the current user
    """
    user = get_user(user_id)
    inventory = [
        InventoryItemModel(product_id=item["product_id"], quantity=item["quantity"])
        for item in user.inventory
    ]
    return UserModel(
        user_id=user.id, user_name=user.name, money=user.money, inventory=inventory
    )


async def get_current_user(
    user_id: Annotated[int, Depends(get_current_user_id)]
) -> User:
    """Find the current user in the database

    Args:
        user_id (Annotated[int, Depends): Dependency that gets the user id

    Returns:
        User: User object representing the current user
    """
    db_user = get_user(user_id)
    return db_user


async def get_product_entry(product_id: int) -> ProductModel:
    """Searches the database for the product

    Args:
        product_id (int): product_id to search for

    Returns:
        ProductModel: Model representing the product
    """
    product = get_product(product_id)
    return ProductModel(
        product_id=product.id, product_name=product.name, current_value=product.value
    )


# Registering all exception handlers
app.exception_handlers.update(
    {
        InvalidToken: invalid_token_handler,
        ProductNotFound: product_not_found_handler,
        NotEnoughMoney: not_enough_money_handler,
        OutOfStock: out_of_stock_handler,
        IncorrectPassword: incorrect_password_handler,
    }
)


@app.get("/", include_in_schema=False)
async def index_() -> dict[str, str]:
    return {"message": "Hello World"}


@app.post(
    "/login",
    description="Generates a token used for authentication if the users credentials match.",
    responses={401: {"description": "Incorrect username or password"}},
)
async def login_(form_data: Annotated[OAuth2PasswordRequestForm, Depends()]) -> Token:
    token = authenticate_user(form_data.username, form_data.password)

    return Token(access_token=token)


@app.post("/register", status_code=201)
async def register_(
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()]
) -> Token:
    register(form_data.username, form_data.password)
    token = authenticate_user(form_data.username, form_data.password)
    return Token(access_token=token)


@app.put("/logout", responses={401: {"description": "Invalid Token"}})
async def logout_(token: Annotated[str, Depends(oauth2_scheme)]) -> None:
    logout(token)


@app.get("/user", responses={401: {"description": "Invalid Token"}})
async def get_authenticated_user_(
    current_user: Annotated[UserModel, Depends(get_current_user_model)]
) -> UserModel:
    return current_user


@app.get("/product/{product_id}", responses={404: {"description": "Product not found"}})
async def get_product_by_id_(
    product: Annotated[ProductModel, Depends(get_product_entry)]
) -> ProductModel:
    return product


@app.get("/products")
async def get_all_products_on_market_() -> list[ProductModel]:
    return [
        ProductModel(
            product_id=product.id,
            product_name=product.name,
            current_value=product.value,
        )
        for product in get_products()
    ]


@app.put(
    "/product/{product_id}/buy",
    status_code=204,
    responses={
        401: {"description": "Invalid Token"},
        400: {"description": "Transaction failed"},
    },
)
async def buy_product_(
    user: Annotated[User, Depends(get_current_user)],
    product: Annotated[ProductModel, Depends(get_product_entry)],
    payload: AmountPayload,
) -> None:
    user.buy_product(product.product_id, payload.amount)


@app.put(
    "/product/{product_id}/sell",
    status_code=204,
    responses={
        401: {"description": "Invalid Token"},
        400: {"description": "Transaction failed"},
    },
)
async def sell_product_(
    user: Annotated[User, Depends(get_current_user)],
    product: Annotated[ProductModel, Depends(get_product_entry)],
    payload: AmountPayload,
) -> None:
    user.sell_product(product.product_id, payload.amount)


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, port=8080)
