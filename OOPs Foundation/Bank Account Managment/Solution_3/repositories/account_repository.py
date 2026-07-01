from abc import ABC, abstractmethod
from typing import Dict, Optional

from domain.bank_account import BankAccount
from domain.value_objects import AccountNumber


class AccountRepository(ABC):
    """Repository pattern — abstracts persistence from domain logic."""

    @abstractmethod
    def save(self, account: BankAccount) -> None:
        ...

    @abstractmethod
    def find_by_number(self, account_number: AccountNumber) -> Optional[BankAccount]:
        ...

    @abstractmethod
    def exists(self, account_number: AccountNumber) -> bool:
        ...


class InMemoryAccountRepository(AccountRepository):
    """In-memory implementation — suitable for the exercise constraints."""

    def __init__(self) -> None:
        self._accounts: Dict[str, BankAccount] = {}

    def save(self, account: BankAccount) -> None:
        self._accounts[str(account.account_number)] = account

    def find_by_number(self, account_number: AccountNumber) -> Optional[BankAccount]:
        return self._accounts.get(str(account_number))

    def exists(self, account_number: AccountNumber) -> bool:
        return str(account_number) in self._accounts
