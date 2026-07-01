from __future__ import annotations

from dataclasses import dataclass
from decimal import Decimal
from typing import Optional


@dataclass(frozen=True)
class OperationResult:
    """Result object pattern — explicit success/failure without exceptions."""

    success: bool
    message: str
    balance: Optional[Decimal] = None

    @classmethod
    def ok(cls, message: str, balance: Optional[Decimal] = None) -> "OperationResult":
        return cls(success=True, message=message, balance=balance)

    @classmethod
    def fail(cls, message: str) -> "OperationResult":
        return cls(success=False, message=message, balance=None)
