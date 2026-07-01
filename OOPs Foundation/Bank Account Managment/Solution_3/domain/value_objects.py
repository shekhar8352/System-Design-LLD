from __future__ import annotations

from dataclasses import dataclass
from decimal import Decimal, InvalidOperation
from typing import Union

AmountInput = Union[float, int, str, Decimal]


@dataclass(frozen=True)
class AccountNumber:
    """Immutable value object — guarantees a non-empty account identifier."""

    value: str

    def __post_init__(self) -> None:
        normalized = self.value.strip()
        if not normalized:
            raise ValueError("Account number cannot be empty.")
        object.__setattr__(self, "value", normalized)

    def __str__(self) -> str:
        return self.value


@dataclass(frozen=True)
class Money:
    """Immutable value object — centralizes monetary validation."""

    amount: Decimal

    def __post_init__(self) -> None:
        if not isinstance(self.amount, Decimal):
            object.__setattr__(self, "amount", Decimal(str(self.amount)))
        if self.amount < Decimal("0"):
            raise ValueError("Money amount cannot be negative.")

    @classmethod
    def from_number(cls, value: AmountInput) -> Money:
        try:
            return cls(Decimal(str(value)))
        except (InvalidOperation, ValueError) as exc:
            raise ValueError(f"Invalid monetary value: {value}") from exc

    @classmethod
    def positive(cls, value: AmountInput) -> Money:
        money = cls.from_number(value)
        if money.amount <= Decimal("0"):
            raise ValueError("Amount must be greater than zero.")
        return money

    def __add__(self, other: "Money") -> "Money":
        return Money(self.amount + other.amount)

    def __sub__(self, other: "Money") -> "Money":
        return Money(self.amount - other.amount)

    def __ge__(self, other: "Money") -> bool:
        return self.amount >= other.amount

    def __str__(self) -> str:
        return f"{self.amount:.2f}"
