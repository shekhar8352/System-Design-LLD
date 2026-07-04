# LLD Machine Coding – Parking Lot System (C++)

## Problem Statement

Design and implement a **Parking Lot Management System** using Object-Oriented Programming in C++.

This is one of the most frequently asked LLD problems in machine coding rounds at companies like Amazon, Google, Flipkart, and Uber. The focus is on **modelling a real-world system** with clean class design, not on algorithmic complexity.

---

## Functional Requirements

1. The parking lot has **multiple floors**, each floor has **multiple parking slots**.
2. Each slot is of a specific **vehicle type**:
   - `MOTORCYCLE` (2-wheeler)
   - `CAR` (4-wheeler)
   - `TRUCK` (heavy vehicle)
3. Vehicles of matching type can park in the corresponding slot type.
4. The system should support:
   - **Parking a vehicle**: assign the nearest available slot on the lowest floor.
   - **Unparking a vehicle**: free up the slot using a ticket.
   - **Generating a parking ticket** on entry (contains slot number, floor, vehicle info, entry time).
   - **Calculating parking fee** on exit (based on duration).
5. The system should support **querying**:
   - Number of free slots per floor per vehicle type.
   - All occupied slots.

---

## Business Rules

- A slot can hold **exactly one vehicle** at a time.
- Slot assignment must be **nearest-first** (lowest floor, lowest slot number).
- Fee calculation:
  - First hour: flat base fee per vehicle type.
  - Each additional hour (or part thereof): hourly rate.
  - Motorcycle: base ₹20, hourly ₹10
  - Car: base ₹40, hourly ₹20
  - Truck: base ₹100, hourly ₹50
- A vehicle cannot be parked twice without unparking first.

---

## Non-Functional Requirements

- In-memory only (no database or file I/O).
- No concurrency required.
- Console-based interaction is sufficient.
- Use standard C++ library only.
- Follow strict OOP principles.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Vehicle` (base or concrete, with `VehicleType` enum)
- `ParkingSlot` (knows its type, floor, number, and occupancy)
- `ParkingFloor` (holds a collection of slots)
- `ParkingLot` (singleton, manages floors)
- `Ticket` (issued on entry, used for exit)
- `FeeCalculator` (strategy pattern potential)

---

### 2. Single Responsibility
- `ParkingSlot` only knows about its own state.
- `ParkingFloor` only manages slots on that floor.
- `ParkingLot` orchestrates the overall operation.
- `FeeCalculator` is isolated for extensibility.

---

### 3. Extensibility
- Adding a new vehicle type should require minimal changes.
- Fee strategy should be swappable (e.g., flat fee, surge pricing).

---

### 4. Encapsulation
- Slot availability must not be modified from outside `ParkingSlot`.
- Ticket details must be immutable after generation.

---

## Constraints

- Data stored in memory only.
- No file system, database, or network usage.
- Concurrency is NOT required.
- No UI required.

---

## Example Scenario

```text
Parking Lot Setup:
  - 3 Floors
  - Each floor: 2 MOTORCYCLE slots, 3 CAR slots, 1 TRUCK slot

Action: Park a CAR with plate "KA-01-HH-9999"
Output: Ticket { ticket_id: T001, floor: 0, slot: 3, entry_time: 10:00 }

Action: Park a MOTORCYCLE with plate "MH-12-AB-1234"
Output: Ticket { ticket_id: T002, floor: 0, slot: 1, entry_time: 10:05 }

Action: Unpark using Ticket T001 at 12:30
Output: Fee = ₹40 (base) + ₹20 (1 extra hour for 2.5 hrs) = ₹60 (wait till next hour rounding)
        Slot 3 on Floor 0 is now FREE.

Action: Query free CAR slots on Floor 0
Output: 3 free CAR slots
```

---

## Bonus Challenges (If Time Permits)

- Add support for **reserved/VIP slots**.
- Implement a **display board** per floor showing free slot counts.
- Add **entry/exit gates** as separate entities.
- Support **monthly pass** holders (flat monthly fee, guaranteed slot).
