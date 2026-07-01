from decimal import Decimal

from common.operation_result import OperationResult
from domain.value_objects import AccountNumber, Money


class BankAccount:
    """
    Entity — owns account state and enforces all balance rules internally.
    External code cannot mutate balance directly.
    """

    __slots__ = ("_account_number", "_account_holder_name", "_balance")

    def __init__(
        self,
        account_number: AccountNumber,
        account_holder_name: str,
        initial_balance: Money,
    ) -> None:
        name = account_holder_name.strip()
        if not name:
            raise ValueError("Account holder name cannot be empty.")

        self._account_number = account_number
        self._account_holder_name = name
        self._balance = initial_balance

    @property
    def account_number(self) -> AccountNumber:
        return self._account_number

    @property
    def account_holder_name(self) -> str:
        return self._account_holder_name

    @property
    def balance(self) -> Decimal:
        return self._balance.amount

    def deposit(self, amount: Money) -> OperationResult:
        if amount.amount <= Decimal("0"):
            return OperationResult.fail("Deposits of zero or negative amounts are not allowed.")

        self._balance = self._balance + amount
        return OperationResult.ok(
            f"Deposited {amount}. New balance: {self._balance}.",
            balance=self._balance.amount,
        )

    def withdraw(self, amount: Money) -> OperationResult:
        if amount.amount <= Decimal("0"):
            return OperationResult.fail("Withdrawals of zero or negative amounts are not allowed.")

        if self._balance.amount < amount.amount:
            return OperationResult.fail(
                f"Insufficient balance. Available: {self._balance}, requested: {amount}."
            )

        self._balance = self._balance - amount
        return OperationResult.ok(
            f"Withdrew {amount}. Remaining balance: {self._balance}.",
            balance=self._balance.amount,
        )

    def get_balance_summary(self) -> OperationResult:
        return OperationResult.ok(
            message=(
                f"Account Number: {self._account_number}\n"
                f"Account Holder: {self._account_holder_name}\n"
                f"Balance: {self._balance}"
            ),
            balance=self._balance.amount,
        )
