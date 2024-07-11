from fastapi import Request
from fastapi.responses import JSONResponse
from trading_server.modules.market_logic import (
    NotEnoughMoney,
    OutOfStock,
    ProductNotFound,
    IncorrectPassword,
    InvalidToken,
    NotInInventory,
    AccountAlreadyExists,
)


async def account_already_exists_handler(request: Request, exc: AccountAlreadyExists):
    return JSONResponse(
        status_code=400,
        content={"message": str(exc)},
    )


async def not_in_inventory_handler(request: Request, exc: NotInInventory):
    return JSONResponse(
        status_code=400,
        content={"message": str(exc)},
    )


async def product_not_found_handler(request: Request, exc: ProductNotFound):
    return JSONResponse(
        status_code=404,
        content={"message": "Product not found"},
    )


async def not_enough_money_handler(request: Request, exc: NotEnoughMoney):
    return JSONResponse(
        status_code=400,
        content={"message": "Not enough money"},
    )


async def out_of_stock_handler(request: Request, exc: OutOfStock):
    return JSONResponse(
        status_code=400,
        content={"message": "Not enough products in stock"},
    )


async def incorrect_password_handler(request: Request, exc: IncorrectPassword):
    return JSONResponse(
        status_code=401,
        content={"message": "Incorrect username or password"},
    )


async def invalid_token_handler(request: Request, exc: InvalidToken):
    return JSONResponse(
        status_code=401,
        content={"message": "Invalid token"},
        headers={"WWW-Authenticate": "Bearer"},
    )
