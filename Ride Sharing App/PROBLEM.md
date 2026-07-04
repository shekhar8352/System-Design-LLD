# LLD Machine Coding – Ride Sharing App (C++)

## Problem Statement

Design and implement a **Ride Sharing Application** (similar to Uber / Ola) using Object-Oriented Programming in C++.

This is a high-signal problem asked at Uber, Ola, Rapido, and Swiggy. It tests your ability to model **matching systems**, **real-time state tracking**, and **pricing strategies** in a service-oriented architecture.

---

## Functional Requirements

1. The system must manage:
   - **Riders** (passengers requesting rides)
   - **Drivers** (with their vehicles)
   - **Rides** (a matched session between one rider and one driver)

2. Riders should be able to:
   - Request a ride by providing source and destination.
   - View estimated fare before confirming.
   - Cancel a ride (before driver arrives).
   - View ride history.

3. Drivers should be able to:
   - Go **online** (available) or **offline** (unavailable).
   - Accept or reject a ride request.
   - Mark a ride as **started** and then **completed**.
   - View their earnings.

4. The system should:
   - **Match** the nearest available driver to a ride request.
   - Calculate **fare** based on distance and ride type.
   - Maintain a **rating system** (rider rates driver, driver rates rider) after ride completion.

---

## Business Rules

- A driver must be **online** and **not on an active ride** to receive requests.
- Driver matching: nearest driver (by distance from rider's source) is preferred.
- If no driver is available within a threshold distance, return "No drivers available."
- Fare calculation:
  - Base fare: ₹30
  - Per km rate: ₹12/km (ECONOMY), ₹18/km (PREMIUM), ₹25/km (SUV)
  - Surge multiplier applies if demand > supply (model as a flag: `surgePricing = true/false`).
  - Surge multiplier: 1.5×.
- Ride lifecycle: `REQUESTED → ACCEPTED → IN_PROGRESS → COMPLETED` (or `CANCELLED`).
- A rider can only have **one active ride** at a time.
- Ratings are between 1.0 and 5.0. A driver's average rating is maintained.

---

## Non-Functional Requirements

- In-memory only.
- No real GPS — model location as `(x, y)` coordinates; use Euclidean distance.
- No real threading or timers.
- Standard C++ library only.
- Clean OOP design.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Location` (x, y coordinates + utility methods for distance)
- `Vehicle` (license plate, vehicle type: ECONOMY / PREMIUM / SUV)
- `Driver` (personal info, vehicle, current location, status, rating)
- `Rider` (personal info, current location, ride history)
- `Ride` (ID, rider, driver, source, destination, status, fare)
- `FareCalculator` (strategy-based, varies by vehicle type + surge)
- `RideManager` / `RideMatcher` (orchestrates matching and ride lifecycle)

---

### 2. Matching Strategy
- `RideMatcher` should be isolated and swappable (nearest-first vs rating-first).
- Matching should iterate over available drivers and find the best fit.

---

### 3. State Management
- `Ride` and `Driver` statuses must follow a strict state machine.
- Invalid transitions (e.g., completing a ride that wasn't started) must be rejected.

---

### 4. Extensibility
- Adding a new vehicle/pricing tier should not break existing logic.
- Surge pricing should be a toggleable strategy, not hardcoded.

---

## Constraints

- Location modelled as 2D Euclidean coordinates.
- In-memory only.
- No concurrency required.
- No UI needed.

---

## Example Scenario

```text
Setup:
  Drivers:
    D1 – John | ECONOMY | Location: (0, 0) | ONLINE
    D2 – Sara | PREMIUM | Location: (10, 10) | ONLINE
    D3 – Mike | SUV     | Location: (3, 4) | ONLINE

  Riders:
    R1 – Alice | Location: (2, 2)
    R2 – Bob   | Location: (9, 9)

Action: Alice requests ECONOMY ride from (2,2) to (8,2)
Output:
  Nearest ECONOMY driver: D1 at distance ~2.83
  Estimated fare: ₹30 + 6km × ₹12 = ₹102
  Ride ID: RIDE-001 assigned to D1

Action: D1 accepts RIDE-001
Output: Ride status → IN_PROGRESS

Action: D1 completes RIDE-001
Output: Ride status → COMPLETED | Final Fare: ₹102

Action: Alice rates D1 → 4.5
Action: D1 rates Alice → 5.0

Action: Bob requests PREMIUM ride from (9,9) to (12,12)
Output: Nearest PREMIUM driver: D2 at distance ~1.41
  Estimated fare: ₹30 + 4.24km × ₹18 ≈ ₹106
  Ride ID: RIDE-002 assigned to D2
```

---

## Bonus Challenges (If Time Permits)

- Implement **ride pooling** (carpooling): match multiple riders going in the same direction.
- Add **scheduled rides** (book a ride for a future time).
- Support **driver tipping** after ride completion.
- Add a **leaderboard** of top-rated drivers in a city.
