# LLD Machine Coding – Search Autocomplete (Typeahead) System (C++)

## Problem Statement

Design and implement a **Search Autocomplete System** that suggests the top-K completions for a typed prefix, ranked by popularity and recency, and learns from what users actually search, using Object-Oriented Programming in C++.

This problem is asked at Google, Amazon, Uber, and Microsoft interviews. It tests **trie design**, **top-K ranking with tie-breaking**, **incremental frequency updates**, and separating storage from ranking policy.

---

## Functional Requirements

1. Dictionary management:
   - `addPhrase(phrase, initialWeight)` — seed the system with known phrases.
   - `recordSearch(phrase)` — a completed user search increments that phrase's frequency (and adds it if new).
   - `removePhrase(phrase)` — e.g., for blocked content.
2. Query:
   - `suggest(prefix, k)` — return up to K phrases starting with `prefix`, ranked by:
     1. Higher score first (score = frequency, see recency bonus below),
     2. Then lexicographically smaller first.
3. Session typing model:
   - `typeChar(c)` / `backspace()` / `commit()` maintain the current input buffer; each keystroke returns fresh suggestions for the buffer.
   - `commit()` records the buffer as a completed search and clears it.
4. Recency (time-decayed) ranking:
   - Score = base frequency + a **recency bonus** for phrases searched within the last N time units (injected clock).
5. Queries:
   - Frequency of a phrase; total phrase count; top-K overall.

---

## Business Rules

- Matching is **case-insensitive**; suggestions display the canonical (first-inserted) casing.
- Prefix must match from the start of the phrase (no substring matching).
- Empty prefix returns the global top-K.
- Removed (blocked) phrases must never be suggested, even if re-searched — document whether `recordSearch` on a blocked phrase is ignored or rejected.
- Ties after score and lexicographic comparison cannot exist (phrases are unique).
- `suggest` must be read-only — only `recordSearch`/`commit` mutate scores.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- `suggest` should be efficient: traverse to the prefix node and gather candidates without scanning the entire dictionary.
- Follow OOP principles; the ranking policy (pure frequency vs frequency+recency) must be swappable.
- No UI/network; demonstrate via `main()` with a simulated typing session.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `TrieNode` (children map, terminal flag, phrase metadata pointer)
- `Trie` (insert, remove, findPrefixNode, collect)
- `PhraseInfo` (canonical text, frequency, lastSearchedAt)
- `RankingPolicy` (interface) + `FrequencyPolicy`, `RecencyBoostPolicy`
- `AutocompleteService` (public API, owns trie + policy + clock)
- `TypingSession` (buffer, typeChar/backspace/commit)
- `Clock` (abstract) + `FakeClock`

---

### 2. Top-K Retrieval
- Collect candidates under the prefix node and select top-K with a bounded heap (`priority_queue` of size K) — not a full sort of all matches.
- Discuss the trade-off vs **caching top-K per node** and updating on writes.

---

### 3. Correct Deletion
- Removing a phrase must clean up now-useless trie nodes (no memory leaks of dead branches) while preserving shared prefixes.

---

### 4. Policy Separation
- The trie stores; the policy ranks. Swapping policies must not touch trie code.

---

## Constraints

- ASCII lowercase letters, digits, and spaces in phrases.
- Single-threaded; no persistence.
- K ≤ 10; dictionary up to ~10⁵ phrases.

---

## Example Scenario

```text
addPhrase("mobile phone", 50)
addPhrase("mobile cover", 30)
addPhrase("mobile charger", 30)
addPhrase("mouse", 40)

suggest("mo", 3) → [mobile phone(50), mouse(40), mobile charger(30)]
                    (charger beats cover: equal score, lexicographic)

recordSearch("mobile cover") ×25 → mobile cover = 55
suggest("mo", 3) → [mobile cover(55), mobile phone(50), mouse(40)]

Typing session: t,y,p → suggest("typ") → [] → commit("typ")
suggest("ty", 5) → [typ(1)]        (learned a brand-new phrase)

removePhrase("mouse") → suggest("mo", 3) never contains "mouse" again
```

---

## Bonus Challenges (If Time Permits)

- Add **fuzzy matching** (edit distance 1) when exact-prefix results are fewer than K.
- Add **per-user personalization**: a user's own history boosts their suggestions.
- Cache top-K lists **inside trie nodes** and keep them consistent on updates.
- Add **phrase expiry**: phrases unsearched for N days decay and eventually drop out.
- Discuss sharding the trie by first character for scale, and the memory-vs-latency trade-off.
