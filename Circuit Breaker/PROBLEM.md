# LLD Machine Coding – Circuit Breaker Library (C++)

## Problem Statement

Design and implement a **Circuit Breaker** library that wraps calls to an unreliable downstream service and prevents cascading failures by failing fast when the service is unhealthy, using Object-Oriented Programming in C++.

This problem is asked at Netflix, Uber, Atlassian, and Salesforce interviews. It tests a precise **state machine (CLOSED / OPEN / HALF_OPEN)**, **sliding-window failure tracking**, **time-based transitions with an injected clock**, and clean **decorator-style API design**.

---

## Functional Requirements

1. Core API:
   - `execute(callable)` — invokes the protected operation through the breaker and returns its result, or a fast-failure without invoking it when the circuit is `OPEN`.
2. States and transitions:
   - **CLOSED** (normal): calls pass through; failures are recorded.
   - `CLOSED → OPEN` when the **failure threshold** is breached (e.g., failure rate ≥ 50% over the last N calls, with a minimum call volume).
   - **OPEN**: calls are rejected immediately (fail fast) without touching the downstream.
   - `OPEN → HALF_OPEN` after a configurable `cooldownSeconds` has elapsed.
   - **HALF_OPEN**: allow up to `K` **trial calls**; if all succeed → `CLOSED` (counters reset); any failure → back to `OPEN` (cooldown restarts).
3. Failure definition: an exception thrown by the callable, or a call exceeding `timeoutMs` (simulated — the callable reports its duration via the fake clock).
4. Configuration per breaker instance: window size, failure-rate threshold, minimum call volume, cooldown, half-open trial count, timeout.
5. Observability:
   - `getState()`, `getMetrics()` (rolling success/failure/rejected counts).
   - State-change **listeners** (Observer): callbacks invoked on every transition with old state, new state, and reason.
6. Support an optional **fallback** function invoked when the call is rejected or fails.

---

## Business Rules

- Rejected calls (while `OPEN`) must NOT count toward the failure rate — they never reached the service.
- The failure rate is computed over a **sliding window of the last N calls** (count-based window; time-based is a bonus).
- The breaker must not trip before `minimumCallVolume` calls are in the window (avoid tripping on 1 failure out of 1 call).
- In `HALF_OPEN`, calls beyond the `K` in-flight trial permits are rejected.
- A success in `HALF_OPEN` does not immediately close the circuit — all `K` trials must succeed.
- All timing uses an injected `Clock`; no `sleep()` anywhere.
- Transitions must be explainable: every transition records a reason (e.g., "failure rate 60% ≥ 50% over 10 calls").

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only (`functional`, `deque`, `chrono`, etc.).
- Follow OOP principles; state behaviour should be modelled explicitly (State pattern or a disciplined switch in one place).
- The breaker must be generic over the callable's return type (templates or `std::function`).
- No UI; demonstrate via `main()` with a scripted flaky service.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `CircuitBreaker` (execute, state, metrics, listeners)
- `CircuitBreakerConfig` (thresholds, window, cooldown, trial count, timeout)
- `SlidingWindow` (ring buffer of call outcomes; success/failure counts)
- `BreakerState` (State pattern: `ClosedState`, `OpenState`, `HalfOpenState`) — or one well-contained transition function
- `Clock` (abstract) + `FakeClock`
- `StateChangeListener` (interface), `CallOutcome` / `Metrics` structs
- Enums: `State` (CLOSED, OPEN, HALF_OPEN)

---

### 2. State Machine Rigor
- Every transition has a single source of truth; no duplicated threshold checks scattered around.
- OPEN → HALF_OPEN happens lazily on the next call after cooldown (no timers/threads) — interviewers check you realize this.

---

### 3. Sliding Window Correctness
- Fixed-capacity ring buffer; O(1) record and rate query.
- Window resets on transition to CLOSED (document why).

---

### 4. API Ergonomics
- `execute` cleanly separates: permission check → invoke → record outcome → maybe transition.
- Fallback path is optional and composable, not entangled with core logic.

---

## Constraints

- Single-threaded; thread-safety is NOT required (discussion is a bonus).
- Simulated time and simulated downstream (a callable you script to succeed/fail/timeout).
- One breaker protects one downstream; a registry of breakers is a bonus.

---

## Example Scenario

```text
Config: window=4, failureRate=50%, minVolume=4, cooldown=30s, halfOpenTrials=2

t=0..3  4 calls: OK, FAIL, FAIL, FAIL → rate 75% ≥ 50% → CLOSED→OPEN
t=5     execute() → REJECTED instantly (fallback invoked if provided)
t=36    execute() → cooldown passed → OPEN→HALF_OPEN, call allowed → OK (trial 1/2)
t=37    execute() → OK (trial 2/2) → HALF_OPEN→CLOSED, window reset
t=40    execute() → passes through normally

Later: HALF_OPEN trial fails → immediately back to OPEN, cooldown restarts
Listener log shows every transition with its reason.
```

---

## Bonus Challenges (If Time Permits)

- **Time-based sliding window** (outcomes bucketed per second) instead of count-based.
- Treat **slow calls** (over a slow-call threshold) as failures with a separate slow-call rate.
- A **BreakerRegistry** managing named breakers with shared default config.
- Make it **thread-safe**; discuss atomics vs mutex and the half-open permit race.
- Combine with a **retry decorator** and discuss ordering (retry inside breaker vs outside).
