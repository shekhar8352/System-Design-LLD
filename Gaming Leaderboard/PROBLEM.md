# LLD Machine Coding – Gaming Leaderboard with Seasons (C++)

## Problem Statement

Design and implement a **Gaming Leaderboard** that ingests player scores, serves ranks and top-N queries efficiently, supports time-windowed boards (daily/weekly/season) and percentile-based tiers, using Object-Oriented Programming in C++.

This problem is asked at Zynga, Dream11, MPL, and Google interviews. It tests **ordered-index design (beyond a sorted vector)**, **rank query efficiency**, **tie-breaking rules**, and windowed aggregation with an injected clock.

---

## Functional Requirements

1. Score ingestion:
   - `submitScore(playerId, score, timestamp)` — from game matches.
   - Two scoring modes per leaderboard, chosen at creation: **BEST** (keep the player's highest score) or **CUMULATIVE** (sum of all submissions in the window).
2. Rank queries:
   - `getRank(boardId, playerId)` — 1-based dense rank.
   - `getTop(boardId, n)` — top-N entries (player, score, rank).
   - `getAround(boardId, playerId, k)` — the player's neighborhood: k above + self + k below (the "you are here" view).
3. Time windows:
   - Boards: `ALL_TIME`, `DAILY`, `WEEKLY` — a submission counts toward every active board.
   - Window rollover (driven by injected clock): `DAILY` resets at midnight; the previous day's board is archived and queryable read-only.
4. Tiers:
   - `getTier(boardId, playerId)` — percentile-based: top 1% `LEGEND`, next 4% `DIAMOND`, next 15% `GOLD`, next 30% `SILVER`, rest `BRONZE`.
5. Player profile:
   - Score history, best rank achieved, tier per active board.

---

## Business Rules

- Tie-breaking: equal scores rank by **earlier achievement time** first; still tied → lexicographic playerId. Ranks are unique (no shared ranks) — document this vs "competition ranking."
- In BEST mode, a lower score never changes the player's entry (but is recorded in history).
- Scores are non-negative integers; a configurable `maxPlausibleScore` rejects obviously bogus submissions (anti-cheat hook).
- `getRank` for a player with no submissions in the window returns "unranked", not rank = last.
- Archived boards are immutable: submissions with timestamps in a past window are rejected (document: no backfill).
- Rollover must be lazy or explicit (`advanceTime(now)`) — no background threads.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Rank and top-N queries must beat "sort everything per query": maintain an ordered structure (`std::map`/`std::set` with a composite key) and discuss the cost of exact rank; an **order-statistic approach** (Fenwick/BIT over score buckets or a policy-free balanced-BST design) is a strong plus.
- Follow OOP principles; scoring mode and tie-break policy should be strategies.
- No UI/network; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `LeaderboardEntry` (playerId, score, achievedAt)
- `Leaderboard` (ordered index of entries, mode, window)
- `ScoringMode` (interface) → `BestScoreMode`, `CumulativeMode`
- `LeaderboardManager` (routes submissions to active boards, handles rollover/archives)
- `Window` (ALL_TIME / DAILY / WEEKLY with boundary math)
- `TierCalculator`
- `Clock` (abstract) + `FakeClock`

---

### 2. The Ordered Index
- Composite ordering key `(score DESC, achievedAt ASC, playerId ASC)` inside a `std::set`, plus a `playerId → iterator/key` map for O(log n) updates (remove old entry, insert new).
- Be explicit about `getRank`'s complexity with `std::set` (O(n) distance) and what you'd use to make it O(log n).

---

### 3. Update Path Correctness
- A score update = remove old key + insert new key, atomically from the caller's view. Stale-key bugs are the classic failure here.

---

### 4. Window Rollover
- On `advanceTime`, close boards whose window ended, snapshot them into an archive, start fresh boards. No score data leaks across windows.

---

## Constraints

- Up to ~10⁶ players conceptually — justify complexity, but correctness first.
- Single-threaded; no persistence.
- One game title (no cross-game boards).

---

## Example Scenario

```text
Board W1 (WEEKLY, BEST mode)

submit(alice, 900, t1) | submit(bob, 850, t2) | submit(carol, 900, t3)
getTop(3) → 1. alice 900 (earlier)  2. carol 900  3. bob 850

submit(bob, 950, t4) → getRank(bob) = 1
submit(bob, 700, t5) → no change (BEST mode), history records it

getAround(carol, 1) → [bob 950 #1] [alice 900 #2 ← wait, carol #3]
                      → returns #2 alice, #3 carol, #4 dave...

advanceTime(next week) → W1 archived (read-only), W2 starts empty
getRank(W2, alice) → UNRANKED until she plays
getTop(W1-archive, 3) → still answers, frozen
```

---

## Bonus Challenges (If Time Permits)

- Implement exact **O(log n) rank** with a Fenwick tree over score buckets.
- Add **friend leaderboards** (rank among an arbitrary subset, computed on the fly).
- Add **score decay** for inactivity in ALL_TIME boards.
- Add **suspicious-jump detection** (score delta > threshold flags the player for review).
- Discuss a Redis-style sorted-set mapping of your design (ZADD/ZRANK equivalence).
