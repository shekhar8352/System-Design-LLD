# LLD Machine Coding – Warehouse Inventory Management System (C++)

## Problem Statement

Design and implement a **Warehouse Inventory Management System** that tracks stock across warehouses, handles reservations for orders with expiry, supports batch/expiry-date-aware picking (FIFO/FEFO), and triggers low-stock replenishment alerts, using Object-Oriented Programming in C++.

This problem is asked at Amazon, Flipkart, Udaan, and Zepto interviews. It tests **inventory state partitioning (on-hand vs reserved vs available)**, **allocation strategies**, **reservation lifecycle with expiry**, and event-driven alerts.

---

## Functional Requirements

1. Catalog & stock:
   - `addSku(skuId, name, replenishThreshold)`.
   - `receiveStock(warehouseId, skuId, quantity, batchId, expiryDate?)` — stock arrives in batches; perishables carry expiry dates.
   - Per SKU per warehouse: **on-hand**, **reserved**, **available = on-hand − reserved**.
2. Reservations (order flow):
   - `reserve(orderId, [(skuId, qty)], ttl)` — atomically reserve all lines or fail the whole request (no partial reservations); reservations expire after `ttl` (injected clock).
   - `confirm(orderId)` — converts the reservation to a **pick**: decrements on-hand using the allocation strategy.
   - `release(orderId)` — cancels the reservation, returning quantity to available.
   - `processExpiredReservations(now)` — sweeps and releases expired holds.
3. Allocation strategies (per SKU):
   - **FIFO** (oldest received batch first) or **FEFO** (earliest expiry first, for perishables).
   - Multi-warehouse: `reserve` may specify a warehouse, or the system picks by priority order (e.g., most stock first) — document your policy.
4. Stock operations:
   - `transfer(skuId, qty, fromWh, toWh)` — only from available stock.
   - `writeOff(warehouseId, skuId, batchId, qty, reason)` — damage/expiry disposal.
   - `removeExpiredBatches(now)` — expired batch quantities become non-available and awaiting write-off.
5. Alerts & queries:
   - When available stock for a SKU (across warehouses) drops below `replenishThreshold`, emit a **low-stock alert** exactly once until replenished above it (Observer).
   - Queries: stock breakdown per SKU per warehouse per batch; reservation status; movement history (audit trail of every stock change).

---

## Business Rules

- Available stock can never go negative; reserve/confirm/transfer validate against **available**, not on-hand.
- Confirm after expiry is rejected (`RESERVATION_EXPIRED`); confirm/release are idempotent per order.
- Expired batches must never be picked, even if that fails an otherwise-satisfiable confirm.
- A batch's quantity is consumed strictly per the strategy; a single pick may span multiple batches.
- Every mutation appends a `StockMovement` record (type, sku, batch, qty, warehouses, orderId?, timestamp) — the audit trail must fully explain current stock.
- Duplicate `orderId` reservations are rejected.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; allocation strategy is per-SKU pluggable (Strategy pattern); alerting decoupled via Observer.
- No threads; time injected. Demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Sku` (id, name, threshold, strategy choice)
- `Batch` (batchId, qty remaining, receivedAt, expiryDate?)
- `WarehouseStock` (per sku: batches ordered per strategy, reserved count)
- `Reservation` (orderId, lines, state: HELD / CONFIRMED / RELEASED / EXPIRED, deadline)
- `AllocationStrategy` (interface) → `FifoStrategy`, `FefoStrategy`
- `InventoryService` (public API)
- `StockMovement` + `MovementLog`
- `StockObserver` (interface) + `ReplenishmentAlerter`

---

### 2. Reserved vs On-Hand Discipline
- Reservation holds **quantity**, not specific batches; batches are chosen at **confirm** time. Explain why (expiry between reserve and confirm changes the right batch).

---

### 3. All-or-Nothing Reserve
- Validate every line against available before mutating anything; a mid-way failure must leave zero side effects.

---

### 4. Alert Edge-Triggering
- Alert fires on the **crossing** of the threshold, not on every subsequent decrement — requires remembering alert state per SKU.

---

## Constraints

- Single-threaded; no persistence; quantities are integers.
- No pricing, purchasing, or shipping logistics.
- ≤ 10 warehouses; batch counts small enough for clarity over micro-optimization.

---

## Example Scenario

```text
addSku(MILK, threshold=20, strategy=FEFO)
receiveStock(WH1, MILK, 30, B1, expiry=d5)
receiveStock(WH1, MILK, 50, B2, expiry=d3)   ← expires sooner

reserve(O1, [(MILK, 40)], ttl=2h) → HELD; available 80−40=40
confirm(O1) → FEFO picks B2 first: 40 from B2 → B2 left 10, B1 left 30
              movement log: PICK O1 (B2:40)

reserve(O2, [(MILK, 45)], ttl=2h) → REJECTED (available 40 < 45), nothing held
reserve(O3, [(MILK, 25)], ttl=1h) → HELD
processExpiredReservations(+2h)   → O3 EXPIRED, 25 back to available

removeExpiredBatches(d4) → B2's 10 units quarantined → available 30
   → 30 > 20, no alert yet
confirm(O4 for 15) → available 15 < 20 → LOW-STOCK ALERT (fires once)
receiveStock(WH1, MILK, 100, B3) → above threshold; alert re-armed
```

---

## Bonus Challenges (If Time Permits)

- Add **safety stock** distinct from the replenishment threshold.
- Auto-generate **purchase orders** on alert, and receive against them.
- Add **cycle counting**: reconcile physical counts vs system stock with adjustment movements.
- Support **substitutable SKUs** (fulfill MILK-1L with 2×MILK-500ML) behind a policy.
- Discuss concurrent order bursts on the last unit — where a real system needs locking or atomic compare-and-set.
