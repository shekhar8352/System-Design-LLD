# LLD Machine Coding – In-Memory Key-Value Store with Transactions (C++)

## Problem Statement

Design and implement an **in-memory Key-Value Store** supporting **nested transactions** with commit/rollback semantics and TTL-based expiry, using Object-Oriented Programming in C++.

This problem is asked at Bloomberg, Stripe, Databricks, and Rubrik interviews. It tests **transaction isolation modelling**, **layered state (copy-on-write vs undo logs)**, **TTL handling with an injected clock**, and clean API design.

---

## Functional Requirements

1. Basic operations:
   - `set(key, value)`, `get(key)`, `del(key)`, `exists(key)`.
2. Transaction operations:
   - `begin()` — start a transaction; transactions can be **nested** (begin inside begin).
   - `commit()` — merge the innermost transaction's changes into its parent (or the global store if outermost).
   - `rollback()` — discard the innermost transaction's changes.
3. Reads inside a transaction see: its own uncommitted writes first, then parent layers, then the global store.
4. Support **TTL**: `set(key, value, ttlSeconds)` — the key expires `ttlSeconds` after the set; expired keys behave as absent.
5. Support `keys(prefix)` — list live (non-expired, non-deleted) keys with a given prefix, respecting transaction visibility.
6. Calling `commit()`/`rollback()` with no open transaction must return a clear error.

---

## Business Rules

- A `del` inside a transaction must **shadow** the key: `get` returns "not found" inside that transaction even though the global store still has it, and the delete applies on commit.
- TTL is evaluated **lazily** against an injected clock at read time (no background threads).
- Setting a key without TTL after it had a TTL **clears** the TTL.
- Nested commit merges only into the **immediate parent**, not directly into the global store.
- Rollback of an outer transaction discards inner committed changes too (they were only merged into the outer layer).
- Values are strings; empty string is a valid value (distinct from absent).

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only (`unordered_map`, `optional`, `vector`, etc.).
- Follow OOP principles; transaction layering must not leak into calling code.
- `get`/`set`/`del` should remain O(1) average, independent of the number of keys (per-layer lookup is fine).
- No UI; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `KeyValueStore` (public API: set/get/del/begin/commit/rollback/keys)
- `TransactionLayer` (write-set of key → `Entry`, where `Entry` = value + TTL metadata, or a **tombstone** marking deletion)
- `Entry` (value, optional expiryTimestamp)
- `Clock` (abstract) + `SystemClock` / `FakeClock`

---

### 2. Layered Lookup
- A stack of `TransactionLayer`s over a base map.
- `get` walks from the innermost layer outward; the first layer containing the key (value **or tombstone**) decides the answer.
- Tombstones are essential — interviewers specifically probe deletes inside transactions.

---

### 3. Commit/Rollback Mechanics
- Rollback = pop the layer (O(size of layer)).
- Commit = merge layer into parent (tombstones overwrite parent entries).
- Discuss the trade-off vs an **undo-log** approach (record inverse operations against a single map).

---

### 4. TTL Correctness
- Expiry must respect transaction layers: an unexpired override in an inner layer beats an expired base entry.
- `keys(prefix)` must filter expired entries and tombstoned keys.

---

## Constraints

- Single-threaded; concurrency/isolation between concurrent transactions is NOT required.
- No persistence, replication, or eviction policies (that is the separate Cache problem).
- Key count small enough that `keys(prefix)` may scan (bonus: sorted structure).

---

## Example Scenario

```text
set(a, 1)                    → store: a=1
begin()
  set(a, 2); set(b, 9)
  get(a) → 2                 (inner write wins)
  begin()
    del(a)
    get(a) → NOT FOUND       (tombstone shadows everything)
    rollback()
  get(a) → 2                 (inner layer discarded)
  commit()
get(a) → 2 | get(b) → 9

set(s, "temp", ttl=10) at t=100
get(s) at t=105 → "temp"
get(s) at t=111 → NOT FOUND  (expired)

commit() with no open txn → ERROR
```

---

## Bonus Challenges (If Time Permits)

- Add `keys(prefix)` in **sorted order** via a trie or `std::map`, keeping point ops fast.
- Add **read-your-writes vs snapshot** discussion: what would `get` see if a parallel transaction committed mid-way?
- Add **WATCH/optimistic concurrency**: commit fails if a watched key changed underneath.
- Add **persistence** via an append-only operation log and replay on startup.
- Support **numeric operations** (`incr`, `decr`) with type errors on non-numeric values.
