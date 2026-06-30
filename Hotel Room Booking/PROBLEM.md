# LLD Machine Coding – Hotel Room Booking System (C++)

## Problem Statement

Design and implement a **Hotel Room Booking System** using Object-Oriented Programming in C++.

This problem is asked at OYO, MakeMyTrip, Airbnb, and Booking.com engineering interviews. It tests your ability to handle **date-range-based inventory management**, **conflict detection**, and **pricing models** in a real-world reservation context.

---

## Functional Requirements

1. The system must manage:
   - **Hotels** (name, location, list of rooms)
   - **Rooms** (number, type, base price, amenities, status)
   - **Guests** (profile, booking history)
   - **Bookings** (guest, room, check-in/check-out, status, total cost)

2. Guests should be able to:
   - Search for available rooms in a hotel for a given date range.
   - Filter rooms by type (STANDARD, DELUXE, SUITE) and max price.
   - Book an available room for specified dates.
   - Cancel a booking (with refund rules based on notice period).
   - View current and past bookings.

3. The system should:
   - Prevent **double booking** the same room for overlapping dates.
   - Compute **total cost** for a booking (nightly rate × number of nights).
   - Apply **seasonal pricing** (peak season surcharge).
   - Generate a **booking confirmation** with a unique ID.

---

## Business Rules

- A room is considered available if it has **no confirmed booking** overlapping the requested dates.
- Booking dates are inclusive of check-in and exclusive of check-out (standard hotel convention).
- Cancellation policy:
  - > 7 days before check-in: 100% refund.
  - 3–7 days before check-in: 50% refund.
  - < 3 days before check-in: no refund.
- Seasonal pricing:
  - December 20 – January 5 (peak): 1.5× base price.
  - All other dates: base price.
- Room types and base nightly rates:
  - STANDARD: ₹2,000/night
  - DELUXE: ₹4,000/night
  - SUITE: ₹8,000/night
- A guest cannot have more than **5 active bookings** simultaneously.

---

## Non-Functional Requirements

- In-memory only (no database or file I/O).
- Standard C++ library only.
- Use `std::chrono` or a simple date struct for date handling.
- Follow OOP principles; avoid god classes.
- Console-based interaction is sufficient.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Date` (year, month, day with comparison operators)
- `DateRange` (check-in, check-out + overlap detection)
- `Room` (number, type, base price, amenities, list of bookings)
- `Hotel` (name, location, room collection)
- `Guest` (ID, name, active and past bookings)
- `Booking` (ID, guest, room, date range, total cost, status)
- `BookingManager` (search, create, cancel operations)
- `PricingEngine` (calculates cost based on dates and room type)

---

### 2. Overlap Detection
- `DateRange::overlaps(other: DateRange) → bool` is the core algorithmic piece.
- Two ranges overlap if `start1 < end2 && start2 < end1`.
- This must be O(n) over existing bookings per room (acceptable).

---

### 3. Pricing Engine
- `PricingEngine::calculateCost(room, dateRange) → double`
- Must correctly handle date ranges that span peak and non-peak periods (split billing).

---

### 4. Cancellation Logic
- `BookingManager::cancelBooking(bookingId, cancellationDate)` applies the refund policy.
- Refund amount must be computed and displayed.

---

### 5. Search & Filtering
- `BookingManager::searchRooms(hotel, dateRange, roomType, maxPrice)` returns available rooms.
- Room is available only if none of its confirmed bookings overlap the requested range.

---

## Constraints

- Data stored in memory only.
- No file system, database, or UI.
- Concurrency is NOT required.
- No real payment processing.

---

## Example Scenario

```text
Setup:
  Hotel: "Grand Palace, Mumbai"
  Rooms:
    R101 – STANDARD  – ₹2,000/night
    R201 – DELUXE    – ₹4,000/night
    R301 – SUITE     – ₹8,000/night

  Guests: Alice, Bob

Action: Alice searches available rooms
  Hotel: Grand Palace | Check-in: 2026-12-25 | Check-out: 2026-12-28 | Type: DELUXE
Output:
  R201 – DELUXE – ₹4,000/night (peak: ×1.5 = ₹6,000/night)
  Available ✓

Action: Alice books R201 (Dec 25–28)
  3 nights × ₹6,000 = ₹18,000
Output: Booking ID: BKG-001 | R201 | Dec 25–28 | ₹18,000

Action: Bob tries to book R201 (Dec 26–29)
Output: ERROR – R201 is not available for Dec 26–29 (overlap with BKG-001)

Action: Bob books R201 (Dec 29 – Jan 3)
  Dec 29–Jan 3 = 5 nights (all peak) → 5 × ₹6,000 = ₹30,000
Output: Booking ID: BKG-002 | R201 | Dec 29–Jan 3 | ₹30,000

Action: Alice cancels BKG-001 on 2026-12-20 (5 days before check-in)
  50% refund policy applies → refund ₹9,000
Output: Booking BKG-001 cancelled. Refund: ₹9,000
```

---

## Bonus Challenges (If Time Permits)

- Add **loyalty points** system (earn points per booking, redeem for discounts).
- Implement **room upgrades** (auto-upgrade guest to available higher tier at no cost).
- Support **group bookings** (multiple rooms under one booking ID).
- Add a **housekeeping schedule** — rooms need N hours between bookings.
