# LLD Machine Coding – Meeting Room Scheduler (C++)

## Problem Statement

Design and implement a **Meeting Room Scheduler** (calendar booking system) that lets users find available rooms, book meetings, cancel them, and detect scheduling conflicts, using Object-Oriented Programming in C++.

This problem is asked at Google, Amazon, Microsoft, and Atlassian interviews. It tests **interval overlap logic**, **room inventory**, **calendar modelling**, and clean API design for booking systems.

---

## Functional Requirements

1. The system manages multiple **meeting rooms** with capacity and optional amenities (projector, video conf, whiteboard).
2. Users can:
   - **Search** rooms available for a time range with minimum capacity and required amenities.
   - **Book** a room for a meeting (title, organizer, attendees, start, end).
   - **Cancel** a booking.
   - **Reschedule** a booking to a new time (and optionally a new room).
3. List all meetings for a given room on a given day, and all meetings for a given user.
4. Detect whether a user already has a **conflicting meeting** in the requested interval (optional but expected in strong solutions).
5. Support querying the next available slot for a room after a given time (simple scan is fine).

---

## Business Rules

- Meeting interval is valid only if `start < end`.
- A room cannot have two meetings with **overlapping** intervals.
- Overlap rule: `startA < endB && startB < endA` (document whether endpoints are inclusive).
- Booking fails if:
  - Room capacity < number of attendees, **or**
  - Required amenities are missing, **or**
  - Time range conflicts with an existing booking on that room.
- Cancelled meetings do not block the room.
- Past meetings can be listed but not modified (or allow cancel only for future meetings — pick one).

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; keep room search criteria extensible (capacity, amenities, floor, building).
- No UI required; demonstrate via `main()` with explicit timestamps (epoch or `chrono`).

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Room` (id, name, capacity, amenities set, building/floor optional)
- `User` (id, name, email)
- `Meeting` / `Booking` (id, room, organizer, attendees, title, start, end, status)
- `TimeInterval` (start, end) with `overlaps(other)` helper
- `Scheduler` / `CalendarService` (search, book, cancel, reschedule, listMeetings)
- `BookingStatus` enum (`SCHEDULED`, `CANCELLED`)
- Optional: `RoomFilter` / specification object for search criteria

---

### 2. Overlap & Data Structures
- Per-room sorted list/map of meetings by start time makes conflict checks clearer.
- Be ready to discuss O(n) scan vs interval tree for large calendars (implement simple scan).

---

### 3. Search API Design
- Search should accept a criteria object rather than a long parameter list.
- Return rooms that satisfy capacity + amenities + availability for the full interval.

---

### 4. Reschedule Semantics
- Reschedule should validate the new slot before releasing/moving the old one (or use a transactional approach: check → cancel old → book new, with rollback on failure).

---

## Constraints

- Single-threaded; concurrency is NOT required.
- No email/calendar invites or recurrence rules required for the core solution.
- No persistence.

---

## Example Scenario

```text
Setup: Room A (capacity 4, projector)
       Room B (capacity 10, projector + video)

Action: search(10:00–11:00, minCapacity=5, amenities={projector})
        → [Room B]

Action: book(Room B, "Design Sync", organizer=U1, attendees=[U1,U2,U3], 10:00–11:00)
        → Meeting M1 created

Action: book(Room B, "Other", ..., 10:30–11:30) → REJECTED (overlap)
Action: book(Room A, "Small chat", attendees=6 people, ...) → REJECTED (capacity)

Action: listRoomMeetings(Room B, day) → [M1]
Action: cancel(M1) → OK
Action: book(Room B, "Other", 10:30–11:30) → OK
```

---

## Bonus Challenges (If Time Permits)

- Support **recurring meetings** (daily/weekly) with exception dates.
- Add **buffer time** between meetings (e.g., 10 minutes cleanup).
- Implement **user conflict checks** across rooms.
- Suggest **alternative rooms/slots** when the preferred booking fails.
- Discuss calendar sync / invite notifications as an Observer hook.
