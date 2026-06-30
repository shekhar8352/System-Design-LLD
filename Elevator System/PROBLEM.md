# LLD Machine Coding – Elevator System (C++)

## Problem Statement

Design and implement an **Elevator Control System** for a multi-floor building using Object-Oriented Programming in C++.

This problem tests your ability to model **state machines**, **request scheduling**, and **coordination between multiple objects**. It is commonly asked at Microsoft, Atlassian, Walmart, and Swiggy engineering interviews.

---

## Functional Requirements

1. The building has **N floors** (configurable) and **E elevators**.
2. Users can make two types of requests:
   - **External request**: Press UP or DOWN button on a floor.
   - **Internal request**: Press a floor button inside an elevator.
3. The system must:
   - Assign the **most optimal elevator** to service an external request.
   - Move the elevator to the requested floor.
   - Open and close doors when reaching the destination.
   - Allow multiple floor stops to be queued inside one elevator.
4. The system should support querying:
   - Current floor of each elevator.
   - Current direction of each elevator (`UP`, `DOWN`, `IDLE`).
   - All pending requests in each elevator's queue.

---

## Business Rules

- An elevator can move in one direction until it exhausts all requests in that direction, then reverses (**SCAN / Elevator algorithm**).
- An elevator that is **IDLE** is preferred for new external requests.
- If multiple elevators are idle, choose the **nearest** to the requesting floor.
- An elevator **cannot skip a floor** where it has a pending stop.
- Doors open for a fixed simulated duration (you can model this as a method call).
- An elevator in `UP` direction only picks up `UP` external requests on its way.

---

## Non-Functional Requirements

- In-memory only.
- No real threading required — simulate movement step-by-step.
- Use standard C++ library only.
- Follow OOP principles with clear class boundaries.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Elevator` (state: current floor, direction, door status, request queue)
- `ElevatorController` / `ElevatorManager` (dispatches requests to elevators)
- `Request` (encapsulates floor number + direction for external, or destination for internal)
- `Direction` enum (`UP`, `DOWN`, `IDLE`)
- `Door` (optional, or modelled as state inside `Elevator`)

---

### 2. State Machine
- Each `Elevator` has a well-defined state: `IDLE`, `MOVING_UP`, `MOVING_DOWN`, `DOOR_OPEN`.
- Transitions between states must be guarded and explicit.

---

### 3. Scheduling Strategy
- The dispatcher (controller) must implement a **selection strategy**:
  - Prefer idle elevator.
  - Prefer nearest elevator moving in the same direction.
- This strategy should be isolated and replaceable (open for extension).

---

### 4. Queue Management
- Each elevator maintains a sorted set of stops in its current direction.
- Requests for the opposite direction are held until direction reversal.

---

## Constraints

- Data stored in memory only.
- No real-time threading or timers needed.
- No UI required (console output is fine).
- Concurrency is NOT required.

---

## Example Scenario

```text
Building Setup:
  - 10 floors (0–9)
  - 2 elevators (E1 starts at floor 0, E2 starts at floor 5)

Action: External request – Floor 3, direction UP
Output: E1 assigned (nearest idle). E1 moves: 0 → 1 → 2 → 3. Door opens.

Action: Internal request inside E1 – go to Floor 7
Output: E1 queues Floor 7. Continues UP: 3 → 4 → 5 → 6 → 7. Door opens.

Action: External request – Floor 2, direction DOWN
Output: E2 assigned (E1 is busy moving up). E2 moves: 5 → 4 → 3 → 2. Door opens.

Action: Query state
Output:
  E1 → Floor 7, Direction: IDLE, Queue: []
  E2 → Floor 2, Direction: IDLE, Queue: []
```

---

## Bonus Challenges (If Time Permits)

- Implement **emergency stop** that halts all elevators.
- Add **weight limit** per elevator; reject request if overloaded.
- Support **VIP floors** that only specific elevators can access.
- Simulate **real-time step-by-step movement** with a `step()` function that advances each elevator by one floor.
