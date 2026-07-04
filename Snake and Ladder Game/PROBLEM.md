# LLD Machine Coding – Snake and Ladder Game (C++)

## Problem Statement

Design and implement a **Snake and Ladder Game** using Object-Oriented Programming in C++.

This is a popular interview problem at companies like Directi, Razorpay, Gojek, and Dunzo. While the game logic itself is simple, this problem is specifically chosen to test **clean class design**, **single responsibility**, **extensibility**, and the ability to avoid writing everything in `main()`.

The interviewer wants to see **how** you model it, not just that it works.

---

## Functional Requirements

1. The game is played on a **10×10 board** (positions 1 to 100).
2. The board has:
   - **Snakes**: head position (higher) → tail position (lower).
   - **Ladders**: bottom position (lower) → top position (higher).
3. The game supports **2 to 4 players**.
4. On each turn:
   - The current player rolls a **single standard die** (1–6).
   - Their position advances by the rolled value.
   - If they land on a **snake's head**, they slide to the snake's tail.
   - If they land on a **ladder's bottom**, they climb to the ladder's top.
   - If the new position exceeds 100, the player does **not** move (exact finish required).
5. The first player to reach **position 100 exactly** wins.
6. The game must display the state of the board and each turn's outcome.

---

## Business Rules

- A snake's head must be at a **higher position** than its tail.
- A ladder's bottom must be at a **lower position** than its top.
- Snakes and ladders cannot **overlap** (no position can be both a snake head and a ladder bottom).
- Position 1 and position 100 cannot be snake heads or ladder tops respectively.
- Players take turns in the **order they were added**.
- If a player rolls and would land past 100, they stay at their current position.
- A snake or ladder at the winning position (100) is invalid.

---

## Non-Functional Requirements

- Standard C++ library only.
- In-memory only.
- No hardcoded board — snakes and ladders must be configurable at game setup.
- Follow strict OOP: avoid putting game logic in `main()`.
- Use `rand()` or `mt19937` for dice rolling.

---

## Design Expectations (What Interviewers Look For)

### 1. Class Modelling

Expected core classes:
- `Player` (name, current position)
- `Snake` (head, tail)
- `Ladder` (bottom, top)
- `Board` (size, map of snakes, map of ladders + position resolution)
- `Dice` (sides, roll method — injectable for testing)
- `Game` (players, board, dice, turn management, win detection)

---

### 2. Separation of Responsibilities
- `Board` is responsible for resolving what happens at a given position (snake/ladder/nothing).
- `Dice` is responsible only for generating a roll.
- `Game` is responsible for orchestrating turns and detecting the winner.
- `Player` only knows about itself (position, name).

---

### 3. Testability
- `Dice` should be **injectable** (or have a seeded constructor) so tests can control dice rolls.
- `Board::resolvePosition(pos)` should be a pure function (given position → final position).

---

### 4. Extensibility
- Adding a new game variant (e.g., two dice, special tiles) should not require rewriting core classes.
- The `Board` constructor should accept vectors of snakes and ladders.

---

### 5. Validation
- The board setup must validate:
  - No overlapping snake heads and ladder bottoms.
  - Snake head > tail; Ladder top > bottom.
  - All positions within 1–100 bounds.
- Invalid board configuration must throw or print a clear error.

---

## Constraints

- Board is always 10×10 (positions 1–100).
- 2–4 players.
- In-memory simulation.
- No UI required (console output per turn is sufficient).

---

## Example Scenario

```text
Board Setup:
  Snakes:  {17→7}, {54→34}, {62→19}, {92→88}
  Ladders: {3→22}, {5→8}, {20→29}, {80→100}

Players: Alice, Bob, Carol

=== Game Start ===

Turn 1 – Alice rolls 4 → moves to position 4
  No snake or ladder. Alice is at 4.

Turn 2 – Bob rolls 2 → moves to position 2
  No snake or ladder. Bob is at 2.

Turn 3 – Carol rolls 3 → moves to position 3
  Ladder at 3! Climbs to 22. Carol is at 22.

Turn 4 – Alice rolls 1 → moves to position 5
  Ladder at 5! Climbs to 8. Alice is at 8.

...

Turn N – Bob rolls X → moves to position 80
  Ladder at 80! Climbs to 100. Bob WINS!

=== Game Over | Winner: Bob ===
```

---

## Bonus Challenges (If Time Permits)

- Add **two dice** mode — player gets an extra turn on a double roll.
- Add **special tiles**: a "skip turn" tile that causes the next player to lose their turn.
- Implement **undo last move** (useful for a digital game with an undo button).
- Add **multiplayer leaderboard** tracking games won/lost per player across multiple game sessions.
