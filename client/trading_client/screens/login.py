from textual import events, log, on, work
from textual.app import App, ComposeResult
from textual.reactive import reactive
from textual.screen import Screen, ModalScreen
from textual.widgets import Static, Label, Header, Button, Input, LoadingIndicator
from textual.containers import (
    Horizontal,
    Vertical,
    VerticalScroll,
    HorizontalScroll,
    Center,
    Container,
)
from textual.worker import Worker, WorkerState

from trading_client.utils.exception_handler import catch_and_notify

from httpx import ConnectError


class LoginScreen(Screen):

    username = ""
    password = ""

    def compose(self) -> ComposeResult:
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

    @catch_and_notify([ConnectError])
    @on(Button.Pressed, "#login-button")
    async def login(self, event):
        await self.app.api.login(self.username, self.password)
        self.dismiss()

    @on(Button.Pressed, "#register-button")
    @catch_and_notify([ConnectError])
    async def register(self, event):
        await self.app.api.register(self.username, self.password)
        self.dismiss()
