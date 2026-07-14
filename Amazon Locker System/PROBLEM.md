# LLD Machine Coding – Amazon Locker System (C++)

## Problem Statement

Design and implement an **Amazon Locker System** where packages are assigned to appropriately sized lockers at a pickup location, customers pick them up with a one-time code, and unclaimed packages are returned after expiry, using Object-Oriented Programming in C++.

This problem is asked at Amazon, Flipkart, and Walmart interviews. It tests **resource allocation with size constraints**, **code generation & validation**, **expiry handling with an injected clock**, and clean locker state machines.

---

## Functional Requirements

1. Locker setup:
   - A location has lockers of sizes `SMALL`, `MEDIUM`, `LARGE`, `XL`, each identified by lockerId.
   - `addLocker(locationId, lockerId, size)`; lockers can be marked `OUT_OF_SERVICE`.
2. Package assignment:
   - `assignLocker(packageId, size, locationId)` — allocate the **smallest available locker** that fits the package.
   - On success, generate a **6-digit one-time pickup code** and a pickup deadline (e.g., 3 days from assignment).
   - If no locker fits, return a clear failure (caller may try another location).
3. Pickup:
   - `pickup(locationId, code)` — validates the code, opens the locker, marks it available again.
   - Wrong codes fail without revealing which locker/package they were for; 3 consecutive wrong attempts for a package **lock pickup** until re-issued.
4. Expiry:
   - `processExpiry(now)` — packages past their deadline are marked `RETURNED`, lockers freed, codes invalidated.
5. Queries:
   - Locker availability by size per location; package status (`ASSIGNED`, `PICKED_UP`, `RETURNED`); audit log of locker events.

---

## Business Rules

- A package may occupy exactly one locker; a locker holds at most one package.
- Smallest-fit allocation: a `SMALL` package prefers `SMALL`, then `MEDIUM`, etc. — never the reverse.
- Codes are single-use: after successful pickup the code must be rejected forever.
- A code is valid only at the location where the package was placed.
- `OUT_OF_SERVICE` lockers are never allocated; marking an **occupied** locker out-of-service is rejected until it is emptied.
- Re-issuing a code (after attempt lockout) invalidates the old code and resets the attempt counter.
- All time-based behaviour uses an injected clock — no real timers.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; allocation strategy must be swappable (e.g., smallest-fit vs nearest-to-entrance).
- No UI/network; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Locker` (id, size, state: AVAILABLE / OCCUPIED / OUT_OF_SERVICE)
- `LockerLocation` (lockers grouped/indexed by size)
- `Package` (id, size, state, assigned locker, deadline)
- `PickupCode` (code, packageId, state, wrong-attempt counter)
- `AllocationStrategy` (interface) + `SmallestFitStrategy`
- `LockerService` (assign, pickup, processExpiry, queries)
- `CodeGenerator` (injectable for deterministic tests)
- Enums: `Size`, `LockerState`, `PackageState`

---

### 2. Allocation Efficiency
- Availability indexed per size (e.g., `map<Size, set<lockerId>>`) so allocation is O(log n), not a scan of every locker.
- Free/occupy operations must keep the index consistent.

---

### 3. Code Lifecycle
- One-time semantics, attempt lockout, re-issue, and expiry invalidation modeled explicitly — not with scattered booleans.

---

### 4. Expiry Sweep
- `processExpiry(now)` should find due packages efficiently (deadline-ordered structure), not iterate all packages.

---

## Constraints

- Single-threaded; concurrency is NOT required.
- No payments, delivery routing, or user accounts beyond an id.
- Code uniqueness is required only among **active** codes.

---

## Example Scenario

```text
Location L1: 2 SMALL, 1 MEDIUM, 1 LARGE lockers

assignLocker(P1, SMALL)  → locker S1, code 483920, deadline t+3d
assignLocker(P2, SMALL)  → locker S2
assignLocker(P3, SMALL)  → no SMALL left → MEDIUM M1 (smallest fit)
assignLocker(P4, LARGE)  → L1
assignLocker(P5, MEDIUM) → REJECTED (M1 occupied, LARGE occupied)

pickup(L1, 111111) ×3 for P1 → code locked → reissueCode(P1) → 771204
pickup(L1, 771204) → SUCCESS → S1 AVAILABLE

processExpiry(t+4d) → P2 RETURNED, S2 freed, its code invalid
```

---

## Bonus Challenges (If Time Permits)

- Support **multi-location search**: find the nearest location with a fitting locker.
- Add **refrigerated lockers** as a locker attribute matched against package needs.
- Add **delivery-agent flow**: separate drop-off codes vs customer pickup codes.
- Emit locker events through an **Observer** (notifications, audit sink).
- Discuss sizing analytics: which locker sizes run out most often, and rebalancing.
