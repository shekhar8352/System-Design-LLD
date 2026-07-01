from __future__ import annotations

from typing import Union

from domain.bank_account import BankAccount
from domain.value_objects import AccountNumber, Money
from repositories.account_repository import AccountRepository


class AccountFactory:
    """Factory pattern — centralizes account creation and uniqueness checks."""

    def __init__(self, repository: AccountRepository) -> None:
        self._repository = repository

    def create(
        self,
        account_number: str,
        account_holder_name: str,
        initial_balance: Union[float, int, str] = 0,
    ) -> BankAccount:
        number = AccountNumber(account_number)

        if self._repository.exists(number):
            raise ValueError(f"Account number '{number}' already exists.")

        account = BankAccount(
            account_number=number,
            account_holder_name=account_holder_name,
            initial_balance=Money.from_number(initial_balance),
        )
        self._repository.save(account)
        return account
