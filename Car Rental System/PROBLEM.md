# LLD Machine Coding – Car Rental System (C++)

## Problem Statement

Design and implement a **Car Rental System** where customers can search available vehicles, book them for a date range, pick up, return, and get billed, using Object-Oriented Programming in C++.

This problem is asked at Uber, Amazon, Flipkart, and Adobe interviews. It tests **inventory modelling**, **reservation/overlap logic**, **pricing strategies**, and clean **booking lifecycle** design.

---

## Functional Requirements

1. The system manages a fleet of **vehicles** across one or more **branches/locations**.
2. Vehicles have attributes: id, type (Hatchback, Sedan, SUV, Luxury), make/model, license plate, status.
3. Customers can:
   - **Search** available vehicles by location, type, and date range.
   - **Create a reservation** for a vehicle over a date range.
   - **Cancel** a reservation (before pickup).
   - **Pickup** a reserved vehicle.
   - **Return** a vehicle and generate a bill.
4. Pricing is based on **vehicle type** and **rental duration** (daily rate × days), with optional add-ons (GPS, child seat, insurance).
5. The system must prevent **double-booking** the same vehicle for overlapping date ranges.
6. Support querying: active reservations for a customer, vehicle availability for a range, and revenue for a branch (simple sum of completed bookings).

---

## Business Rules

- A vehicle is **AVAILABLE**, **RESERVED**, **RENTED**, or **MAINTENANCE**.
- Search returns only vehicles that are free for the **entire** requested interval (no overlap with existing reservations/rentals).
- Reservation requires: customer, vehicle, start date, end date (end > start).
- Pickup is allowed only on/after the reservation start and only for that reservation's vehicle.
- Return computes bill from actual rental days (at least 1 day); late return may add a **late fee** (bonus or simple fixed fee).
- Cancelled reservations free the vehicle for those dates again.
- Two reservations overlap if: `startA < endB && startB < endA` (half-open or closed — pick one and be consistent).

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; pricing and vehicle types must be extensible without rewriting booking logic.
- No UI required; demonstrate via `main()` with a simulated calendar/clock if needed.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Vehicle` (abstract or base) + type-specific subclasses or a `VehicleType` enum with rate table
- `Branch` / `Location` (id, name, list of vehicles)
- `Customer` (id, name, licenseNumber)
- `Reservation` (id, customer, vehicle, start, end, status)
- `Booking` / `Rental` (reservation reference, pickup time, return time, bill)
- `PricingStrategy` (abstract: `calculate(vehicle, days, addOns)`)
- `CarRentalService` (facade: search, reserve, cancel, pickup, return)
- `ReservationStatus` enum (`CREATED`, `CANCELLED`, `PICKED_UP`, `COMPLETED`)

---

### 2. Availability & Overlap
- Availability check must consider all non-cancelled reservations for that vehicle.
- Do not rely only on a single `status` flag — a vehicle can have future reservations while currently AVAILABLE.

---

### 3. Pricing Strategy
- Base daily rates per vehicle type.
- Add-ons as optional line items.
- `CarRentalService` depends on `PricingStrategy`, not hardcoded rates in the return flow.

---

### 4. Booking Lifecycle
- Clear state transitions: Created → PickedUp → Completed, or Created → Cancelled.
- Invalid transitions (e.g., return without pickup) must be rejected with a reason.

---

## Constraints

- Single-threaded; concurrency is NOT required (discuss race on double-book as bonus).
- No payment gateway integration — billing is computed and recorded in memory.
- GPS tracking / damage assessment are NOT required.

---

## Example Scenario

```text
Setup: Branch "Airport"
       V1 Sedan (₹2000/day), V2 SUV (₹3500/day)

Action: search(Airport, SEDAN, Jul10–Jul12)
        → [V1]

Action: reserve(customer=C1, vehicle=V1, Jul10–Jul12) → R1 created
Action: search(Airport, SEDAN, Jul11–Jul13) → []  (V1 overlaps)
Action: search(Airport, SEDAN, Jul12–Jul14) → [V1] (no overlap if end-exclusive)

Action: pickup(R1) → V1 status RENTED
Action: return(R1, actualEnd=Jul12) → Bill: 2 × 2000 = ₹4000 | V1 AVAILABLE

Invalid examples:
reserve(V1, Jul10–Jul11) while R1 active → REJECTED (overlap)
pickup(R1) twice → REJECTED
cancel after pickup → REJECTED
```

---

## Bonus Challenges (If Time Permits)

- Support **hourly rentals** in addition to daily.
- Add **loyalty discounts** or weekend surge pricing via strategy composition.
- Implement **waitlist** when no vehicle is available for the range.
- Model **maintenance windows** that block availability.
- Make reservation creation **thread-safe** and discuss optimistic vs pessimistic locking.
