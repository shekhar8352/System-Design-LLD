# LLD Machine Coding – Stock Exchange Order Matching Engine (C++)

## Problem Statement

Design and implement an **in-memory Stock Exchange Order Matching Engine** that accepts buy/sell orders and matches them according to price-time priority, using Object-Oriented Programming in C++.

This problem is asked at Jane Street, Goldman Sachs, Groww, Zerodha, and Uber interviews. It tests **order book data structures**, **matching algorithms**, **partial fills**, and clean separation between order management and trade execution.

---

## Functional Requirements

1. Support placing **LIMIT** orders:
   - `BUY` — willing to buy up to `quantity` shares at price ≤ `limitPrice`.
   - `SELL` — willing to sell up to `quantity` shares at price ≥ `limitPrice`.
2. Each order has: orderId, userId, side (BUY/SELL), type (LIMIT), price, quantity, timestamp, status.
3. On place, the engine attempts to **match immediately** against the opposite side of the book; unmatched remainder rests on the book.
4. Matching rules (**price-time priority**):
   - A buy matches the **lowest** ask that is ≤ buy price.
   - A sell matches the **highest** bid that is ≥ sell price.
   - Among equal prices, earlier orders (FIFO) match first.
5. Support **partial fills**: an order may be filled across multiple trades until quantity is exhausted or no more matches exist.
6. Support **cancel** of an open (unfilled/partially filled) order; remove remaining quantity from the book.
7. Query APIs:
   - Best bid / best ask.
   - Order status (open quantity, filled quantity).
   - Trade history (or last N trades).

---

## Business Rules

- Trade price is the **resting order's price** (the order already on the book) — state this clearly.
- After a trade of size `q`:
  - Both orders reduce remaining quantity by `q`.
  - Fully filled orders are removed from the book and marked `FILLED`.
  - Partially filled orders stay on the book with reduced quantity (`PARTIALLY_FILLED`).
- Cannot match two orders from the same user if you choose self-trade prevention (optional; document if enabled).
- Cancel fails for already `FILLED` or `CANCELLED` orders.
- Quantity and price must be positive; reject invalid orders.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only (`map`, `queue`/`deque`, `unordered_map`, etc.).
- Follow OOP principles; matching logic should live in an `OrderBook` / `MatchingEngine`, not scattered in `main`.
- No UI / network; demonstrate via `main()` with a sequence of orders and printed trades.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Order` (id, userId, side, price, quantity, remaining, timestamp, status)
- `Trade` (tradeId, buyOrderId, sellOrderId, price, quantity, timestamp)
- `OrderBook` (bids, asks, match logic)
- `MatchingEngine` / `Exchange` (placeOrder, cancelOrder, getOrder, getTrades, bestBidAsk)
- `OrderSide` enum (`BUY`, `SELL`)
- `OrderStatus` enum (`OPEN`, `PARTIALLY_FILLED`, `FILLED`, `CANCELLED`)

---

### 2. Order Book Structure
- Bids: price levels descending; each level is a FIFO queue of orders.
- Asks: price levels ascending; each level is a FIFO queue of orders.
- Typical C++ approach: `std::map` with custom comparator, value = `std::queue<Order*>` or list.

---

### 3. Matching Loop
- While opposite top-of-book is matchable and incoming has remaining qty:
  - Execute trade at resting price for `min(incoming.remaining, resting.remaining)`.
  - Update/remove resting order; advance to next if level empties.
- Place leftover incoming quantity on its side of the book.

---

### 4. Correctness Details Interviewers Probe
- Partial fills across multiple price levels.
- Cancel mid-book without breaking FIFO at that price.
- Idempotent handling of duplicate cancel.

---

## Constraints

- LIMIT orders only for the core solution (MARKET orders are bonus).
- Single instrument (one symbol) is enough; multi-symbol is bonus.
- Single-threaded; concurrency is NOT required.
- No persistence, fees, or margin checks required.

---

## Example Scenario

```text
Action: SELL 10 @ 100 (Order S1) → rests on ask
Action: SELL 5  @ 101 (Order S2) → rests on ask
Action: BUY  8  @ 101 (Order B1)
        → Trade1: B1↔S1 qty=8 @ 100
        → S1 remaining 2 @ 100 | B1 FILLED

Action: BUY  5  @ 100 (Order B2)
        → Trade2: B2↔S1 qty=2 @ 100
        → Trade3: none more at ≤100; B2 remaining 3 rests on bid @ 100
        → S1 FILLED

Book now: BID 100×3 (B2) | ASK 101×5 (S2)

Action: cancel(B2) → B2 CANCELLED | best bid empty
Action: BUY 5 @ 101 → Trade with S2 @ 101 qty=5 | both FILLED
```

---

## Bonus Challenges (If Time Permits)

- Add **MARKET** orders (match at any price until filled or book empty; reject leftover or cancel remainder).
- Support **multiple symbols**, each with its own order book.
- Add **stop-loss** / stop-limit orders triggered by last trade price.
- Emit **market data** callbacks (Observer) on best bid/ask changes.
- Discuss lock-free or sharded books for multi-threaded matching.
