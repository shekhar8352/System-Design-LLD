# LLD Machine Coding – Text Editor with Undo/Redo (C++)

## Problem Statement

Design and implement the core of a **Text Editor** supporting insert, delete, cursor movement, selection, and unlimited **undo/redo**, using Object-Oriented Programming in C++.

This problem is asked at Microsoft, Adobe, Atlassian, and Google interviews. It is the classic test of the **Command pattern** (with inverse operations), **Memento vs Command trade-offs**, and careful **index/cursor bookkeeping**.

---

## Functional Requirements

1. Document operations:
   - `insert(text)` — insert at the cursor position; cursor moves to the end of the inserted text.
   - `deleteChars(n)` — delete `n` characters **before** the cursor (like backspace).
   - `moveCursor(pos)` / `moveLeft(n)` / `moveRight(n)`.
2. Selection operations:
   - `select(start, end)` — subsequent `insert` replaces the selection; `deleteChars` deletes the selection.
3. History operations:
   - `undo()` — revert the most recent **mutating** operation (insert/delete/replace). Cursor moves are NOT undoable.
   - `redo()` — re-apply the most recently undone operation.
   - Any new mutating operation **clears the redo stack**.
4. Query APIs:
   - `getText()`, `getCursor()`, `getSelection()`.
5. Undo/redo must restore **both text and cursor/selection state** as they were.

---

## Business Rules

- `deleteChars(n)` with fewer than `n` characters before the cursor deletes only what exists (no error).
- Replacing a selection is **one** undoable operation (undo restores the selected text and the selection), not a delete followed by an insert.
- `undo()` on empty history and `redo()` on empty redo stack are safe no-ops (or return false) — no crashes.
- Cursor and selection indices must always be clamped to valid range `[0, length]`.
- Consecutive single-character inserts MAY be coalesced into one undo step (document your choice; bonus if implemented with a time/size window).

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only (`string`, `stack`, `memory`, etc.).
- Follow OOP principles; each operation should be a first-class object, not a branch in a giant switch.
- No UI/terminal editing required; demonstrate via `main()` with printed document states.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Document` (text buffer, low-level splice operations)
- `Cursor` / `Selection` (position, anchor–end range)
- `Command` (abstract: `execute()`, `undo()`) with concrete `InsertCommand`, `DeleteCommand`, `ReplaceSelectionCommand`
- `HistoryManager` (undo stack, redo stack, push/undo/redo rules)
- `Editor` (public facade wiring document + cursor + history)

---

### 2. Command Pattern Done Right
- Every mutating command captures **exactly enough state to invert itself** (e.g., `DeleteCommand` stores the deleted substring and prior cursor).
- `undo()` must not recompute from scratch — it replays the stored inverse.
- Discuss Command (inverse ops) vs Memento (full snapshots): memory vs complexity.

---

### 3. Cursor/Selection Integrity
- Off-by-one handling at position 0 and end-of-document.
- After undo of a replace, the original selection is restored, not just the text.

---

### 4. History Discipline
- Redo stack cleared on new mutation.
- Cursor-only moves bypass history entirely.

---

## Constraints

- Plain `std::string` buffer is acceptable; rope/gap-buffer is NOT required (discussion is a bonus).
- Single document, single user; no concurrency.
- No file I/O, syntax highlighting, or rendering.

---

## Example Scenario

```text
insert("hello world")        → "hello world" | cursor=11
moveCursor(5)
insert(",")                  → "hello, world" | cursor=6
select(7, 12)                → selection = "world"
insert("there")              → "hello, there" | cursor=12   (one replace op)

undo()                       → "hello, world" | selection (7,12) restored
undo()                       → "hello world"  | cursor=6
redo()                       → "hello, world"

deleteChars(6) with cursor=6 → "world" → wait: deletes "hello," before cursor
undo()                       → "hello, world" restored exactly
insert("!")                  → redo stack cleared; redo() → no-op
```

---

## Bonus Challenges (If Time Permits)

- Implement **coalescing** of consecutive typing into one undo unit (break on pause/word boundary).
- Add **transactions/macro commands** (composite command: many edits, one undo step).
- Swap `std::string` for a **gap buffer** behind the same `Document` interface and show nothing else changes.
- Add **multi-cursor** editing and discuss how commands generalize.
- Persistent/branching history (undo tree instead of a linear stack).
