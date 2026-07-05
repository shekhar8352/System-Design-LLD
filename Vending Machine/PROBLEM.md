# LLD Machine Coding – Vending Machine (C++)

## Problem Statement

Design and implement a **Vending Machine** system using Object-Oriented Programming in C++.

This is a classic state machine problem frequently asked at Google, Amazon, and Adobe. It is ideal for evaluating **design patterns** (State, Strategy), **encapsulation**, and your ability to model a physical machine with strict behavioral rules.

---

## Functional Requirements

1. The vending machine must:
   - Hold an **inventory** of products, each with a name, price, and quantity.
   - Accept **coins/cash** from the user incrementally.
   - Allow the user to **select a product**.
   - **Dispense** the selected product if payment is sufficient.
   - Return **change** to the user.
   - Allow an **admin** to restock products and collect cash.

2. The machine must handle:
   - Inserting money before selecting a product.
   - Selecting a product that is out of stock.
   - Insufficient balance for the selected product.
   - Requesting a **refund** at any point before dispensing.
   - Dispensing change correctly.

---

## Business Rules

- Accepted coin denominations: ₹1, ₹2, ₹5, ₹10.
- Accepted note denominations: ₹20, ₹50, ₹100.
- Change is dispensed using the **fewest coins** possible from available denominations.
- If the machine cannot make exact change, the transaction is refused.
- An admin can:
  - Add new products.
  - Restock existing products.
  - Collect all cash in the machine.
  - View current inventory.
- The machine does **not** accept cash if no products are available.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP / State design pattern.
- No UI needed.

---

## Design Expectations (What Interviewers Look For)

### 1. State Machine (Core of this Problem)

The vending machine must exist in exactly one of these states at any time:
- `IDLE` — waiting for user interaction.
- `HAS_MONEY` — user has inserted money, product not yet selected.
- `PRODUCT_SELECTED` — user has selected a product.
- `DISPENSING` — machine is dispensing product and change.
- `OUT_OF_SERVICE` — machine is out of all products or has a fault.

State transitions must be explicit and guarded. Invalid operations in a given state must produce clear error messages.

---

### 2. Class Modelling

Expected core classes:
- `Product` (name, price, quantity)
- `Inventory` (map of product → quantity + restocking logic)
- `Coin` / `Denomination` enum (₹1, ₹2, ₹5, ₹10, ₹20, ₹50, ₹100)
- `CashRegister` (holds cash, dispenses change using greedy algorithm)
- `VendingMachine` (orchestrates all states and transitions)
- `State` interface / abstract class + concrete state classes (if using State pattern)

---

### 3. Change Dispensing
- `CashRegister` must track available denominations separately.
- Change dispensing must use a greedy algorithm on available denominations.
- If exact change cannot be made, the transaction must be rolled back.

---

### 4. Admin vs User Interface
- Separate methods or an `AdminPanel` class for admin operations.
- Normal user operations should not allow restocking or cash collection.

---

## Constraints

- Data stored in memory only.
- No file system, database, or UI.
- Concurrency is NOT required.
- Physical machine simulation only — no real timers.

---

## Example Scenario

```text
Machine Setup:
  Products:
    Cola    → ₹35 | qty: 5
    Chips   → ₹20 | qty: 3
    Water   → ₹15 | qty: 10
  Cash Register:
    ₹10 × 5, ₹5 × 10, ₹2 × 10, ₹1 × 10

--- User Session ---

Action: Insert ₹20
State: HAS_MONEY | Balance: ₹20

Action: Insert ₹20
State: HAS_MONEY | Balance: ₹40

Action: Select "Cola" (price ₹35)
Output: Product selected. ₹40 ≥ ₹35.
State: PRODUCT_SELECTED

Action: Confirm purchase
Output: Dispensing "Cola"...
        Change returned: ₹5 (one ₹5 coin)
        State: IDLE | Cola qty: 4

--- Another Session ---

Action: Insert ₹10
Action: Select "Cola" (price ₹35)
Output: ERROR – Insufficient balance. Please insert ₹25 more or press Refund.

Action: Press Refund
Output: ₹10 returned. State: IDLE

--- Admin Session ---

Action: Admin restocks Cola → +10 units
Action: Admin collects cash
Output: ₹35 collected. Register reset.
```

---

## Bonus Challenges (If Time Permits)

- Support **membership discounts** (registered users get 10% off).
- Add a **maintenance mode** where admin can clear faults.
- Implement **product expiry** — products older than a threshold are auto-removed.
- Track **sales history** and generate a revenue report.
