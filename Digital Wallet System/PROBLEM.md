# LLD Machine Coding – Digital Wallet System (C++)

## Problem Statement

Design and implement a **Digital Wallet System** (like Paytm/PhonePe wallet) supporting money loading, peer-to-peer transfers, idempotent transactions, and statement generation, using Object-Oriented Programming in C++.

This problem is asked at PhonePe, Razorpay, CRED, and Navi interviews. It tests **double-entry style ledger thinking**, **idempotency**, **transaction state machines**, and precise **money arithmetic**.

---

## Functional Requirements

1. User & wallet management:
   - `createUser(userId)` — creates a user with a wallet at balance 0.
2. Money operations:
   - `load(userId, amount, idempotencyKey)` — add money from an external source (assume the external payment always succeeds).
   - `transfer(fromUserId, toUserId, amount, idempotencyKey)` — atomic P2P transfer.
   - `withdraw(userId, amount, idempotencyKey)` — move money out to an external account.
3. Every operation produces immutable **ledger entries** (never update a past entry; corrections are new entries).
4. **Idempotency**: retrying an operation with the same `idempotencyKey` must return the original result and must NOT move money twice.
5. Queries:
   - `getBalance(userId)`.
   - `getStatement(userId, fromTime, toTime)` — chronological entries with running balance.
6. Support **transaction status** lookup by transactionId (`SUCCESS`, `FAILED`).

---

## Business Rules

- Money is represented in **integer minor units (paise/cents)** — floating point for money must be rejected in review.
- Transfer must be **atomic**: debit and credit both happen or neither does (insufficient balance → `FAILED`, no partial state).
- Amounts must be positive; self-transfer must be rejected.
- Balance can never go negative.
- A `FAILED` transaction is still recorded (with reason) and is still idempotent — retrying the same key returns the same failure.
- The **sum of all wallet balances** must always equal total loaded minus total withdrawn — an invariant you should be able to assert (conservation of money).
- Ledger entries are append-only and time-ordered per user.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; ledger/accounting logic must be separate from the user-facing service.
- No UI or real payment integration; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Money` (integer minor units; arithmetic + comparison; no implicit double conversion)
- `Wallet` / `Account` (userId, balance derived from or reconciled with ledger)
- `Transaction` (id, type, parties, amount, status, failure reason, timestamp, idempotencyKey)
- `LedgerEntry` (transactionId, accountId, DEBIT/CREDIT, amount, resulting balance, timestamp)
- `WalletService` (public API; orchestrates validation → ledger posting → status)
- `IdempotencyStore` (key → completed transaction result)
- Enums: `TransactionType` (LOAD, TRANSFER, WITHDRAW), `TransactionStatus`, `EntryType`

---

### 2. Ledger Discipline
- A transfer posts **two entries** (debit sender, credit receiver) under one transactionId.
- Balance is either maintained incrementally and **verifiable** by replaying the ledger, or computed from it — be able to defend the choice.

---

### 3. Idempotency Mechanics
- Key lookup happens **before** any validation side effects.
- Same key + different parameters → reject as a conflict (document this).

---

### 4. Failure Handling
- Insufficient balance produces a recorded `FAILED` transaction with **zero ledger entries**.
- Order of validation (user exists → amount valid → balance sufficient) should be deliberate and tested.

---

## Constraints

- Single-threaded; concurrency is NOT required (locking discussion is a bonus).
- No currency conversion (single currency).
- No KYC/limits in core (bonus below).
- No persistence.

---

## Example Scenario

```text
createUser(alice); createUser(bob)
load(alice, ₹1000.00, key=L1)      → SUCCESS | alice: 1000.00
load(alice, ₹1000.00, key=L1)      → returns SAME txn (idempotent) | alice still 1000.00

transfer(alice, bob, ₹300.00, key=T1) → SUCCESS | alice: 700.00, bob: 300.00
transfer(alice, bob, ₹900.00, key=T2) → FAILED (insufficient balance) | balances unchanged
transfer(alice, bob, ₹900.00, key=T2) → same FAILED result (idempotent)

withdraw(bob, ₹100.00, key=W1)     → SUCCESS | bob: 200.00

getStatement(alice):
  LOAD    +1000.00 → 1000.00
  TRANSFER -300.00 →  700.00

Invariant check: 700 + 200 = 1000 (loaded) − 100 (withdrawn) ✓
```

---

## Bonus Challenges (If Time Permits)

- Add **daily transaction limits** per user (count and amount), reset by an injected clock.
- Add **cashback/offer rules** (e.g., 1% cashback on transfers above ₹500) as pluggable Strategy objects posting their own ledger entries.
- Add **hold/capture flows** (reserve funds, then capture or release) with an expiry.
- Make operations **thread-safe** and discuss lock ordering to avoid deadlock on transfers (lock both accounts).
- Add **reversal/refund** as a compensating transaction linked to the original.
