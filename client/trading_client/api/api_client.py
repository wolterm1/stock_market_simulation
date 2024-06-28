import httpx

import asyncio
from dataclasses import dataclass
from typing import Optional, TypedDict

import random
import logging
import json

from .exceptions import *
from .product import Product
from .market import Market, MarketItem
from .user import User, InventoryItem
from .responses import UserResponse, ProductResponse

from textual import log


class BearerAuth(httpx.Auth):
    def __init__(self, token_type, access_token) -> None:
        self.token_type = token_type
        self.access_token = access_token

    def auth_flow(self, request):
        request.headers["Authorization"] = f"{self.token_type} {self.access_token}"
        yield request


class APIClient:
    def __init__(self, base_url="http://localhost:8000") -> None:
        self.client = httpx.AsyncClient(base_url=base_url)  # Use AsyncClient

    async def login(self, username: str, password: str) -> str:
        response = await self.client.post(
            "/login", data={"username": username, "password": password}
        )
        response.raise_for_status()
        data = response.json()
        self.client.auth = BearerAuth(data["token_type"], data["access_token"])

    async def register(self, username: str, password: str) -> str:
        response = await self.client.post(
            "/register", data={"username": username, "password": password}
        )
        response.raise_for_status()
        data = response.json()
        self.client.auth = BearerAuth(data["token_type"], data["access_token"])

    async def logout(self):
        response = await self.client.put("/logout")
        response.raise_for_status()
        self.client.auth = None

    async def get_user(self) -> dict:
        response = await self.client.get("/user")
        response.raise_for_status()
        data: UserResponse = response.json()

        data.inventory = tuple(
            InventoryItem(self.get_product(item["product_id"]), item["quantity"])
            for item in data["inventory"]
        )
        return User(**data)

    async def get_product(self, product_id: int) -> Product:
        response = await self.client.get(f"/product/{product_id}")
        response.raise_for_status()
        data: ProductResponse = response.json()
        return Product(**data)

    async def get_market(self) -> Market:
        response = await self.client.get("/products")
        response.raise_for_status()
        data: list[ProductResponse] = response.json()
        supply = tuple(
            MarketItem(self.get_product(item["product_id"]), item["quantity"])
            for item in data
        )
        return Market(supply=supply)

    async def buy_product(self, product_id: int, quantity: int):
        response = await self.client.post(
            f"/product/{product_id}/buy", data={"quantity": quantity}
        )
        response.raise_for_status()

    async def sell_product(self, product_id: int, quantity: int):
        response = await self.client.post(
            f"/product/{product_id}/sell", data={"quantity": quantity}
        )
        response.raise_for_status()
