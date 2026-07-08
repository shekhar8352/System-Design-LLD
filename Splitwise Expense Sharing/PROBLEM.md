# LLD Machine Coding – Splitwise / Expense Sharing System (C++)

## Problem Statement

Design and implement an **Expense Sharing System** (like Splitwise) where users can add shared expenses and the system tracks who owes whom, using Object-Oriented Programming in C++.

This problem is frequently asked at Flipkart, Cred, PhonePe, and Amazon interviews. It tests **domain modelling**, **polymorphism** (different split types), **precision handling**, and **balance-sheet bookkeeping**.

---

## Functional Requirements

1. The system must support **users** with a unique id, name, email, and phone.
2. A user can add an **expense** with:
   - Total amount.
   - The user who **paid**.
   - The list of users among whom the expense is **split**.
   - A **split type**: `EQUAL`, `EXACT`, or `PERCENT`.
3. The system must support the following queries:
   - `showBalance(userId)` — all balances involving one user.
   - `showBalances()` — all non-zero balances in the system.
4. Balances must be shown in the format: `UserA owes UserB: amount`.
5. A user can **settle up** with another user, fully or partially.

---

## Business Rules

- **EQUAL split**: amount is divided equally; if it doesn't divide evenly, the payer absorbs the rounding difference (e.g., 100/3 → 33.34, 33.33, 33.33).
- **EXACT split**: the provided shares must sum exactly to the total amount; otherwise reject the expense with a validation error.
- **PERCENT split**: percentages must sum to exactly 100; otherwise reject.
- All amounts are rounded to **2 decimal places**.
- The payer does not owe themselves — their own share is excluded from the balance sheet.
- Balances are **net**: if A owes B 50 and B later owes A 20, the sheet shows A owes B 30.
- Settling more than the owed amount must be rejected.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles with clear class boundaries.
- No UI required (console output is fine).

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `User` (id, name, email, phone)
- `Expense` (amount, paid-by, splits, metadata)
- `Split` (abstract) with `EqualSplit`, `ExactSplit`, `PercentSplit`
- `SplitStrategy` / validation logic per split type (Strategy pattern or polymorphic `validate()`)
- `BalanceSheet` (stores pairwise net balances)
- `ExpenseManager` / `SplitwiseService` (facade tying it together)

---

### 2. Split Validation via Polymorphism
- Each split type validates itself (or is validated by a strategy).
- Adding a new split type (e.g., `SHARE`-based) must not require modifying existing split logic (Open/Closed Principle).

---

### 3. Balance Netting
- Store balances as a map keyed by user pair, always netted in one direction.
- After each expense, update net balances — do not store a transaction log as the source of truth for balances (though keeping a history is a bonus).

---

### 4. Precision Handling
- Demonstrate awareness of floating-point pitfalls: round to 2 decimals, assign rounding remainder deterministically.

---

## Constraints

- Data stored in memory only.
- No persistence or external storage.
- Concurrency is NOT required.
- No UI required.

---

## Example Scenario

```text
Users: u1 (Alice), u2 (Bob), u3 (Carol), u4 (Dave)

Action: u1 pays 1000, split EQUAL among u1, u2, u3, u4
Output: u2 owes u1: 250 | u3 owes u1: 250 | u4 owes u1: 250

Action: u1 pays 1250, split EXACT — u2: 370, u3: 880
Output: u2 owes u1: 620 | u3 owes u1: 1130 | u4 owes u1: 250

Action: u4 pays 1200, split PERCENT — u1: 40%, u2: 20%, u3: 20%, u4: 20%
Output:
  u1 owes u4: 230        (480 owed minus the 250 u4 owed u1)
  u2 owes u1: 620
  u2 owes u4: 240
  u3 owes u1: 1130
  u3 owes u4: 240

Action: u2 settles 240 with u4
Output: u2 owes u4: 0 (removed from balance sheet)
```

---

## Bonus Challenges (If Time Permits)

- Implement **groups** (e.g., "Goa Trip") with group-scoped expenses and balances.
- Add **simplify debts**: minimize the number of transactions needed to settle all balances.
- Maintain a **transaction history** per user with timestamps.
- Support **multi-currency** expenses with a fixed conversion-rate table.
