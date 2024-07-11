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


import httpx
from rich.text import Text
from rich.style import Style

import humanize
from httpx import ConnectError
from textual import on, work
from textual.app import ComposeResult
from textual.css.query import NoMatches
from textual.color import Color
from textual.geometry import Size
from textual.events import Resize
from textual.containers import VerticalScroll, Vertical, Grid, Horizontal, Container
from textual.message import Message
from textual.reactive import reactive, var
from textual.screen import ModalScreen, Screen
from textual.widgets import Button, Label, Static, TabbedContent, Footer, Input
from textual.worker import Worker, WorkerState
from textual_plotext import PlotextPlot

from trading_client.api.market import Supply
from trading_client.api import Product, PriceRecord, User, Market, InventoryItem
from trading_client.api.exceptions import (
    IncorrectCredentials,
    UserAlreadyExists,
    TransactionFailed,
)
from trading_client.utils import AppType, catch_and_notify, COOL_COLORS


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
        yield UserInfoWidget()
        yield Footer()

    def on_mount(self):
        self.updater = self.set_interval(1, self.update_childs, pause=True)

    def on_market_widget_all_product_fetch_done(
        self, message: "MarketWidget.AllProductFetchDone"
    ):
        self.updater.resume()

    async def update_childs(self):
        market = self.query_one(MarketWidget)
        products = market.query(ProductWidget)
        inventory = self.query_one(UserInfoWidget)

        market.fetch_market()
        inventory.fetch_user()
        for product in products:
            product.fetch_new_records()

        for product in products:
            await product.update_product_label()

        self.refresh()

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


class UserInfoWidget(AppType, Static):
    """ """

    user: var[User] = var(User(-1, "", 0, []))

    def compose(self) -> ComposeResult:
        with Horizontal(id="heading"):
            yield Label("Logged in as: ", id="logged-in-as-label")
            yield Button("Logout", id="logout-button")

        with Horizontal():
            yield Label("Balance", id="balance-label")
            yield Label("", id="balance-value")

        yield InventoryWidget()

    @work(exclusive=True)
    async def fetch_user(self):
        self.user = await self.app.api.get_user()
        await self.update_user()

    async def update_user(self):
        logged_in_as_label = self.query_one("#logged-in-as-label", Label)
        logged_in_as_label.update(f"Logged in as: {self.user.user_name}")

        balance_label = self.query_one("#balance-label", Label)
        money = Text(f"${self.user.balance}", style="bold green")
        balance_label_text = Text("Balance: ", style="bold") + money
        balance_label.update(balance_label_text)

        inventory_widget = self.query_one(InventoryWidget)
        inventory_widget.inventory = self.user.inventory

    @on(Button.Pressed, "#logout-button")
    async def logout(self, event: Button.Pressed):
        await self.app.api.logout()
        self.app.pop_screen()


class InventoryWidget(AppType, Static):
    inventory: reactive[list[InventoryItem]] = reactive([], recompose=True)

    def compose(self) -> ComposeResult:
        yield Label("Inventory", id="inventory-title")
        with Grid(id="inventory-content"):
            yield Label("Product", id="product-label")
            yield Label("Holding", id="quantity-label")
            for item in self.inventory:
                yield Button(
                    "↩",
                    id=f"scroll-to-{item.product.product_id}-button",
                    classes="scroll-to-product-button",
                    tooltip="Scroll to product",
                )
                yield Label(
                    item.product.product_name,
                    id=f"product-label-{item.product.product_id}",
                    classes="inventory-product",
                )
                yield Label(
                    str(item.quantity),
                    id=f"product-quantity-{item.product.product_id}",
                    classes="inventory-product-quantity",
                )

    @on(Button.Pressed, ".scroll-to-product-button")
    async def scroll_to_product(self, event: Button.Pressed):
        self.log.info(f"{event.button.id} pressed")
        if event.button.id is None:
            return
        product_id = int(event.button.id.split("-")[-2])
        self.log(product_id)
        market_scroll = self.screen.query_one(
            "MarketWidget > VerticalScroll", VerticalScroll
        )
        for product in market_scroll.query(ProductWidget):
            if product.product_id == product_id:
                self.log.info(f"Scrolling to product {event.button.id}")
                market_scroll.scroll_to_widget(product)
                break


class MarketWidget(AppType, Static):
    """ """

    class AllProductFetchDone(Message):
        pass

    product_ids: reactive[list[int]] = reactive(
        [], recompose=True, layout=True, init=False, always_update=True
    )
    market = var(Market({}), init=False)
    all_product_fetch_done: var[bool] = var(False, init=False)
    product_fetch_done: dict[int, bool] = {}

    def __init__(self):
        super().__init__()
        self.product_fetch_done = {product_id: False for product_id in self.product_ids}
        self.old_height = self.size.height

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

    def compute_all_product_fetch_done(self):
        return all(self.product_fetch_done.values())

    def watch_all_product_fetch_done(self, all_product_fetch_done: bool):
        if all_product_fetch_done:
            self.post_message(self.AllProductFetchDone())

    def on_product_widget_fetch_product_finished(
        self, message: "ProductWidget.FetchProductFinished"
    ):
        self.product_fetch_done[message.product_id] = True

    @work(exclusive=True)
    async def fetch_market(self):
        self.market = await self.app.api.get_market()

    async def watch_market(self):
        for product_widget in self.query(ProductWidget):
            product_widget.in_stock = self.market.supply[product_widget.product_id]

    # @work(thread=True, name="update_products_height")
    def on_resize(self, new_resize: Resize):
        if new_resize.size.height == self.old_height:
            return

        space = new_resize.size.height
        num_products = len(self.product_ids)
        self.log(space)

        min_height = 15
        max_height = 28

        if space < min_height:
            self.log.warning(
                f"Cannot fit any products in the available space. \
                  Minimum height: {min_height}, available space: {space}"
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

        # ideal height for each product, so that n products perfectly fit
        # in the available space
        ideal_height = space / num_products

        # adjust ideal_height within the constraints, if necessary
        while not (min_height <= ideal_height <= max_height):
            # if the ideal height is outside the constraints, adjust the number of
            # visible products, but do not exceed the constraints
            if ideal_height < min_height:
                num_products = max(num_products - 1, min_possible_products)
            elif ideal_height > max_height:
                num_products = min(num_products + 1, max_possible_products)
            else:
                # Ideal height is within the range, HAPPY
                break

            # recalculate the ideal height
            ideal_height = space / num_products

            # if the numbers of products is at the edge of the constraints, we cannot do
            # more and accept the constraint infringement
            if (
                num_products == min_possible_products
                or num_products == max_possible_products
            ):
                break

        self.query(ProductWidget).set_styles(f"height: {ideal_height};")


class ProductWidget(AppType, Static):
    """ """

    parent: VerticalScroll
    MAX_RECORD_LENGTH = 60 * 60

    product_id: int
    product: reactive[Product] = reactive(Product(-1, ""))
    in_stock: var[int] = var(0)

    class FetchProductFinished(Message):
        def __init__(self, product_id) -> None:
            super().__init__()
            self.product_id = product_id

    def __init__(self, product_id: int):
        super().__init__()
        self.product_id = product_id
        self.color = random.choice(COOL_COLORS)
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

    @work
    async def fetch_product(self):
        self.log.info(f"Fetching product {self.product_id}")

        self.product = await self.app.api.get_product(self.product_id)
        product_name = self.query_one("#product-name", Label)
        product_name.update(self.product.product_name)

        self.post_message(self.FetchProductFinished(self.product_id))

    @work(exclusive=True, name="fetch_new_records")
    async def fetch_new_records(self):
        """Fetches new records from the API and adds them to the plot."""
        # if not self.parent.can_view(self):
        #     return

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
    def replot(self):
        """Replots the price by clearing all data, adding all records that are within \
        the time window and refreshing the widget."""
        self.plt.clear_data()

        # this doesn't work, we are stuck with blue axes
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

        self.app.call_from_thread(self.refresh)

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

        buy_button = self.query_one("#buy-button", Button)
        buy_button.styles.background = self.color

        sell_button = self.query_one("#sell-button", Button)
        sell_button.styles.background = self.color.lighten(0.5).blend(
            (181, 16, 33, 255), 0.6
        )

        self.styles.hatch = ("left", self.color.with_alpha(0.2))

    @on(Button.Pressed, "#buy-button")
    @catch_and_notify(
        [
            TransactionFailed,
        ]
    )
    async def buy(self, event: Button.Pressed):
        try:
            amount = int(self.query_one("#buy-amount-input", Input).value)
            await self.parent.app.api.buy_product(self.parent.product_id, amount)
        except httpx.HTTPStatusError as e:
            self.log(e.response.json())
            self.app.notify(f"Transaction failed: {e.response.json()['detail']}")

    @on(Button.Pressed, "#sell-button")
    @catch_and_notify(
        [
            TransactionFailed,
        ]
    )
    async def sell(self, event: Button.Pressed):
        amount = int(self.query_one("#sell-amount-input", Input).value)
        await self.parent.app.api.sell_product(self.parent.product_id, amount)
