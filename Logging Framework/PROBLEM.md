# LLD Machine Coding – Logging Framework (C++)

## Problem Statement

Design and implement a **Logging Framework** (like Log4j / spdlog) that applications can use to emit log messages at different severity levels to multiple destinations, using Object-Oriented Programming in C++.

This problem is asked at Atlassian, Adobe, Salesforce, and Oracle interviews. It tests the **Chain of Responsibility** pattern, **Observer/appender design**, **formatting abstractions**, and building a clean **library-style API**.

---

## Functional Requirements

1. The framework must support standard **log levels** in increasing severity:
   `DEBUG < INFO < WARN < ERROR < FATAL`.
2. A `Logger` exposes convenience methods: `debug(msg)`, `info(msg)`, `warn(msg)`, `error(msg)`, `fatal(msg)`.
3. Each logger has a **configured minimum level**; messages below that level are silently dropped.
4. Log output must go to one or more **appenders (sinks)**:
   - **Console appender** — writes to stdout.
   - **File appender** — writes to a file (simulated in-memory buffer is acceptable).
   - A logger can have **multiple appenders** attached simultaneously.
5. Each log line is produced by a **formatter** that renders: timestamp, level, logger name, and message.
6. Loggers are retrieved by name via a **LoggerManager/factory**: `getLogger("com.app.service")` — the same name returns the same instance.

---

## Business Rules

- A message is dispatched to **all attached appenders** once it passes the level check.
- The level check happens **once** in the logger, not in each appender.
- Loggers support **hierarchical names** ("a.b.c" is a child of "a.b"): if a logger has no explicit level set, it **inherits** from its nearest configured ancestor (root logger has a default of `INFO`).
- Appenders and formatters must be **attachable/replaceable at runtime**.
- The default format is: `[timestamp] [LEVEL] [loggerName] message`.

---

## Non-Functional Requirements

- In-memory only; file appender may write to a real file or an in-memory buffer.
- Standard C++ library only.
- Follow OOP principles; adding a new appender or formatter must not modify existing code.
- No UI required; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `LogLevel` enum with ordering.
- `LogMessage` (level, logger name, message text, timestamp).
- `Logger` (name, level, list of appenders, log methods).
- `Appender` (abstract: `append(LogMessage)`), with `ConsoleAppender`, `FileAppender`.
- `Formatter` (abstract: `format(LogMessage)` → string), with `SimpleFormatter` (and optionally `JsonFormatter`).
- `LoggerManager` (singleton/registry: creates, caches, and configures loggers; owns the root logger).

---

### 2. Level Filtering & Hierarchy
- Level comparison via enum ordering — no chains of if/else per method.
- Effective level resolution walks up the name hierarchy ("a.b.c" → "a.b" → "a" → root).

---

### 3. Appender Design (Observer-style)
- `Logger` holds `Appender` pointers and iterates over them — it must not know concrete appender types.
- Each appender owns its `Formatter`, so console and file output can be formatted differently.

---

### 4. Extensibility
- New appender (e.g., `NetworkAppender` stub) — one new class, zero modified.
- New formatter (e.g., JSON) — one new class, zero modified.

---

## Constraints

- Concurrency is NOT required (async logging is a bonus discussion).
- No third-party libraries.
- No UI required.

---

## Example Scenario

```text
Setup:
  root logger level = INFO, ConsoleAppender with SimpleFormatter
  getLogger("app.payment") — no explicit level → inherits INFO
  getLogger("app.payment.gateway") — explicit level DEBUG, extra FileAppender

Action: app.payment logger → debug("starting payment")
Output: (dropped — DEBUG < effective level INFO)

Action: app.payment logger → info("payment initiated")
Output (console): [2026-07-06 10:00:00] [INFO] [app.payment] payment initiated

Action: app.payment.gateway logger → debug("gateway handshake ok")
Output (console + file): [2026-07-06 10:00:01] [DEBUG] [app.payment.gateway] gateway handshake ok

Action: change root level to ERROR, then app.payment → warn("retrying")
Output: (dropped — WARN < ERROR, level inherited from root)
```

---

## Bonus Challenges (If Time Permits)

- Implement an **AsyncAppender** that queues messages and flushes in batches (simulate the queue; no real threads needed).
- Add **rolling file behaviour**: rotate when the file exceeds N bytes (simulate with buffers).
- Support **structured logging**: key-value context fields attached to a message, rendered by a `JsonFormatter`.
- Add per-appender **level thresholds** (e.g., console shows all, file only ERROR+) and discuss where that check belongs.
