from calendar import c
from functools import partial
from itertools import tee
import random
from collections import deque
from datetime import date, datetime, timedelta
from re import T
import re
import time
from tkinter import Y
from typing import Iterable

from rich.text import Text
from rich.style import Style

from click import style
import humanize
from httpx import ConnectError
from textual import on, work
from textual.app import ComposeResult
from textual.css.query import NoMatches
from textual.color import Color
from textual.containers import VerticalScroll, Vertical
from textual.message import Message
from textual.reactive import reactive, var
from textual.screen import ModalScreen, Screen
from textual.widgets import (
    Button,
    Label,
    Static,
    TabbedContent,
    Footer,
)
from textual.worker import Worker, WorkerState
from textual_plotext import PlotextPlot

from trading_client.api.product import PriceRecord
from trading_client.api import Product
from trading_client.api.exceptions import IncorrectCredentials, UserAlreadyExists
from trading_client.utils import AppType, catch_and_notify


class TradingScreen(AppType, Screen):
    """ """

    BINDINGS = [
        ("q", "decrease_time_window()", "Decrease time window"),
        ("e", "increase_time_window()", "Increase time window"),
    ]

    available_time_windows = [
        timedelta(minutes=1),
        timedelta(minutes=5),
        timedelta(minutes=10),
        timedelta(minutes=30),
        timedelta(hours=1),
    ]
    current_time_window_index = var(2)
    time_window = var(timedelta(minutes=10), init=False)

    def compose(self) -> ComposeResult:
        """ """
        with TabbedContent("Market", "Inventory"):
            yield MarketWidget()
            yield InventoryWidget()
        yield Footer()

    def compute_time_window(self) -> timedelta:
        """ """
        return self.available_time_windows[self.current_time_window_index]

    def watch_time_window(self, time_window: timedelta):
        for price_chart in self.query(PriceChart):
            price_chart.time_window = time_window

    def action_increase_time_window(self):
        """ """
        if self.current_time_window_index < len(self.available_time_windows) - 1:
            self.current_time_window_index += 1
            self.app.notify(f"Increased time window to {self.time_window}")

    def action_decrease_time_window(self):
        """ """
        if self.current_time_window_index > 0:
            self.current_time_window_index -= 1
            self.app.notify(f"Decreased time window to {self.time_window}")


class InventoryWidget(AppType, Static):
    """ """

    pass


class MarketWidget(AppType, Static):
    """ """

    product_ids: reactive[list[int]] = reactive(
        [], recompose=True, layout=True, init=False, always_update=True
    )

    def compose(self) -> ComposeResult:
        """ """
        with VerticalScroll():
            for product_id in self.product_ids:
                yield ProductWidget(product_id)

    async def on_mount(self):
        self.product_ids = [
            product.product_id for product in await self.app.api.get_products()
        ]
        await self.recompose()


cool_colors = [
    Color.parse(
        "crimson",
    ),
    Color.parse("darkorange"),
    Color.parse("deepskyblue"),
    Color.parse("firebrick"),
    Color.parse("forestgreen"),
    Color.parse("indigo"),
    Color.parse("lightgreen"),
    Color.parse("lightseagreen"),
    Color.parse("mediumspringgreen"),
    Color.parse("palegreen"),
    Color.parse("seagreen"),
    Color.parse("springgreen"),
    Color.parse("steelblue"),
    Color.parse("cornflowerblue"),
    Color.parse("ivory"),
    Color.parse("khaki"),
    Color.parse("lightcoral"),
]


class ProductWidget(AppType, Static):
    """ """

    MAX_RECORD_LENGTH = 60 * 60

    product_id: int
    product: reactive[Product] = reactive(Product(-1, ""))

    class FetchProductFinished(Message):
        pass

    def __init__(self, product_id: int):
        super().__init__()
        self.product_id = product_id
        self.color = random.choice(cool_colors)
        self.records = deque([], maxlen=self.MAX_RECORD_LENGTH)

    def compose(self) -> ComposeResult:
        """ """
        yield Label(id="product-name")
        yield Label("Your Position")
        yield PriceChart()
        with Vertical():
            yield Button("Buy", name="buy", id="buy-button")
            yield Button("Sell", name="sell", id="sell-button")

    async def on_mount(self):
        self.fetch_product()
        self.record_fetcher = self.set_interval(1, self.fetch_new_records, pause=True)

    @work(name="fetch_product")
    async def fetch_product(self):
        """In the future, this should only fetch the new price and add it to the plot"""
        self.product = await self.app.api.get_product(self.product_id)
        product_name = self.query_one("#product-name", Label)
        product_name.update(self.product.product_name)

        self.post_message(self.FetchProductFinished())

    @on(FetchProductFinished)
    async def on_fetch_product_finished(self, message: FetchProductFinished):
        self.record_fetcher.resume()

    @work(name="fetch_new_records")
    async def fetch_new_records(self):
        """Fetches new records from the API and adds them to the plot."""

        if self.records:
            newest_record_time = self.records[-1].date
        else:
            newest_record_time = datetime.now() - timedelta(hours=1)

        new_records = (
            await self.app.api.get_records(self.product_id, from_=newest_record_time)
        )[1:]

        self.records.extend(new_records)
        price_chart = self.query_one(PriceChart)
        price_chart.records = self.records
        price_chart.replot()


class PriceChart(AppType, PlotextPlot):
    """A Plot that displays a price chart.
    Supports adding records and changing its time window dynamically.

    Args:
      max_num_records(int | None): Maximum number of records to keep saved in a ß
      price chart. If more records are added, the oldest records will be removed. \
    None for no limit.
      time_window(int): Plot all records that are within the last `time_window` delta.
      initial_records(Iterable[record_type] | None, optional): Start with these records

    Returns:

    """

    screen: TradingScreen
    parent: ProductWidget

    time_window = var(timedelta(minutes=1), init=False)
    marker: var[str] = var("braille")

    records: deque[PriceRecord] | None = None

    async def on_mount(self):
        self.time_window = self.screen.time_window
        self.plt.title(
            f"{self.parent.product.product_name} Price Chart - last {humanize.precisedelta(self.time_window)}"
        )
        self.plt.xlabel("Time")
        self.plt.ylabel("Price")
        self.plt.date_form("H:M:S")

        self.replot()

    @work(thread=True, name="replot")
    async def replot(self):
        """Replots the price by clearing all data, adding all records that are within \
        the time window and refreshing the widget."""
        self.plt.clear_data()

        if not self.records:
            self.log.info("No records to plot")
            return

        # filter records that are older than the cutoff_time
        now = datetime.now()
        cutoff_time = now - self.time_window

        # only plot the desired records
        filtered_records = [
            record for record in self.records if record.date >= cutoff_time
        ]

        deltas = [record.date - now for record in filtered_records]
        prices = [record.value for record in filtered_records]

        self.log.info(f"Plotting {len(prices)} records", deltas[0:10])

        # Plot the filtered records
        if not prices or not deltas:
            return

        delta_seconds = [delta.total_seconds() for delta in deltas]

        self.plt.plot(delta_seconds, prices, marker=self.marker)
        self.plt.hline(prices[-1], color="red")

        # calculate xticks dynamically
        # Always 6 ticks, evenly spaced out
        xticks = [float(-x * (len(deltas) // 5)) for x in range(6)]
        # convert xticks to human readable time
        labels = [humanize.naturaltime(int(x)) for x in xticks]
        self.plt.xticks(xticks, labels)

        # calculate yticks
        # always 3 ticks, (hopefully) evenly spaced out
        min_price = min(prices)
        max_price = max(prices)
        yticks = [min_price, (min_price + max_price) // 2, max_price, prices[-1]]
        yticks = [float(tick) for tick in yticks]
        self.plt.yticks(yticks)

        self.refresh()

    async def watch_marker(self):
        """ """
        self.replot()

    async def watch_time_window(self):
        """ """
        self.plt.title(
            f"{self.parent.product.product_name} Price Chart - last {humanize.precisedelta(self.time_window)}"
        )
        self.refresh()
