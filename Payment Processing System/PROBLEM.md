# LLD Machine Coding – Payment Processing System with Idempotency & Retries (C++)

## Problem Statement

Design and implement a **Payment Processing System** that routes payments through pluggable payment methods, guarantees **exactly-once semantics via idempotency keys**, handles gateway failures with retries, and supports refunds, using Object-Oriented Programming in C++.

This problem is asked at Stripe, Razorpay, PhonePe, and Amazon Pay interviews. It tests **payment state machines**, **idempotent API design**, **Strategy pattern for payment methods**, and reconciliation thinking.

---

## Functional Requirements

1. Payment initiation:
   - `pay(idempotencyKey, orderId, amount, method, details)` — methods: `CARD`, `UPI`, `WALLET`.
   - Returns a `paymentId` and status; the gateway call is simulated by an injected `GatewayClient` that can return `SUCCESS`, `DECLINED`, or `TIMEOUT` deterministically for tests.
2. Idempotency:
   - Retrying `pay` with the **same idempotency key** must return the original result — never charge twice.
   - The same key with **different request parameters** is rejected (`IDEMPOTENCY_CONFLICT`).
3. Failure handling:
   - `TIMEOUT` leaves the payment `PENDING_CONFIRMATION`; `reconcile(paymentId)` later queries the gateway for the true outcome and settles the state.
   - `DECLINED` is terminal for that attempt; a **new attempt** requires a new idempotency key.
   - Automatic retry policy for timeouts: up to `maxRetries` with backoff (simulated clock, no sleeping).
4. Refunds:
   - `refund(idempotencyKey, paymentId, amount)` — full or partial; total refunds can never exceed the captured amount; refunds are idempotent by key.
5. Queries:
   - Payment status and attempt history; all payments for an order; daily summary (counts + amounts by status).

---

## Business Rules

- Payment states: `CREATED → PROCESSING → SUCCEEDED | DECLINED | PENDING_CONFIRMATION (→ SUCCEEDED | DECLINED)`; refund states: `REFUND_INITIATED → REFUNDED | REFUND_FAILED`. Illegal transitions rejected.
- One order may have multiple payment attempts, but at most **one** `SUCCEEDED` payment; a `pay` for an already-paid order is rejected (`ORDER_ALREADY_PAID`).
- Amounts are integer paise — no floating point.
- Refund against a non-`SUCCEEDED` payment is rejected.
- Every state change is recorded in an append-only `LedgerEntry` list (who/when/what), never edited.
- Method-specific validation lives with the method: card number Luhn check, UPI id format, wallet balance check.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; adding a new payment method (e.g., NETBANKING) must require no change to the orchestrator.
- No real network/threads; gateway and clock are injected. Demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Payment` (id, orderId, amount, method, state, attempts, refundedTotal)
- `PaymentAttempt` (attempt #, gateway ref, outcome, timestamps)
- `PaymentMethod` (interface: validate, buildGatewayRequest) → `CardMethod`, `UpiMethod`, `WalletMethod`
- `GatewayClient` (interface: charge, queryStatus, refund) + `FakeGateway`
- `IdempotencyStore` (key → stored request hash + response)
- `PaymentService` (pay, refund, reconcile, queries)
- `Ledger` / `LedgerEntry`
- Enums: `PaymentState`, `GatewayResult`

---

### 2. Idempotency Done Right
- Store the request **fingerprint** and final response keyed by idempotency key.
- Handle the subtle case: a retry arriving while the original is `PENDING_CONFIRMATION` returns the pending state, not a new charge.

---

### 3. State Machine Rigor
- A central, table-driven transition validator. This is where most candidates lose points — ad-hoc `if` chains allow illegal transitions.

---

### 4. Reconciliation
- `reconcile` demonstrates you understand that a timeout is **unknown**, not failed: querying the gateway may reveal the charge succeeded.

---

## Constraints

- Single currency; single-threaded (discuss concurrency, don't implement it).
- No card vaulting/PCI concerns — details are opaque strings plus minimal validation.
- No persistence.

---

## Example Scenario

```text
pay(key=K1, order=O1, ₹500, CARD)   → gateway SUCCESS → P1 SUCCEEDED
pay(key=K1, order=O1, ₹500, CARD)   → replay → returns P1 SUCCEEDED (no new charge)
pay(key=K1, order=O1, ₹999, CARD)   → REJECTED (IDEMPOTENCY_CONFLICT)
pay(key=K2, order=O1, ₹500, UPI)    → REJECTED (ORDER_ALREADY_PAID)

pay(key=K3, order=O2, ₹800, UPI)    → gateway TIMEOUT → P2 PENDING_CONFIRMATION
pay(key=K3, ...)                    → replay → P2 PENDING_CONFIRMATION
reconcile(P2)                       → gateway says SUCCESS → P2 SUCCEEDED

refund(key=R1, P1, ₹200) → partial refund OK (refunded ₹200/₹500)
refund(key=R2, P1, ₹400) → REJECTED (exceeds remaining ₹300)
refund(key=R1, P1, ₹200) → replay → original result returned
```

---

## Bonus Challenges (If Time Permits)

- Add **gateway routing**: multiple gateways with health scores; route around a failing one.
- Add **idempotency key expiry** (keys valid 24h) with an injected clock.
- Add **webhook-style async confirmations** feeding the same state machine.
- Produce an **end-of-day reconciliation report** diffing ledger vs (fake) gateway records.
- Discuss exactly-once vs at-least-once, and why the ledger must be append-only.
