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

from trading_client.screens import TradingScreen, LoginScreen

from trading_client.api.api_client import APIClient


class TradingApp(App):

    api = APIClient()

    @work
    async def on_mount(self):
        await self.push_screen_wait(LoginScreen())

    async def on_unmount(self) -> None:
        await self.api.client.aclose()


if __name__ == "__main__":
    app = TradingApp()
    app.run()
