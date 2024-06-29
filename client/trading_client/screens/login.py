from httpx import ConnectError
from textual import events, log, on, work
from textual.app import App, ComposeResult
from textual.containers import (
    Horizontal,
    Vertical,
)
from textual.reactive import reactive
from textual.screen import Screen
from textual.widgets import Button, Input, Label
from trading_client.api.exceptions import IncorrectCredentials, UserAlreadyExists

from trading_client.utils import AppType, catch_and_notify


class LoginScreen(AppType, Screen):
    """ """

    username = ""
    password = ""

    def compose(self) -> ComposeResult:
        """ """
        yield Label("Login or Register")
        with Vertical():
            yield Input(placeholder="Username", name="username", id="username-input")
            yield Input(
                placeholder="Password",
                name="password",
                id="password-input",
                password=True,
            )

        with Horizontal():
            yield Button("Login", name="login", id="login-button")
            yield Button("Register", name="register", id="register-button")

    @on(Input.Changed, "#username-input")
    async def username_changed(self, event: Input.Changed):
        self.username = event.value

    @on(Input.Changed, "#password-input")
    async def password_changed(self, event: Input.Changed):
        self.password = event.value

    @on(Button.Pressed, "#login-button")
    @catch_and_notify([ConnectError, IncorrectCredentials])
    async def login(self, event: Button.Pressed):
        await self.app.api.login(self.username, self.password)
        self.dismiss()

    @on(Button.Pressed, "#register-button")
    @catch_and_notify([ConnectError, UserAlreadyExists])
    async def register(self, event: Button.Pressed):
        await self.app.api.register(self.username, self.password)
        self.dismiss()
