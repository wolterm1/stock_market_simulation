from functools import partial
import random
from collections import deque
from datetime import date, datetime, timedelta
from typing import Iterable

from httpx import ConnectError
from textual import on, work
from textual.app import ComposeResult
from textual.containers import (
    VerticalScroll,
)
from textual.message import Message
from textual.reactive import reactive, var
from textual.screen import ModalScreen, Screen
from textual.widgets import (
    Button,
    Label,
    Static,
    TabbedContent,
)
from textual.worker import Worker, WorkerState
from textual_plotext import PlotextPlot

from trading_client.api import Product
from trading_client.api.exceptions import IncorrectCredentials, UserAlreadyExists
from trading_client.utils import AppType, catch_and_notify


class TradingScreen(AppType, Screen):
    """ """

    def compose(self) -> ComposeResult:
        """ """
        with TabbedContent("Market", "Inventory"):
            yield MarketWidget()
            yield InventoryWidget()

    def on_mount(self):
        """ """
        self.set_interval(1, self.update_products, pause=True)

    async def update_products(self):
        for product in self.query(ProductWidget):
            product.fetch_product()


class InventoryWidget(AppType, Static):
    """ """

    pass


class MarketWidget(AppType, Static):
    """ """

    product_ids: reactive[list[int]] = reactive([1], recompose=True, layout=True)

    def compose(self) -> ComposeResult:
        """ """
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
    """ """

    BINDINGS = [
        ("e", "increase_time_window()", "Increase time window"),
        ("q", "decrease_time_window()", "Decrease time window"),
    ]

    product_id: int
    product: reactive[Product] = reactive(Product(-1, ""))

    available_time_windows = [
        timedelta(minutes=1),
        timedelta(minutes=5),
        timedelta(minutes=10),
        timedelta(minutes=30),
        timedelta(hours=1),
    ]
    current_time_window_index = var(2)
    time_window = var(timedelta(minutes=5), init=False)

    class FetchProductFinished(Message):
        pass

    def __init__(self, product_id: int):
        super().__init__()
        self.product_id = product_id
        self.old_value = 150

    def compose(self) -> ComposeResult:
        """ """
        yield Label(f"{self.product.product_name}")
        yield PriceChart(60 * 60 * 60, self.time_window)
        yield Button("Buy", name="buy", id="buy-button")
        yield Button("Sell", name="sell", id="sell-button")

    async def on_mount(self):
        self.fetch_product()
        self.fetcher = self.set_interval(1, self.fetch_new_records, pause=True)

    @work(exclusive=True, name="work")
    async def fetch_product(self):
        """In the future, this should only fetch the new price and add it to the plot"""
        self.product = await self.app.api.get_product(self.product_id)
        self.post_message(self.FetchProductFinished())

    @on(FetchProductFinished)
    async def on_fetch_product_finished(self, message: FetchProductFinished):
        self.fetcher.resume()

    @work(exclusive=True)
    async def fetch_new_records(self):
        price_chart = self.query_one(PriceChart)

        if price_chart.records:
            oldest_record_time = price_chart.records[0][0]
        else:
            oldest_record_time = datetime(1970, 1, 1)

        records = await self.app.api.get_records(
            self.product_id, from_=oldest_record_time
        )

        for record in records:
            if record.date > oldest_record_time:
                price_chart.add_record((record.date, record.value))

    async def watch_current_time_window_index(self):
        self.query_one(PriceChart).time_window = self.available_time_windows[
            self.current_time_window_index
        ]

    def increase_time_window(self):
        """ """
        if self.current_time_window_index < len(self.available_time_windows) - 1:
            self.current_time_window_index += 1

    def decrease_time_window(self):
        """ """
        if self.current_time_window_index > 0:
            self.current_time_window_index -= 1


class PriceChart(AppType, PlotextPlot):
    """A Plot that displays a price chart.
    Supports adding records and changing its time window dynamically.

    Args:
      max_num_records(int | None): Maximum number of records to keep saved in a price chart. \
    If more records are added, the oldest records will be removed. \
    None for no limit.
      time_window(int): Plot all records that are within the last `time_window` \
    seconds.
      initial_records(Iterable[record_type] | None, optional): Start with these records

    Returns:

    """

    record_type = tuple[datetime, float]
    time_window = reactive(timedelta(minutes=1))
    marker: var[str] = var("braille")

    def __init__(
        self,
        max_num_records: int | None,
        time_window: timedelta,
        initial_records: Iterable[record_type] | None = None,
        *args,
        **kwargs,
    ):
        """Initializes a price chart.

        :param max_num_records: Maximum number of records to keep saved in a price chart. \
            If more records are added, the oldest records will be removed. \
            None for no limit.
        :type max_num_records: int | None
        :param time_window: Defines the starting point of the time window by \
            subtracting the time window from the current time.
        :type time_window: timedelta
        :param initial_records: Start with these records
        :type initial_records: Iterable[record_type] | None, optional
        """
        super().__init__(*args, **kwargs)
        self.time_window = time_window
        if initial_records is None:
            initial_records = []
        self.records = deque(initial_records, maxlen=max_num_records)

    def on_mount(self):
        """ """
        self.plt.title("Price Chart - last 10 minutes")
        self.plt.xlabel("Time")
        self.plt.ylabel("Price")
        self.plt.yfrequency(5)
        self.plt.date_form("H:M:S")
        self.replot()

    def replot(self):
        """ """
        # filter records that are older than the cutoff_time
        cutoff_time = datetime.now() - self.time_window

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
          initial_records(Iterable[record_type] | None, optional): Start with these records
          record: tuple[datetime:
          float]:

        Returns:

        """
        self.records.append(record)
        self.replot()

    def watch_marker(self):
        """ """
        self.replot()
