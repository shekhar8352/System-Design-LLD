# LLD Machine Coding – Movie Ticket Booking System (C++)

## Problem Statement

Design and implement a **Movie Ticket Booking System** (similar to BookMyShow) using Object-Oriented Programming in C++.

This problem is frequently asked at Flipkart, Paytm, BookMyShow, and Razorpay. It evaluates your ability to model **inventory management**, **concurrent-safe booking**, and **multi-entity coordination** in a real product context.

---

## Functional Requirements

1. The system must manage:
   - **Movies** (title, duration, genre, language)
   - **Theatres** (name, location, screens)
   - **Screens** (seat layout)
   - **Shows** (a specific movie running on a specific screen at a specific time)
   - **Seats** (row, number, type: SILVER / GOLD / PLATINUM)
   - **Bookings** (user, show, selected seats)

2. Users should be able to:
   - Browse movies currently running in a city.
   - Select a show (date + time + theatre).
   - View the seat map for a show (available vs booked).
   - Book one or more seats for a show.
   - Cancel a booking.

3. The system should:
   - Generate a **booking confirmation** with a unique booking ID.
   - Calculate **total price** based on seat type and show pricing.
   - Prevent **double-booking** the same seat for the same show.

---

## Business Rules

- A seat is either `AVAILABLE`, `LOCKED`, or `BOOKED`.
- A seat gets `LOCKED` temporarily during the booking process (simulate with a flag; no real timers needed).
- Once payment is confirmed (simulate as a method call), seat becomes `BOOKED`.
- If booking is cancelled before the show time, seat returns to `AVAILABLE`.
- Seat pricing per show:
  - SILVER: base price
  - GOLD: 1.5× base price
  - PLATINUM: 2× base price
- A user can book at most **10 seats** per booking.

---

## Non-Functional Requirements

- In-memory only (no database).
- No real concurrency required; model locking as a flag.
- Console-based interaction.
- Standard C++ library only.
- Clean OOP — avoid god classes.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Movie`
- `Theatre`
- `Screen`
- `Show` (links Movie + Screen + date/time + pricing)
- `Seat` (has type, row, number, status)
- `Booking` (user ID, show, list of seats, total price, status)
- `BookingManager` (orchestrates the booking flow)

---

### 2. Status State Machine
- `Seat` status transitions: `AVAILABLE → LOCKED → BOOKED` (or back to `AVAILABLE` on cancel).
- `Booking` status: `PENDING → CONFIRMED → CANCELLED`.

---

### 3. Search & Filtering
- `BookingManager::getShowsForMovieInCity(movie, city)` — returns available shows.
- Results should be filterable by date, language, screen type.

---

### 4. Encapsulation
- Seat status must only be changed through controlled methods.
- Booking creation must validate seat availability before locking.

---

### 5. Separation of Concerns
- Payment processing should be a separate component (even if stubbed).
- Seat map rendering is separate from booking logic.

---

## Constraints

- Data stored in memory only.
- No file system, database, or UI.
- Concurrency is NOT required.
- No actual payment integration.

---

## Example Scenario

```text
Setup:
  Movie: "Interstellar" | Duration: 169 min
  Theatre: "PVR Cinemas, Bangalore" | 1 Screen (10 rows × 12 seats)
    Row A–D: SILVER (base ₹150), Row E–H: GOLD, Row I–J: PLATINUM

  Show: Interstellar @ PVR, 2026-07-05, 18:00
    Base price: ₹150

Action: User "alice" searches movies in "Bangalore"
Output: ["Interstellar" @ PVR, 18:00 – 10 SILVER, 6 GOLD, 3 PLATINUM available]

Action: User "alice" views seat map for the show
Output: Seat grid with AVAILABLE/BOOKED indicators

Action: User "alice" selects seats A1, A2, A3 (SILVER)
Output: Seats locked. Invoice: 3 × ₹150 = ₹450

Action: Payment confirmed
Output: Booking ID: BK-0001 | Seats: A1, A2, A3 | Show: Interstellar 18:00 | Total: ₹450

Action: User "alice" cancels BK-0001
Output: Booking cancelled. Seats A1, A2, A3 are now AVAILABLE.
```

---

## Bonus Challenges (If Time Permits)

- Add **waiting list** support when all seats are booked.
- Implement **group discounts** (>5 seats gets 10% off).
- Add different **food combo** add-ons tied to a booking.
- Support **screen types**: 2D, 3D, IMAX with different base pricing.
