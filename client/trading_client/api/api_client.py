from datetime import datetime
from functools import wraps
from re import T
import traceback
import httpx

import asyncio
from dataclasses import dataclass
from typing import (
    AsyncGenerator,
    Callable,
    Generator,
    Iterable,
    Optional,
    Type,
    TypedDict,
)

import random
import logging
import json

from .product import Product, PriceRecord
from .market import Market, MarketItem
from .user import User, InventoryItem
from .responses import (
    MarketItemResponse,
    UserResponse,
    ProductResponse,
    PriceRecordResponse,
)
from .exceptions import (
    IncorrectCredentials,
    UserAlreadyExists,
    InvalidToken,
    ProductNotFound,
    TransactionFailed,
)

from textual import log


# write a decorator function that takes in a list of exceptions catches them and retries the api call. The decorator should be applicable to a class to effect all async functions. It should also take a max_retries function


def retry_on_exception(
    exceptions: Iterable[Type[Exception]], max_retries: int
) -> Callable:
    def decorator(func):
        @wraps(func)
        async def wrapper(*args, **kwargs):
            retries = 0
            while retries < max_retries:
                try:
                    return await func(*args, **kwargs)
                except tuple(exceptions) as e:
                    log.error(traceback.format_exc())
                    log.error(f"Caught exception {str(e)}. Retrying...")
                    retries += 1
                    if retries == max_retries:
                        raise

        return wrapper

    return decorator


class BearerAuth(httpx.Auth):
    """ """

    def __init__(self, token_type, access_token) -> None:
        self.token_type = token_type
        self.access_token = access_token

    def auth_flow(self, request):
        """

        Args:
          request:

        Returns:

        """
        request.headers["Authorization"] = f"{self.token_type} {self.access_token}"
        yield request


class NoAuth(httpx.Auth):
    """ """

    def __init__(self):
        pass


CATCH = [
    httpx.RemoteProtocolError,
    httpx.ConnectError,
    httpx.NetworkError,
    httpx.ProtocolError,
]

MAX_RETRIES = 5


class Cache(TypedDict):
    products: dict[int, Product]


class APIClient:
    """ """

    def __init__(self, base_url="http://localhost:8000") -> None:
        self.client = httpx.AsyncClient(base_url=base_url)  # Use AsyncClient

        self.cache: Cache = {
            "products": {},
        }

    async def login(self, username: str, password: str):
        response = await self.client.post(
            "/login", data={"username": username, "password": password}
        )

        if response.status_code == 401:
            raise IncorrectCredentials(response.json()["message"])
        response.raise_for_status()

        data = response.json()
        self.client.auth = BearerAuth(data["token_type"], data["access_token"])

    async def register(self, username: str, password: str):
        response = await self.client.post(
            "/register", data={"username": username, "password": password}
        )

        if response.status_code == 400:
            raise UserAlreadyExists(response.json()["message"])
        response.raise_for_status()

        data = response.json()
        self.client.auth = BearerAuth(data["token_type"], data["access_token"])

    async def logout(self):
        response = await self.client.put("/logout")

        if response.status_code == 401:
            raise InvalidToken(response.json()["message"])
        response.raise_for_status()

        self.client.auth = NoAuth()

    @retry_on_exception(CATCH, MAX_RETRIES)
    async def get_user(self) -> User:
        response = await self.client.get("/user")

        if response.status_code == 401:
            raise InvalidToken(response.json()["message"])
        response.raise_for_status()

        data: UserResponse = response.json()
        inventory = [
            InventoryItem(await self.get_product(item["product_id"]), item["quantity"])
            for item in data["inventory"]
        ]
        return User(
            user_id=data["user_id"],
            user_name=data["user_name"],
            balance=data["balance"],
            inventory=inventory,
        )

    @retry_on_exception(CATCH, MAX_RETRIES)
    async def get_product(self, product_id: int) -> Product:
        if product_id in self.cache["products"]:
            return self.cache["products"][product_id]

        log.info("sending request for {}".format(product_id))
        response = await self.client.get(f"/product/{product_id}")

        if response.status_code == 404:
            raise ProductNotFound(response.json()["message"])
        response.raise_for_status()

        data: ProductResponse = response.json()
        product = Product(**data)
        self.cache["products"][product_id] = product
        return product

    @retry_on_exception(CATCH, MAX_RETRIES)
    async def get_records(
        self, product_id: int, from_: datetime, to_: datetime | None = None
    ) -> list[PriceRecord]:
        params = {
            "from": from_.isoformat(),
        }
        if to_ is not None:
            params["to"] = to_.isoformat()

        response = await self.client.get(
            f"/product/{product_id}/records",
            params=params,
        )
        response.raise_for_status()
        data: PriceRecordResponse = response.json()
        return [
            PriceRecord(
                date=datetime.fromisoformat(record["date"]), value=record["value"]
            )
            for record in data["records"]
        ]

    @retry_on_exception(CATCH, MAX_RETRIES)
    async def get_products(self) -> list[Product]:
        response = await self.client.get("/products")
        response.raise_for_status()
        data: list[ProductResponse] = response.json()
        return [Product(**product) for product in data]

    @retry_on_exception(CATCH, MAX_RETRIES)
    async def get_market(self) -> Market:
        response = await self.client.get("/market")
        response.raise_for_status()
        data: list[MarketItemResponse] = response.json()
        supply = {item["product_id"]: item["quantity"] for item in data}
        return Market(supply=supply)

    async def buy_product(self, product_id: int, amount: int):
        response = await self.client.post(
            f"/product/{product_id}/buy", json={"amount": amount}
        )

        if response.status_code == 400:
            raise TransactionFailed(response.json()["message"])
        elif response.status_code == 401:
            raise InvalidToken(response.json()["message"])
        response.raise_for_status()

    async def sell_product(self, product_id: int, amount: int):
        response = await self.client.post(
            f"/product/{product_id}/sell", json={"amount": amount}
        )

        if response.status_code == 400:
            raise TransactionFailed(response.json()["message"])
        elif response.status_code == 401:
            raise InvalidToken(response.json()["message"])
        response.raise_for_status()
