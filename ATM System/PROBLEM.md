# LLD Machine Coding – ATM System (C++)

## Problem Statement

Design and implement an **ATM (Automated Teller Machine)** system that authenticates a cardholder and supports cash withdrawal, deposit, balance inquiry, and PIN change, using Object-Oriented Programming in C++.

This is a classic LLD problem asked at Amazon, PayPal, Visa, and JPMorgan interviews. It tests the **State pattern**, **session lifecycle**, **cash-dispensing algorithms**, and clean separation between the ATM hardware facade and banking services.

---

## Functional Requirements

1. A user inserts a **card** and enters a **PIN** to start a session.
2. After successful authentication, the ATM must support:
   - **Balance inquiry** — show available balance.
   - **Cash withdrawal** — dispense cash if balance and ATM cash allow it.
   - **Cash deposit** — credit the account (simulate deposit acceptance).
   - **PIN change** — update PIN after verifying the current PIN.
   - **Eject card / end session**.
3. The ATM holds a finite **cash inventory** by denomination (e.g., 100, 200, 500, 2000).
4. Withdrawal must dispense notes using a **greedy or optimal denomination strategy** and update inventory.
5. Failed PIN attempts are limited (e.g., **3 tries**); exceeding the limit **blocks the card** for the session (or permanently — pick one and be consistent).
6. Every successful or failed transaction should produce a **receipt/log entry** (printed to console).

---

## Business Rules

- A session is tied to one card; operations are rejected if no active authenticated session exists.
- Withdrawal amount must be a **positive multiple of the smallest denomination**.
- Withdrawal is rejected if:
  - Account balance is insufficient, **or**
  - ATM cannot assemble the exact amount from available notes.
- Deposit amount must be positive; deposited cash is added to ATM inventory (optional simplification: credit account only).
- PIN is never printed in logs; only success/failure is recorded.
- Card ejection ends the session and returns the machine to the idle state.

---

## Non-Functional Requirements

- In-memory only (accounts, cards, cash inventory simulated in memory).
- Standard C++ library only.
- Follow OOP principles; ATM behaviour must be driven by **states**, not a giant switch in `main`.
- Console I/O only; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Card` (cardNumber, pin, blocked flag)
- `Account` (accountId, balance, linked card)
- `BankService` / `Bank` (authenticate, getBalance, debit, credit, changePin)
- `CashInventory` (denomination → count; `canDispense(amount)`, `dispense(amount)`)
- `ATM` (facade holding current state, card, bank service, cash inventory)
- `ATMState` (abstract: `insertCard`, `enterPin`, `withdraw`, `deposit`, `ejectCard`, ...)
  - Concrete: `IdleState`, `CardInsertedState`, `AuthenticatedState`, `DispensingState` (optional)
- `Transaction` / `Receipt` (type, amount, status, timestamp)

---

### 2. State Pattern
- Each state implements only the operations that are valid in that state; invalid operations return a clear error.
- Transitions: Idle → CardInserted → Authenticated → (operation) → Authenticated / Idle.
- The `ATM` context delegates all user actions to the current `ATMState`.

---

### 3. Cash Dispensing
- Prefer largest denominations first (greedy), or discuss DP for exact change.
- Dispense must be **atomic**: either inventory is updated fully or not at all (no partial dispense on failure).

---

### 4. Session & Security
- Track remaining PIN attempts on the card/session.
- Do not keep the PIN in plain fields longer than needed for the check.

---

## Constraints

- Single ATM instance; no network/distributed banking required.
- Concurrency is NOT required.
- No real hardware; card insert/eject and cash dispense are simulated method calls.
- Mini-statements / fund transfer are NOT required (bonus only).

---

## Example Scenario

```text
Setup: Account balance = 10000, PIN = 1234
       ATM cash: 2000×5, 500×10, 100×20

Action: insertCard(card) → OK (state: CardInserted)
Action: enterPin(0000)   → REJECTED (attempts left: 2)
Action: enterPin(1234)   → OK (state: Authenticated)

Action: balanceInquiry() → 10000
Action: withdraw(3700)   → OK | Dispense: 2000×1, 500×3, 100×2
                           Account: 6300 | ATM inventory updated
Action: deposit(500)     → OK | Account: 6800
Action: changePin(1234, 4321) → OK
Action: ejectCard()      → OK (state: Idle)

Invalid examples:
withdraw(50)   → REJECTED (not multiple of min denomination)
withdraw(9999) → REJECTED (ATM cannot assemble notes / insufficient ATM cash)
enterPin after 3 failures → Card blocked; session ends
```

---

## Bonus Challenges (If Time Permits)

- Support **mini-statement** (last N transactions).
- Add **fund transfer** between two accounts.
- Implement **optimal note combination** (DP) instead of greedy.
- Model **hardware components** separately (`CardReader`, `CashDispenser`, `Screen`, `Keypad`) behind interfaces.
- Make cash inventory reloadable by an admin operator role.
