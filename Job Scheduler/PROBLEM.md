# LLD Machine Coding – Job Scheduler with Dependencies & Priorities (C++)

## Problem Statement

Design and implement an **in-memory Job Scheduler** that executes jobs based on scheduled time, priority, and inter-job dependencies, with retry support for failed jobs, using Object-Oriented Programming in C++.

This problem is asked at Google, Uber, Atlassian, and LinkedIn interviews. It tests **priority queue usage**, **dependency graphs (topological ordering)**, **state machines**, and **time abstraction**.

---

## Functional Requirements

1. Support submitting jobs:
   - `submit(jobId, executeAt, priority, dependencies)` — a job runs no earlier than `executeAt` and only after all its dependencies have `COMPLETED`.
2. Support **one-time** and **recurring** jobs (recurring: fixed interval, e.g., every 30s).
3. Job selection rule when multiple jobs are runnable at the same tick:
   - Higher **priority** first; among equal priority, earlier `executeAt` first; then FIFO by submission.
4. Support **cancel(jobId)** for jobs not yet running.
5. Support **retries**: a `FAILED` job is retried up to `maxRetries` times with a fixed backoff delay.
6. The scheduler is driven by an explicit `tick()` / `runDueJobs(now)` call (no real threads) using an injected clock.
7. Query APIs:
   - Job status (`PENDING`, `WAITING_ON_DEPS`, `RUNNING`, `COMPLETED`, `FAILED`, `CANCELLED`).
   - Execution history (jobId, start time, end time, outcome, attempt number).

---

## Business Rules

- A job whose dependency `FAILED` permanently (retries exhausted) becomes `CANCELLED` (cascading cancellation) — document this choice.
- **Cycle detection**: submitting a job whose dependencies form a cycle must be rejected at submission time.
- Cancelling a job cascades to jobs that depend on it.
- A recurring job schedules its **next occurrence only after the current run finishes** (no overlapping runs).
- Job work is simulated via an injected callable/`Runnable` that returns success/failure, so failures are deterministic in tests.
- Duplicate jobIds must be rejected.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only (`priority_queue`, `unordered_map`, `functional`, etc.).
- Follow OOP principles; scheduling policy should be isolated and swappable.
- No real threads or `sleep()`; time is simulated. Demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Job` (id, executeAt, priority, dependencies, maxRetries, attemptCount, status, work callable)
- `Schedule` (one-time vs recurring with interval — polymorphic or variant)
- `Scheduler` (submit, cancel, tick/runDueJobs, status queries)
- `DependencyGraph` (adjacency, cycle detection, "all deps completed?" checks)
- `ExecutionRecord` (jobId, attempt, start, end, outcome)
- `Clock` (abstract) + `FakeClock`
- Enums: `JobStatus`, `JobOutcome`

---

### 2. Ready-Queue Discipline
- A `priority_queue` with a comparator implementing (priority, executeAt, submission order).
- Jobs blocked on dependencies must NOT sit in the ready queue; they enter it only when unblocked.

---

### 3. Job State Machine
- Legal transitions only: `PENDING/WAITING_ON_DEPS → RUNNING → COMPLETED | FAILED(→retry→PENDING)`; `CANCELLED` reachable from non-running states.
- Illegal transitions (cancel a `RUNNING` job) rejected with clear errors.

---

### 4. Dependency Handling
- Cycle detection at submit time (DFS or Kahn's algorithm).
- On completion, efficiently unblock dependents (reverse adjacency, indegree counters).

---

## Constraints

- Single-threaded, tick-driven execution; concurrency is NOT required.
- Job execution is instantaneous within a tick (no long-running simulation needed).
- No persistence or distributed coordination.

---

## Example Scenario

```text
t=0  submit(J1, executeAt=5, prio=1)
t=0  submit(J2, executeAt=5, prio=3)
t=0  submit(J3, executeAt=0, prio=2, deps=[J1])   → WAITING_ON_DEPS

tick(now=5):
  runnable: J2(prio 3), J1(prio 1) → run J2 first, then J1
  J1 COMPLETED → J3 unblocked, becomes runnable (executeAt already passed)
  run J3 → J3's work fails → FAILED, retry scheduled at now+backoff(2) = 7

tick(now=7):
  retry J3 (attempt 2) → SUCCESS → COMPLETED

submit(J4, deps=[J5]); submit(J5, deps=[J4]) → REJECTED (cycle)
```

---

## Bonus Challenges (If Time Permits)

- Add **exponential backoff with jitter** for retries.
- Add **cron-like expressions** for recurring schedules.
- Add a **worker-pool abstraction** (max K jobs per tick) and discuss fairness/starvation.
- Add **job timeouts** (a job running longer than T is marked failed).
- Discuss how you'd persist state to survive restarts (write-ahead log of transitions).
