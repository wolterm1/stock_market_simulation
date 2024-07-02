from asyncio import sleep
from functools import partial
from itertools import tee
import random
from collections import deque
from datetime import date, datetime, timedelta, time
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
from textual.geometry import Size
from textual.events import Resize
from textual.containers import VerticalScroll, Vertical
from textual.message import Message
from textual.reactive import reactive, var
from textual.screen import ModalScreen, Screen
from textual.widgets import Button, Label, Static, TabbedContent, Footer, Input
from textual.worker import Worker, WorkerState
from textual_plotext import PlotextPlot

from trading_client.api.product import PriceRecord
from trading_client.api import Product
from trading_client.api.exceptions import IncorrectCredentials, UserAlreadyExists
from trading_client.utils import AppType, catch_and_notify


class TradingScreen(AppType, Screen):
    """ """

    BINDINGS = [
        ("j", "decrease_time_window()", "Decrease time window"),
        ("k", "increase_time_window()", "Increase time window"),
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

    def compose(self) -> ComposeResult:
        yield Label("Inventory")


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
        self.set_interval(1, self.update_market)
        await self.recompose()

    async def update_market(self):
        market = await self.app.api.get_market()
        # self.log.info(market)
        for product_widget in self.query(ProductWidget):
            product_widget.in_stock = market.supply[product_widget.product_id]

    @on(Resize)
    @work(thread=True, name="update_products_height")
    async def update_products_height(self, resize: Resize):
        space = resize.size.height
        num_products = len(self.product_ids)
        self.log(space)

        min_height = 15
        max_height = 28

        if space < min_height:
            self.log.warning(
                f"Cannot fit any products in the available space. Minimum height: {min_height}, available space: {space}"
            )
            min_height = space

        # Calculate the maximum and minimum number of products that can fit within the constraints
        max_possible_products = space // min_height
        # Ensure at least 1 product is displayed
        min_possible_products = max(1, space // max_height)

        # clamp num_products to the constrains
        num_products = min(
            max(min_possible_products, num_products), max_possible_products
        )

        # ideal height for each product
        ideal_height = space / num_products

        # adjust ideal_height within the constraints, if necessary
        while not (min_height <= ideal_height <= max_height):
            # if the ideal height is outside the constraints, adjust the number of products, but do not exceed the constraints
            if ideal_height < min_height:
                num_products = max(num_products - 1, min_possible_products)
            elif ideal_height > max_height:
                num_products = min(num_products + 1, max_possible_products)
            else:
                # Ideal height is within the range, HAPPY
                break

            # recalculate the ideal height and check if the new ideal height fits within the constraints
            ideal_height = space / num_products

            # if the numbers of products is at the edge of the constraints, we cannot do more and accept the constraint infringement
            if (
                num_products == min_possible_products
                or num_products == max_possible_products
            ):
                # Exit if adjustments do not change the outcome
                break

        self.query(ProductWidget).set_styles(f"height: {ideal_height};")


cool_colors = [
    Color.parse("crimson"),
    Color.parse("darkorange"),
    Color.parse("deepskyblue"),
    Color.parse("firebrick"),
    Color.parse("forestgreen"),
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

    parent: MarketWidget
    MAX_RECORD_LENGTH = 60 * 60

    product_id: int
    product: reactive[Product] = reactive(Product(-1, ""))
    in_stock: reactive[int] = reactive(0)

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
        yield PriceChart()
        yield TradeWidget()

    async def on_mount(self):
        self.fetch_product()
        self.styles.background = self.color.with_alpha(0.1)
        self.query_one("#product-name", Label).styles.background = self.color.darken(
            0.3
        )
        current_time = datetime.now().microsecond
        await sleep(1 - current_time / 1000000)
        self.record_fetcher = self.set_interval(1, self.fetch_new_records, pause=True)

    async def watch_in_stock(self, in_stock: int):
        await self.update_product_label()

    @work(name="fetch_product")
    async def fetch_product(self):
        """In the future, this should only fetch the new price and add it to the plot"""
        self.product = await self.app.api.get_product(self.product_id)
        product_name = self.query_one("#product-name", Label)
        product_name.update(self.product.product_name)

        self.post_message(self.FetchProductFinished())

    @on(FetchProductFinished)
    async def on_fetch_product_finished(self, message: FetchProductFinished):
        current_time = datetime.now().microsecond
        await sleep(1 - current_time / 1000000)
        self.record_fetcher.resume()

    @work(exclusive=True, name="fetch_new_records")
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
        await self.update_product_label()

    async def update_product_label(self):
        label = self.query_one("#product-name", Label)
        text = []
        text.append(self.product.product_name)
        text.append(f"{self.in_stock} in stock")
        if self.records:
            text.append(f"${self.records[-1].value}")
        label.update(" - ".join(text))


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
        self.color = self.parent.color
        self.replot()

    @work(thread=True, name="replot")
    async def replot(self):
        """Replots the price by clearing all data, adding all records that are within \
        the time window and refreshing the widget."""
        self.plt.clear_data()
        # self.plt.title(
        #     f"{self.parent.product.product_name} Price Chart - last {humanize.precisedelta(self.time_window)}"
        # )
        # self.plt.xlabel("Time")
        # self.plt.ylabel("Price")

        self.plt.axes_color((255, 0, 0))
        self.plt.ticks_color((0, 255, 0))

        if not self.records:
            self.log.info("No records to plot")
            return

        # filter records that are older than the cutoff_time
        now = datetime.now()
        cutoff_time = now - self.time_window

        # only plot the desired records
        filtered_records = [
            record for record in self.records if record.date > cutoff_time
        ]

        deltas = [record.date - now for record in filtered_records]
        prices = [record.value for record in filtered_records]

        # self.log.info(
        #     f"{len(self.records)} records, plotting {len(filtered_records)} records.",
        #     f"\nCurrent time: {now}. Cut off time: {cutoff_time}",
        #     "\nFirst: ",
        #     filtered_records[0],
        #     "\nLast: ",
        #     filtered_records[-1],
        # )

        # Plot the filtered records
        if not prices or not deltas:
            return

        delta_seconds = [delta.total_seconds() for delta in deltas]

        self.plt.plot(delta_seconds, prices, marker=self.marker, color=self.color.rgb)
        self.plt.hline(prices[-1], color=self.color.lighten(0.2).rgb)

        # calculate xticks dynamically
        # Always 6 ticks, evenly spaced out
        # Add one second because the actual time span is from the first record to the
        # last record is 59 seconds, but we want to calc with 1 minute (we include
        # the waiting time for the next record in the time span for accurate ticking)
        total_time_span = deltas[-1] - deltas[0] + timedelta(seconds=1)
        num_xticks = 6
        xtick_intervals = total_time_span / (num_xticks - 1)
        xticks = [
            float(int((deltas[-1] + (-i) * xtick_intervals).total_seconds()))
            for i in range(num_xticks)
        ]

        # self.log.info(
        #     "total_time_span",
        #     total_time_span,
        #     "\ninterval",
        #     xtick_intervals,
        #     "\nxticks:",
        #     xticks,
        # )

        # Convert xticks to human-readable format
        labels = [humanize.naturaltime(x) for x in xticks]

        # Apply the calculated xticks and their labels
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
        # self.plt.title(
        #     f"{self.parent.product.product_name} Price Chart - last {humanize.precisedelta(self.time_window)}"
        # )
        self.replot()


class TradeWidget(AppType, Static):

    parent: ProductWidget

    BORDER_SUBTITLE = "Your Position"

    def compose(self) -> ComposeResult:
        """ """
        yield Label("Holding:", id="holding-label")
        yield Label("5", id="holding-value")
        yield Button("Invest", name="buy", id="buy-button")
        yield Input(
            placeholder="Amount", name="amount", id="buy-amount-input", type="integer"
        )
        yield Button("Sell", name="sell", id="sell-button")
        yield Input(
            placeholder="Amount", name="amount", id="sell-amount-input", type="integer"
        )

    def on_mount(self):
        self.color = self.parent.color

        self.styles.border = ("heavy", self.color.darken(0.3))
        self.styles.border_top = None

        holding_label = self.query_one("#holding-label", Label)
        holding_label.styles.color = self.color
        holding_label.styles.text_style = "bold"

        buy_button = self.query_one("#buy-button", Button)
        buy_button.styles.background = self.color

        sell_button = self.query_one("#sell-button", Button)
        sell_button.styles.background = self.color.lighten(0.5).blend(
            (181, 16, 33, 255), 0.6
        )

        self.styles.hatch = ("left", self.color.with_alpha(0.2))
