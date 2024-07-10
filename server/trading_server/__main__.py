from datetime import datetime, timedelta
from logging import getLogger
from typing import Annotated

from fastapi import Depends, FastAPI, HTTPException, Query, Request
from fastapi.responses import JSONResponse
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from pydantic import BaseModel
from contextlib import asynccontextmanager
import asyncio

from pathlib import Path

from trading_server.exception_handlers import (
    incorrect_password_handler,
    invalid_token_handler,
    not_enough_money_handler,
    out_of_stock_handler,
    product_not_found_handler,
)
from trading_server.models import (
    InventoryItemModel,
    ProductModel,
    Token,
    UserModel,
    ProductRecordModel,
    ProductRecordsModel,
)
from trading_server.payloads import AmountPayload

try:
    from trading_server.modules.auth import (  # type: ignore
        InvalidToken,
        IncorrectPassword,
        Account,
        authenticate_user,
        find_user_by_token,
        logout,
        register,
    )
    from trading_server.modules.market_logic import (  # type: ignore
        NotEnoughMoney,
        OutOfStock,
        ProductNotFound,
        User,
        Product,
        MarketPlace,
        get_product,
        get_user,
    )

    app = FastAPI()
except ImportError as e:
    raise ImportError(
        "Could not import the shared object file. Please run the build script to \
            build the shared object file."
    ) from e

logger = getLogger("uvicorn")

market = MarketPlace(60 * 60)

oauth2_scheme = OAuth2PasswordBearer(tokenUrl="login")


async def get_current_user(token: Annotated[str, Depends(oauth2_scheme)]) -> User:
    """Find the user by the token

    Args:
        token (Annotated[str, Depends): Dependency that gets the token from the request

    Returns:
        User: User object representing the current user
    """
    user = find_user_by_token(token)
    return user


async def get_current_user_model(
    user: Annotated[User, Depends(get_current_user)]
) -> UserModel:
    """Get the current user as a UserModel

    Args:
        user_id (Annotated[int, Depends): Dependency that gets the user id

    Returns:
        UserModel: Model describing the current user
    """
    inventory = [
        InventoryItemModel(product_id=entry.product.id, quantity=entry.amount)
        for entry in user.get_inventory()
    ]
    return UserModel(
        user_id=user.id,
        user_name=user.name,
        balance=user.balance,
        inventory=inventory,
    )


async def get_market_product(product_id: int) -> Product:
    """Searches the database for the product

    Args:
        product_id (int): product_id to search for

    Returns:
        Product: Model representing the product
    """
    product = get_product(product_id)
    return product


async def get_product_model(
    product: Annotated[Product, Depends(get_market_product)]
) -> ProductModel:
    """Searches the database for the product

    Args:
        product_id (int): product_id to search for

    Returns:
        ProductModel: Model representing the product
    """
    return ProductModel(
        product_id=product.id,
        product_name=product.name,
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
    responses={401: {"description": "Incorrect username or password"}},
    description="""
    Generates a token used for authentication if the users credentials match.
    """,
)
async def login_(form_data: Annotated[OAuth2PasswordRequestForm, Depends()]) -> Token:
    account = Account(form_data.username, form_data.password)
    token = authenticate_user(account)

    return Token(access_token=token)


@app.post(
    "/register",
    status_code=201,
    description="""
    Registers a new user in the system with the given credentials and logins the user.
    """,
)
async def register_(
    form_data: Annotated[OAuth2PasswordRequestForm, Depends()]
) -> Token:
    account = Account(form_data.username, form_data.password)
    register(account, form_data.username)
    token = authenticate_user(account)
    return Token(access_token=token)


@app.put(
    "/logout",
    responses={401: {"description": "Invalid Token"}},
    description="""
    Logs out the current user by invalidating the token.
    """,
)
async def logout_(token: Annotated[str, Depends(oauth2_scheme)]) -> None:
    logout(token)


@app.get(
    "/user",
    responses={401: {"description": "Invalid Token"}},
    description="""
    Get all relevant information about a user, including their inventory.
    """,
)
async def get_authenticated_user_(
    current_user: Annotated[UserModel, Depends(get_current_user_model)]
) -> UserModel:
    return current_user


@app.get(
    "/product/{product_id}",
    responses={404: {"description": "Product not found"}},
    description="""
    Get products information by its id.
    """,
)
async def get_product_by_id_(
    product: Annotated[ProductModel, Depends(get_product_model)]
) -> ProductModel:
    return product


def _10_minutess_ago() -> datetime:
    """Helper Function that generates a datetime of 10 minutes ago from invocation"""
    return datetime.now() - timedelta(minutes=10)


@app.get(
    "/product/{product_id}/records",
    responses={404: {"description": "Product not found"}},
    description="""
    Get all records of the specified product in the given time range.
    """,
)
async def get_product_records_(
    product: Annotated[Product, Depends(get_market_product)],
    from_: Annotated[datetime, Query(alias="from", default_factory=_10_minutess_ago)],
    to_: Annotated[datetime, Query(alias="to", default_factory=datetime.now)],
) -> ProductRecordsModel:
    records = product.get_records(from_, to_)
    if records:
        return ProductRecordsModel(
            product_id=product.id,
            records=[
                ProductRecordModel(date=record.date, value=record.value)
                for record in records
            ],
            start_date=records[0].date,
            end_date=records[-1].date,
        )
    else:
        return ProductRecordsModel(
            product_id=product.id,
            records=[],
            start_date=from_,
            end_date=to_,
        )


@app.get("/products")
async def get_all_products_() -> list[ProductModel]:
    return [
        ProductModel(
            product_id=product.id,
            product_name=product.name,
        )
        for product in market.get_all_products()
    ]


@app.get("/market")
async def get_market_() -> list[InventoryItemModel]:
    return [
        InventoryItemModel(
            product_id=entry.product.id,
            quantity=entry.amount,
        )
        for entry in market.get_inventory()
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
    product: Annotated[Product, Depends(get_market_product)],
    payload: AmountPayload,
) -> None:
    user.buy_product(product, payload.amount)


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
    product: Annotated[Product, Depends(get_market_product)],
    payload: AmountPayload,
) -> None:
    user.sell_product(product, payload.amount)


if __name__ == "__main__":
    import uvicorn
    from pathlib import Path

    log_config_path = Path("trading_server/log_conf.yaml").resolve()

    uvicorn.run(app, port=8080, log_config=str(log_config_path))
