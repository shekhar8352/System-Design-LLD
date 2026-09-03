# LLD Machine Coding – Spreadsheet Engine with Formulas (C++)

## Problem Statement

Design and implement an **in-memory Spreadsheet Engine** (like a tiny Excel) that supports cell values, formulas referencing other cells, automatic recalculation, and cycle detection, using Object-Oriented Programming in C++.

This problem is asked at Google, Microsoft, Rippling, and DE Shaw interviews. It tests **dependency graphs**, **expression parsing/evaluation**, **incremental recomputation**, and caching with invalidation.

---

## Functional Requirements

1. Cell operations:
   - `setValue(cellRef, number)` — e.g., `setValue("A1", 5)`.
   - `setFormula(cellRef, formula)` — e.g., `setFormula("C1", "=A1+B1*2")`.
   - `getValue(cellRef)` — returns the evaluated numeric value (empty cells evaluate to 0).
   - `clear(cellRef)`.
2. Formula language:
   - Operators `+ - * /` with standard precedence and parentheses.
   - Cell references (`A1`, `B12`) and numeric literals.
   - Functions: `SUM(A1:A5)`, `AVG(A1:A5)`, `MIN`, `MAX` over rectangular ranges.
3. Recalculation:
   - Changing a cell updates every cell that (transitively) depends on it.
   - `getValue` must reflect the latest state; repeated reads without changes must not re-evaluate (caching).
4. Cycle detection:
   - Setting a formula that creates a circular reference (even transitively) is **rejected**, leaving the previous content intact.
5. Queries:
   - `getDependents(cellRef)` and `getPrecedents(cellRef)`; display a raw formula string vs its evaluated value.

---

## Business Rules

- Division by zero makes the cell's value an **error state** (`#DIV/0!`) that propagates to dependents (`#REF` chain semantics — document your choice).
- A formula referencing an error cell evaluates to an error.
- Range functions ignore truly empty cells for `AVG` (document: `AVG` of an empty range is an error).
- Re-setting a formula must fully replace old dependency edges (no stale edges).
- Cell references are case-insensitive (`a1` == `A1`).
- Invalid formula syntax is rejected at `setFormula` time with a clear parse error.

---

## Non-Functional Requirements

- In-memory only; sparse storage (a 1000×1000 grid with 10 filled cells must not allocate a million cells).
- Standard C++ library only.
- Follow OOP principles; the parser, the dependency graph, and evaluation must be separate components.
- No UI; demonstrate via `main()`.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Spreadsheet` (public API, cell map keyed by `CellRef`)
- `Cell` (raw content, cached value, dirty flag, state: VALUE / FORMULA / ERROR)
- `CellRef` (parsed column+row, hashable)
- `FormulaParser` → `Expression` AST (`NumberNode`, `CellRefNode`, `BinaryOpNode`, `FunctionNode`, `RangeNode`)
- `DependencyGraph` (precedents/dependents edges, cycle check, topological ordering)
- `Evaluator` (visits the AST against the sheet)

---

### 2. Incremental Recalculation
- On change: mark transitive dependents dirty, re-evaluate lazily on read **or** eagerly in topological order — pick one and justify it.
- Avoid re-evaluating unaffected cells (this is the key differentiator).

---

### 3. Cycle Detection
- Check for cycles **before committing** a formula (DFS from the new cell through new edges).
- Rejection must restore the exact previous state — edges, content, and cache.

---

### 4. Parsing
- A small recursive-descent parser with correct precedence beats a fragile regex hack.
- AST evaluation and dependency extraction should reuse the same tree.

---

## Constraints

- Numeric (double) values only — no strings/dates.
- Single sheet; columns `A–Z` and `AA–ZZ`, rows 1–9999.
- Single-threaded; no persistence; no undo required (see bonus).

---

## Example Scenario

```text
setValue(A1, 5)
setValue(A2, 10)
setFormula(A3, "=A1+A2")        → A3 = 15
setFormula(B1, "=A3*2+SUM(A1:A2)") → B1 = 45

setValue(A1, 20)                → A3 = 30, B1 = 105  (both auto-updated)

setFormula(A1, "=B1")           → REJECTED (cycle A1→B1→A3→A1); A1 remains 20
setFormula(C1, "=A1/0... wait")
setValue(D1, 0); setFormula(C1, "=A1/D1") → C1 = #DIV/0!
setFormula(C2, "=C1+1")         → C2 = #DIV/0! (error propagates)
setValue(D1, 4)                 → C1 = 5, C2 = 6
```

---

## Bonus Challenges (If Time Permits)

- Add **undo/redo** of cell mutations (Command pattern).
- Add **string values** and `CONCAT`, with type checking at evaluation.
- Support **inserting a row/column** with automatic reference shifting.
- Add `IF(cond, a, b)` and comparison operators.
- Discuss recalculation strategies at scale (dirty sets vs full topological sweeps).
