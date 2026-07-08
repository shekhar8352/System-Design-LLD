# LLD Machine Coding – Rate Limiter with Pluggable Algorithms (C++)

## Problem Statement

Design and implement a **Rate Limiter** library that decides whether an incoming request from a client should be **allowed or rejected**, supporting multiple rate-limiting algorithms, using Object-Oriented Programming in C++.

This problem is asked at Google, Stripe, Uber, and Atlassian interviews. It tests **algorithm implementation** (token bucket, sliding window), the **Strategy pattern**, **time-based logic**, and clean **interface design**.

---

## Functional Requirements

1. The rate limiter must expose a single core API:
   - `allowRequest(clientId)` → `true` (allow) or `false` (reject).
2. Limits are configured **per client** (e.g., client A: 10 req/sec, client B: 100 req/min). A **default limit** applies to unknown clients.
3. The system must support **at least three algorithms**:
   - **Fixed Window Counter** — N requests per fixed time window.
   - **Sliding Window Log** — N requests in any rolling window of W seconds.
   - **Token Bucket** — bucket of capacity C refilled at R tokens/second; each request consumes one token.
4. The algorithm must be **selectable at construction time** per limiter instance.
5. The system should support querying the **current state** for a client (e.g., remaining tokens / requests used in the window).

---

## Business Rules

- Time should be injected via a **Clock abstraction** so tests can simulate time instead of sleeping.
- **Fixed window**: window boundaries are aligned (e.g., 10:00:00–10:00:59); the counter resets at each boundary.
- **Sliding window log**: store request timestamps; evict timestamps older than the window before deciding.
- **Token bucket**: tokens refill lazily on each request based on elapsed time; the bucket never exceeds capacity; a request is rejected if no token is available (no partial tokens consumed).
- Different clients are **isolated** — one client exhausting its limit must not affect another.
- A rejected request must **not** consume quota.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only (`chrono`, `unordered_map`, `deque`, etc.).
- Follow OOP principles; the algorithm must be swappable without touching client code.
- No UI required; demonstrate via `main()` with a simulated clock.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `RateLimiter` (abstract interface: `allowRequest(clientId)`)
- `FixedWindowRateLimiter`, `SlidingWindowLogRateLimiter`, `TokenBucketRateLimiter`
- `RateLimitConfig` (limit, window size / refill rate, capacity)
- `Clock` (abstract) + `SystemClock` / `FakeClock` implementations
- `RateLimiterFactory` (optional — builds a limiter from config + algorithm type)

---

### 2. Strategy Pattern
- All algorithms implement the same `RateLimiter` interface.
- Adding a fourth algorithm (e.g., Sliding Window Counter) must require **zero changes** to existing classes.

---

### 3. Time Abstraction
- No `sleep()` calls in tests or demo.
- `FakeClock` with an `advance(seconds)` method drives all time-dependent behaviour deterministically.

---

### 4. Per-Client State Management
- Each client's state (counter, timestamps, tokens) is tracked independently in a map.
- Discuss memory growth for the sliding window log and how you would bound it.

---

## Constraints

- In-memory only; no distributed coordination required.
- Single-threaded — concurrency is NOT required (thread-safety discussion is a bonus).
- No UI required.

---

## Example Scenario

### Token Bucket (capacity = 3, refill = 1 token/sec)

```text
t=0s  request(c1) → ALLOW (tokens: 3 → 2)
t=0s  request(c1) → ALLOW (tokens: 2 → 1)
t=0s  request(c1) → ALLOW (tokens: 1 → 0)
t=0s  request(c1) → REJECT (no tokens)
t=2s  request(c1) → ALLOW (2 tokens refilled: 0 → 2, consume 1)
t=2s  request(c2) → ALLOW (c2 has its own full bucket)
```

### Fixed Window (limit = 2 per 60s window)

```text
t=10s request(c1) → ALLOW (window 0–59: count 1)
t=20s request(c1) → ALLOW (count 2)
t=30s request(c1) → REJECT (limit reached)
t=61s request(c1) → ALLOW (new window 60–119: count 1)
```

---

## Bonus Challenges (If Time Permits)

- Implement **Sliding Window Counter** (weighted average of two fixed windows).
- Make the limiter **thread-safe** with `std::mutex` and discuss lock granularity (global vs per-client).
- Add **tiered limits** (e.g., per-second AND per-minute limits simultaneously; both must pass).
- Return **retry-after** information (seconds until the next request would be allowed).
