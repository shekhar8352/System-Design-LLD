# LLD Machine Coding – Chess Game (C++)

## Problem Statement

Design and implement a **two-player Chess game** playable on the console, using Object-Oriented Programming in C++.

This is a classic LLD problem asked at Amazon, Microsoft, Adobe, and Uber interviews. It tests **inheritance and polymorphism** (piece movement), **board modelling**, **game-state management**, and rule validation.

---

## Functional Requirements

1. The game is played between **two players** (White and Black) on a standard **8×8 board** with the standard initial setup.
2. Players alternate turns; **White moves first**.
3. A move is given as source and destination squares (e.g., `e2 e4`).
4. The system must **validate every move**:
   - The source square contains a piece belonging to the current player.
   - The move is legal for that piece type.
   - The path is not blocked (except for the Knight).
   - The destination is not occupied by the player's own piece.
   - The move does not leave the player's own king **in check**.
5. The system must detect and announce:
   - **Check** — opponent's king is under attack after the move.
   - **Checkmate** — the game ends; the mover wins.
   - **Stalemate** — the game ends in a draw.
6. The board must be printable to the console after each move.
7. Captured pieces are removed from the board and tracked per player.

---

## Business Rules

- **Piece movement**:
  - Pawn: 1 step forward (2 from its starting rank), captures diagonally.
  - Rook: any distance along rank/file.
  - Bishop: any distance along diagonals.
  - Queen: rook + bishop movement.
  - Knight: L-shape; can jump over pieces.
  - King: 1 step in any direction.
- An **illegal move is rejected** with a reason; the same player retries (turn does not pass).
- **Pawn promotion**: a pawn reaching the last rank is promoted (default Queen; player may choose).
- Castling and en passant are **bonus** — not required for the core solution.

---

## Non-Functional Requirements

- In-memory only.
- Standard C++ library only.
- Follow OOP principles; piece behaviour must be polymorphic, not a giant switch.
- Console I/O only.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Piece` (abstract: color, `canMove(board, from, to)`), with `Pawn`, `Rook`, `Knight`, `Bishop`, `Queen`, `King`
- `Board` (8×8 grid of `Cell`/`Square`, piece placement, path-blocking checks)
- `Cell` / `Square` (coordinates + occupying piece)
- `Move` (from, to, piece moved, piece captured — enables history/undo)
- `Player` (color, captured pieces)
- `Game` / `ChessGame` (turn management, move loop, game status)
- `GameStatus` enum (`ACTIVE`, `CHECK`, `CHECKMATE`, `STALEMATE`, ...)

---

### 2. Polymorphic Move Validation
- Each `Piece` subclass implements its own movement rule.
- Path-blocking logic belongs to the `Board` (shared), not duplicated per piece.
- No `if (pieceType == KNIGHT)` chains in the game loop.

---

### 3. Check / Checkmate Detection
- After each move, determine whether the opponent's king square is attacked by any enemy piece.
- Checkmate = king in check AND no legal move by any piece removes the check (simulate-and-revert moves).
- A player's move that would expose their own king must be rejected.

---

### 4. Game Loop & State
- `Game` orchestrates: read move → validate → apply → update status → switch turn.
- Move history stored as a list of `Move` objects.

---

## Constraints

- Two human players on the same console — no AI required.
- No timers/clocks required.
- Concurrency is NOT required.
- Castling, en passant, and draw-by-repetition are NOT required (bonus only).

---

## Example Scenario

```text
Initial board printed. White to move.

Move: e2 e4  → OK. Pawn advances two squares.
Move: e7 e5  → OK (Black).
Move: f1 c4  → OK. Bishop develops.
Move: b8 c6  → OK (Black).
Move: d1 f3  → OK. Queen develops.
Move: a7 a6  → OK (Black).
Move: f3 f7  → Queen captures f7 pawn. CHECKMATE — White wins.

Invalid examples:
Move: e4 e6  → REJECTED (pawn cannot move 2 squares after its first move)
Move: c4 c5  → REJECTED (bishop cannot move vertically)
```

---

## Bonus Challenges (If Time Permits)

- Implement **castling** (king-side and queen-side with all legality checks).
- Implement **en passant** captures.
- Add **undo** using the move history.
- Add **draw detection**: threefold repetition or 50-move rule.
- Support saving/loading a game as a **move list (algebraic notation)**.
