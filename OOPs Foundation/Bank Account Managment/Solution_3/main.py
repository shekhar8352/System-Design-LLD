from repositories.account_repository import InMemoryAccountRepository
from services.bank_account_service import BankAccountService


def _print_result(title: str, result) -> None:
    status = "SUCCESS" if result.success else "FAILED"
    print(f"\n[{status}] {title}")
    print(result.message)


def main() -> None:
    repository = InMemoryAccountRepository()
    bank = BankAccountService(repository)

    create_result = bank.create_account(
        account_number="1001",
        account_holder_name="Sudhanshu Shekhar",
        initial_balance=5000,
    )
    _print_result("Create Account", create_result)

    _print_result("Deposit 2000", bank.deposit("1001", 2000))
    _print_result("Withdraw 1000", bank.withdraw("1001", 1000))
    _print_result("Withdraw 7000", bank.withdraw("1001", 7000))
    _print_result("Check Balance", bank.check_balance("1001"))


if __name__ == "__main__":
    main()
