# LLD Machine Coding – Online Food Ordering System (C++)

## Problem Statement

Design and implement an **Online Food Ordering System** (mini Swiggy/Zomato) where customers browse restaurants, place orders, track order status, and get billed, using Object-Oriented Programming in C++.

This problem is asked at Swiggy, Zomato, Uber, and Amazon interviews. It tests **catalog modelling**, **order lifecycle / state machine**, **assignment of delivery partners**, and separation of restaurant, order, and delivery concerns.

---

## Functional Requirements

1. The system manages **restaurants**, each with a **menu** of items (name, price, veg/non-veg, availability).
2. Customers can:
   - **Search / list** restaurants (optionally filter by cuisine or location/zone).
   - **View menu** of a restaurant.
   - **Place an order** with one or more menu items and quantities.
   - **Cancel** an order only before it is prepared/out for delivery (define a clear cutoff).
   - **Track** order status.
3. Order statuses must progress through a lifecycle, e.g.:
   `PLACED → CONFIRMED → PREPARING → OUT_FOR_DELIVERY → DELIVERED`
   (and `CANCELLED` as a terminal side path).
4. On confirmation, the system assigns an available **delivery partner**.
5. Billing: sum of item prices × quantities + optional taxes/delivery fee; support simple **coupon/discount** (percentage or flat).
6. Restaurants can mark menu items **unavailable**; unavailable items cannot be ordered.

---

## Business Rules

- An order belongs to exactly one restaurant; all items must be from that restaurant's menu.
- Order placement fails if any requested item is unavailable or quantity ≤ 0.
- Only the restaurant (or system admin simulation) can move status forward: confirm → prepare → ready/out for delivery.
- Delivery partner assignment picks from partners who are **AVAILABLE** in the same zone; assigned partner becomes **BUSY** until delivery completes.
- Cancellation:
  - Allowed in `PLACED` or `CONFIRMED`.
  - Rejected once `PREPARING` or later.
- Delivered and cancelled orders are terminal — no further transitions.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; order status transitions must be validated (State pattern or guarded transitions).
- No UI / maps / GPS required; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `User` / `Customer` (id, name, address/zone)
- `Restaurant` (id, name, zone, cuisine, menu)
- `MenuItem` (id, name, price, veg flag, available)
- `Order` (id, customer, restaurant, line items, status, pricing breakdown)
- `OrderItem` (menuItemId, quantity, unitPrice snapshot)
- `DeliveryPartner` (id, name, zone, status)
- `OrderService` / `FoodOrderingService` (place, cancel, updateStatus, assignPartner)
- `PricingService` or strategy for bill + coupon
- `OrderStatus` enum

---

### 2. Order Lifecycle
- Centralize allowed transitions (map/set of from→to) so invalid jumps are impossible.
- Snapshot item prices into `OrderItem` at place-time so later menu price changes do not alter past orders.

---

### 3. Delivery Assignment
- Simple strategy: first available partner in the restaurant's zone.
- On `DELIVERED` or `CANCELLED` after assignment, free the partner back to AVAILABLE.
- Discuss what happens if no partner is available (queue order, or keep CONFIRMED until one frees up).

---

### 4. Extensibility
- Adding a new coupon type or payment mode should not rewrite the order core.
- Menu and restaurant catalog should be independently updatable.

---

## Constraints

- Single-threaded; concurrency is NOT required.
- No real payment gateway, maps, or ETA calculation.
- Ratings/reviews and multi-restaurant carts are NOT required (bonus).

---

## Example Scenario

```text
Setup: Restaurant "SpiceHub" (zone=Z1)
       Menu: Butter Chicken ₹300, Paneer Tikka ₹250
       Partners: D1 (Z1, AVAILABLE), D2 (Z2, AVAILABLE)

Action: placeOrder(C1, SpiceHub, [Butter Chicken×2, Paneer Tikka×1])
        → Order O1 PLACED | Bill: 300*2 + 250 = ₹850

Action: confirm(O1) → CONFIRMED | assign D1 (now BUSY)
Action: updateStatus(O1, PREPARING) → OK
Action: updateStatus(O1, OUT_FOR_DELIVERY) → OK
Action: updateStatus(O1, DELIVERED) → OK | D1 AVAILABLE again

Action: placeOrder with unavailable item → REJECTED
Action: cancel(O1) after PREPARING → REJECTED
```

---

## Bonus Challenges (If Time Permits)

- Support **multi-restaurant cart** (separate orders per restaurant from one checkout).
- Add **ETA estimation** based on preparation time + distance stub.
- Implement **surge delivery fee** by zone/time via strategy.
- Add **ratings** for restaurants and partners after delivery.
- Model **restaurant capacity** (max concurrent preparing orders).
