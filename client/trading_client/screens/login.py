import httpx
from textual import events, log, on, work
from textual.app import App, ComposeResult
from textual.containers import Horizontal, Vertical, Container, Grid
from textual.reactive import reactive
from textual.screen import Screen
from textual.widgets import Button, Input, Label
from trading_client.screens.trading import TradingScreen
from trading_client.api.exceptions import IncorrectCredentials, UserAlreadyExists

from trading_client.utils import AppType, catch_and_notify


class LoginScreen(AppType, Screen[bool]):
    """ """

    username = ""
    password = ""

    def compose(self) -> ComposeResult:
        """ """
        with Container(id="login-box"):
            yield Label("Login or Register")
            with Vertical():
                yield Input(
                    placeholder="Username", name="username", id="username-input"
                )
                yield Input(
                    placeholder="Password",
                    name="password",
                    id="password-input",
                    password=True,
                )

            with Grid():
                yield Button("Login", name="login", id="login-button")
                yield Button("Register", name="register", id="register-button")

    @on(Input.Changed, "#username-input")
    async def username_changed(self, event: Input.Changed):
        self.username = event.value

    @on(Input.Changed, "#password-input")
    async def password_changed(self, event: Input.Changed):
        self.password = event.value

    @on(Button.Pressed, "#login-button")
    @catch_and_notify([httpx.ConnectError, IncorrectCredentials, httpx.HTTPStatusError])
    async def login(self, event: Button.Pressed):
        await self.app.api.login(self.username, self.password)
        self.app.push_screen(TradingScreen())

    @on(Button.Pressed, "#register-button")
    @catch_and_notify([httpx.ConnectError, UserAlreadyExists, httpx.HTTPStatusError])
    async def register(self, event: Button.Pressed):
        await self.app.api.register(self.username, self.password)
        self.app.push_screen(TradingScreen())
