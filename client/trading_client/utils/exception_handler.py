from textual.widget import Widget

from functools import wraps

from typing import Iterable


from functools import wraps
from typing import Callable, Iterable, Type, Any
import asyncio


def catch_and_notify(exceptions: Iterable[Type[Exception]]) -> Callable:
    def catch_and_notify_(func: Callable[..., Any]) -> Callable[..., Any]:
        if asyncio.iscoroutinefunction(func):

            @wraps(func)
            async def async_wrapper(self: Widget, *args: Any, **kwargs: Any) -> Any:
                try:
                    return await func(self, *args, **kwargs)
                except tuple(exceptions) as e:
                    self.app.notify(str(e), severity="error", timeout=5)

            return async_wrapper
        else:

            @wraps(func)
            def wrapper(self: Widget, *args: Any, **kwargs: Any) -> Any:
                try:
                    return func(self, *args, **kwargs)
                except tuple(exceptions) as e:
                    self.app.notify(str(e), severity="error", timeout=5)

            return wrapper

    return catch_and_notify_
