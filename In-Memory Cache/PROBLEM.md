# LLD Machine Coding – In-Memory Cache with Eviction Policies (C++)

## Problem Statement

Design and implement a **generic In-Memory Cache** system with pluggable **eviction policies** using Object-Oriented Programming in C++.

This problem is asked at Google, Meta, Netflix, and Uber. It evaluates **data structure knowledge** (doubly linked list + hash map), **template/generics usage**, **design patterns** (Strategy), and clean **interface design**.

---

## Functional Requirements

1. The cache must support:
   - `put(key, value)` — insert or update a key-value pair.
   - `get(key)` → value — retrieve a value by key; return -1 (or throw) if not found.
   - `remove(key)` — explicitly remove a key.
   - `size()` — return current number of entries.
   - `contains(key)` — check if a key exists.

2. The cache has a fixed **capacity** set at construction time.

3. When the cache is **full** and a new key is inserted, an eviction policy determines which entry to remove.

4. The system must support **at least two eviction policies**:
   - **LRU (Least Recently Used)** — evict the entry that was accessed longest ago.
   - **LFU (Least Frequently Used)** — evict the entry with the lowest access count; ties broken by recency.

5. The eviction policy should be **configurable at construction time** and swappable at runtime.

---

## Business Rules

- `get` counts as a **use** and must update recency/frequency accordingly.
- `put` on an existing key **updates** the value and counts as a use.
- `put` on a new key when full triggers eviction **before** inserting.
- The cache must be **O(1) average** for both `get` and `put` operations.
- Capacity must be a positive integer; reject 0 or negative.

---

## Non-Functional Requirements

- Standard C++ library only (use `unordered_map`, `list`, etc.).
- Templates encouraged for key/value type genericity.
- Follow OOP principles and Strategy pattern for eviction.
- No UI required; demonstrate via `main()` with test cases.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core design:
- `EvictionPolicy` (abstract interface with `keyAccessed(key)`, `evict()` methods)
- `LRUEvictionPolicy` (implements using a doubly linked list + hash map of iterators)
- `LFUEvictionPolicy` (implements using frequency map + ordered sets or linked lists per frequency)
- `Cache<K, V>` (holds the storage map, capacity, and an `EvictionPolicy` instance)
- `Storage<K, V>` (optional separate class for the actual map — separates storage from eviction)

---

### 2. LRU Implementation (O(1) requirement)
- Use a `std::list<K>` as a doubly linked list (most recent at front).
- Use `std::unordered_map<K, list<K>::iterator>` for O(1) access to list nodes.
- On `get`/`put`: move the accessed node to front.
- On eviction: remove the back of the list.

---

### 3. LFU Implementation (O(1) requirement)
- Maintain:
  - `keyToVal`: key → value
  - `keyToFreq`: key → frequency
  - `freqToKeys`: frequency → ordered set of keys (insertion order matters for tie-breaking)
  - `minFreq`: track current minimum frequency
- On `get`/`put`: increment frequency, update `freqToKeys`, update `minFreq`.
- On eviction: remove the least-recent key from `freqToKeys[minFreq]`.

---

### 4. Strategy Pattern
- `Cache` must accept an `EvictionPolicy*` at construction.
- It should be possible to switch policy at runtime without rebuilding the cache.

---

### 5. Thread Safety (Optional / Bonus)
- Discuss how you would make the cache thread-safe using `std::mutex`.
- Add a `ThreadSafeCache<K, V>` wrapper (optional).

---

## Constraints

- O(1) average time for `get` and `put`.
- In-memory only.
- No persistence or external storage.
- Concurrency not required (but thread-safety discussion is a bonus).

---

## Example Scenario

### LRU Cache (Capacity: 3)

```text
put(1, "A") → Cache: {1:"A"}
put(2, "B") → Cache: {1:"A", 2:"B"}
put(3, "C") → Cache: {1:"A", 2:"B", 3:"C"}
get(1)      → "A" | Cache order (LRU→MRU): 2, 3, 1
put(4, "D") → Evicts key 2 (LRU) | Cache: {3:"C", 1:"A", 4:"D"}
get(2)      → -1 (evicted)
get(3)      → "C"
```

### LFU Cache (Capacity: 3)

```text
put(1, "A") → freq[1]=1
put(2, "B") → freq[2]=1
put(3, "C") → freq[3]=1
get(1)      → "A" | freq[1]=2
get(1)      → "A" | freq[1]=3
get(2)      → "B" | freq[2]=2
put(4, "D") → Evicts key 3 (freq=1, LRU among freq-1 keys)
get(3)      → -1 (evicted)
```

---

## Bonus Challenges (If Time Permits)

- Implement **TTL (Time-To-Live)**: entries expire after a set duration.
- Add **cache statistics**: hit rate, miss rate, eviction count.
- Implement **MRU (Most Recently Used)** eviction policy.
- Build a **write-through** vs **write-back** mode (simulate downstream store with a stub).
