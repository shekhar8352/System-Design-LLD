from __future__ import annotations

from typing import Optional, Union

from common.operation_result import OperationResult
from domain.value_objects import AccountNumber, Money
from factories.account_factory import AccountFactory
from repositories.account_repository import AccountRepository


class BankAccountService:
    """
    Facade / application service — orchestrates use cases.
    Callers interact with the system through this layer instead of entities directly.
    """

    def __init__(
        self,
        repository: AccountRepository,
        account_factory: Optional[AccountFactory] = None,
    ) -> None:
        self._repository = repository
        self._factory = account_factory or AccountFactory(repository)

    def create_account(
        self,
        account_number: str,
        account_holder_name: str,
        initial_balance: Union[float, int, str] = 0,
    ) -> OperationResult:
        try:
            account = self._factory.create(
                account_number=account_number,
                account_holder_name=account_holder_name,
                initial_balance=initial_balance,
            )
            return OperationResult.ok(
                message=(
                    f"Account created successfully.\n"
                    f"Account Number: {account.account_number}\n"
                    f"Account Holder: {account.account_holder_name}\n"
                    f"Initial Balance: {account.balance:.2f}"
                ),
                balance=account.balance,
            )
        except ValueError as exc:
            return OperationResult.fail(str(exc))

    def deposit(self, account_number: str, amount: Union[float, int, str]) -> OperationResult:
        account = self._find_account(account_number)
        if account is None:
            return OperationResult.fail(f"Account '{account_number}' not found.")

        try:
            money = Money.positive(amount)
        except ValueError as exc:
            return OperationResult.fail(str(exc))

        return account.deposit(money)

    def withdraw(self, account_number: str, amount: Union[float, int, str]) -> OperationResult:
        account = self._find_account(account_number)
        if account is None:
            return OperationResult.fail(f"Account '{account_number}' not found.")

        try:
            money = Money.positive(amount)
        except ValueError as exc:
            return OperationResult.fail(str(exc))

        return account.withdraw(money)

    def check_balance(self, account_number: str) -> OperationResult:
        account = self._find_account(account_number)
        if account is None:
            return OperationResult.fail(f"Account '{account_number}' not found.")
        return account.get_balance_summary()

    def _find_account(self, account_number: str):
        try:
            number = AccountNumber(account_number)
        except ValueError as exc:
            return None
        return self._repository.find_by_number(number)
