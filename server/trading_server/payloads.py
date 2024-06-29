from pydantic import BaseModel, Field
from datetime import datetime, timedelta


class AmountPayload(BaseModel):
    amount: int = Field(default=1, ge=1, examples=[1, 2, 3])
