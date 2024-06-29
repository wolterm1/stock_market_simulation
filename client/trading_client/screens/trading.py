from asyncio import sleep
import random
import time
from typing import Iterable
from httpx import ConnectError
from textual import events, log, on, work
from textual.app import App, ComposeResult
from textual.reactive import reactive, var
from textual.screen import Screen, ModalScreen
from textual.widgets import (
    Static,
    Label,
    Header,
    Button,
    Input,
    LoadingIndicator,
    TabbedContent,
)
from textual.containers import (
    Horizontal,
    Vertical,
    VerticalScroll,
    HorizontalScroll,
    Center,
    Container,
)
from textual.worker import Worker, WorkerState

from textual_plotext import PlotextPlot

from trading_client.utils.exception_handler import catch_and_notify
from trading_client.api.exceptions import IncorrectCredentials, UserAlreadyExists
from trading_client.api.product import Product
from trading_client.utils.mixins import AppType

from datetime import date, datetime, timedelta
from collections import deque
from itertools import islice


class TradingScreen(AppType, Screen):

    def compose(self) -> ComposeResult:
        with TabbedContent("Market", "Inventory"):
            yield MarketWidget()
            yield InventoryWidget()

    def on_mount(self):
        self.set_interval(1, self.update_products, pause=True)

    async def update_products(self):
        for product in self.query(ProductWidget):
            product.fetch_product()


class InventoryWidget(AppType, Static):
    pass


class MarketWidget(AppType, Static):

    product_ids: reactive[list[int]] = reactive([1], recompose=True, layout=True)

    def compose(self) -> ComposeResult:
        self.log(f"recompose with {self.product_ids}")
        yield Label(f"Products: {self.product_ids}")
        with VerticalScroll():
            for product_id in self.product_ids:
                yield ProductWidget(product_id)

    async def on_mount(self):
        self.log(self.product_ids)
        self.product_ids = [
            product.product_id for product in await self.app.api.get_products()
        ]
        self.log(self.product_ids)
        await self.recompose()


class ProductWidget(AppType, Static):

    BINDINGS = [
        ("e", "increase_time_window()", "Increase time window"),
        ("q", "decrease_time_window()", "Decrease time window"),
    ]

    product_id: int
    product: reactive[Product] = reactive(Product(-1, "", -1))

    time_window = var(60, init=False)
    available_time_windows = [60, 300, 600, 1800, 3600]
    current_time_window_index = var(2)

    def __init__(self, product_id: int):
        super().__init__()
        self.product_id = product_id
        self.old_value = 150

    async def on_mount(self):
        self.fetch_product()
        now = datetime.now()
        for i in range(100 * 60, 0, -1):
            time = now - timedelta(seconds=i)
            await self.add_random_record(time)

        self.set_interval(1, self.add_random_record)

    def compose(self) -> ComposeResult:
        yield Label(f"{self.product.product_name}")
        yield PriceChart(60 * 100, self.time_window)
        yield Button("Buy", name="buy", id="buy-button")
        yield Button("Sell", name="sell", id="sell-button")

    @work(exclusive=True)
    async def fetch_product(self):
        """In the future, this should only fetch the new price and add it to the plot"""
        self.product = await self.app.api.get_product(self.product_id)

    async def watch_product(self):
        pass

    async def watch_current_time_window_index(self):
        self.time_window = self.available_time_windows[self.current_time_window_index]

    async def add_random_record(self, date: datetime | None = None):
        price_chart = self.query_one(PriceChart)
        random_val = random.randint(self.old_value - 10, self.old_value + 10)
        self.old_value = random_val
        if date is None:
            date = datetime.now()
        price_chart.add_record((date, random_val))

    def increase_time_window(self):
        if self.current_time_window_index < len(self.available_time_windows) - 1:
            self.current_time_window_index += 1
            self.time_window = self.available_time_windows[
                self.current_time_window_index
            ]

    def decrease_time_window(self):
        if self.current_time_window_index > 0:
            self.current_time_window_index -= 1
            self.time_window = self.available_time_windows[
                self.current_time_window_index
            ]


class PriceChart(AppType, PlotextPlot):
    """A Plot that displays a price chart. Supports adding records and changing its time window dynamically.

    Args:
        AppType (_type_): _description_
        PlotextPlot (_type_): _description_
    """

    record_type = tuple[datetime, float]
    marker: var[str] = var("braille")

    def __init__(
        self,
        max_num_records: int,
        time_window: int,
        initial_records: Iterable[record_type] | None = None,
        *args,
        **kwargs,
    ):
        """Initializes a price chart.

        Args:
            max_num_records (int): Maximum number of records to keep in saved in a price chart. If more records are added, the oldest records will be removed.
            time_window (int): Plot all records that are within the last `time_window` seconds.
            initial_records (Iterable[record_type] | None, optional): Sets the initial records. Defaults to None.
        """
        super().__init__(*args, **kwargs)
        self.time_window = time_window
        if initial_records is None:
            initial_records = []
        self.records = deque(initial_records, maxlen=max_num_records)

    def on_mount(self):
        self.plt.title("Price Chart - last 10 minutes")
        self.plt.xlabel("Time")
        self.plt.ylabel("Price")
        self.plt.yfrequency(5)
        self.plt.date_form("H:M:S")
        self.replot()

    def replot(self):
        # filter records that are older than the cutoff_time
        cutoff_time = datetime.now() - timedelta(seconds=self.time_window)

        # filters old records and converts the records to time deltas from now on
        filtered_records = [
            ((date - datetime.now()).total_seconds(), price)
            for date, price in self.records
            if date >= cutoff_time
        ]
        # Plot the filtered records
        self.plt.clear_data()
        if filtered_records:
            # split the records into deltas and prices
            deltas, prices = zip(*filtered_records)

            deltas: list[float] = list(deltas)
            prices: list[float] = list(prices)
            self.plt.plot(deltas, prices, marker=self.marker)
            # TODO: Better frequency, default frequency is not smooth
            self.plt.xfrequency(5)
            self.plt.yfrequency(3)

        self.refresh()

    def add_record(self, record: tuple[datetime, float]):
        """Adds a record. If the number of records exceeds the maximum number of records, the oldest records will be removed. The plot will be updated after adding the record.

        Args:
            record (tuple[datetime, float]): Price Record to be added
        """
        self.records.append(record)
        self.replot()

    def watch_marker(self):
        self.replot()
