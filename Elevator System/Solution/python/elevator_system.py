"""LLD Machine Coding – Elevator Control System (Python).

Run:
    python3 elevator_system.py

Design overview:
    - Elevator                 : state machine (IDLE, MOVING_UP, MOVING_DOWN,
                                 DOOR_OPEN) with two stop sets (above / below
                                 the current floor), advanced one floor per
                                 step() — the SCAN / elevator algorithm.
    - SchedulingStrategy       : pluggable dispatch policy (Strategy pattern).
    - NearestAvailableStrategy : idle-nearest -> same-direction on-the-way ->
                                 least busy fallback.
    - ElevatorController       : public API — external hall calls, internal
                                 floor selections, tick-based simulation, and
                                 state queries.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from enum import Enum
from typing import Callable, List, Optional


class Direction(Enum):
    UP = "UP"
    DOWN = "DOWN"
    IDLE = "IDLE"


class ElevatorState(Enum):
    IDLE = "IDLE"
    MOVING_UP = "MOVING_UP"
    MOVING_DOWN = "MOVING_DOWN"
    DOOR_OPEN = "DOOR_OPEN"


class Elevator:
    """A single elevator car running the SCAN algorithm."""

    def __init__(self, elevator_id: int, start_floor: int) -> None:
        self.id = elevator_id
        self.current_floor = start_floor
        self.state = ElevatorState.IDLE
        self._last_direction = Direction.IDLE
        self._up_stops: set[int] = set()    # stops above the current floor
        self._down_stops: set[int] = set()  # stops below the current floor

    # -- queries -----------------------------------------------------------

    @property
    def direction(self) -> Direction:
        if self.state is ElevatorState.MOVING_UP:
            return Direction.UP
        if self.state is ElevatorState.MOVING_DOWN:
            return Direction.DOWN
        return Direction.IDLE

    def is_idle(self) -> bool:
        return (
            self.state is ElevatorState.IDLE
            and not self._up_stops
            and not self._down_stops
        )

    def pending_stop_count(self) -> int:
        return len(self._up_stops) + len(self._down_stops)

    def pending_stops(self) -> List[int]:
        return sorted(self._up_stops) + sorted(self._down_stops, reverse=True)

    def serves_on_the_way(self, floor: int, direction: Direction) -> bool:
        """True if the car will pass `floor` while already moving in `direction`."""
        if self.state is ElevatorState.MOVING_UP and direction is Direction.UP:
            return floor >= self.current_floor
        if self.state is ElevatorState.MOVING_DOWN and direction is Direction.DOWN:
            return floor <= self.current_floor
        return False

    # -- commands ----------------------------------------------------------

    def add_stop(self, floor: int) -> None:
        if floor == self.current_floor:
            if self.state is ElevatorState.IDLE:
                self._open_door()
            elif self.state is ElevatorState.MOVING_UP:
                # Already passing this floor — serve it on the way back down.
                self._down_stops.add(floor)
            elif self.state is ElevatorState.MOVING_DOWN:
                self._up_stops.add(floor)
            # DOOR_OPEN at this floor: rider can walk in, nothing to queue.
            return
        if floor > self.current_floor:
            self._up_stops.add(floor)
        else:
            self._down_stops.add(floor)

    def step(self) -> bool:
        """Advance one tick: close doors, pick a direction, or move one floor.

        Returns False when the elevator had nothing to do.
        """
        if self.state is ElevatorState.DOOR_OPEN:
            self._close_door()
            self._choose_direction()
            return True
        if self.state is ElevatorState.IDLE:
            self._choose_direction()
            if self.state is ElevatorState.IDLE:
                return False
            if self.state is ElevatorState.DOOR_OPEN:
                return True  # a stop was registered for the current floor
        self._move_one_floor()
        return True

    # -- internals ---------------------------------------------------------

    def _open_door(self) -> None:
        self.state = ElevatorState.DOOR_OPEN
        self._log(f"arrives at floor {self.current_floor} — doors open")

    def _close_door(self) -> None:
        self.state = ElevatorState.IDLE
        self._log(f"doors close at floor {self.current_floor}")

    def _choose_direction(self) -> None:
        """SCAN policy: continue in the last direction while stops remain,
        otherwise reverse; from a cold idle start head for the nearest stop."""
        if self.current_floor in self._up_stops or self.current_floor in self._down_stops:
            self._up_stops.discard(self.current_floor)
            self._down_stops.discard(self.current_floor)
            self._open_door()
            return

        has_up, has_down = bool(self._up_stops), bool(self._down_stops)
        if not has_up and not has_down:
            self.state = ElevatorState.IDLE
            self._last_direction = Direction.IDLE
            return

        if self._last_direction is Direction.UP:
            self.state = ElevatorState.MOVING_UP if has_up else ElevatorState.MOVING_DOWN
        elif self._last_direction is Direction.DOWN:
            self.state = ElevatorState.MOVING_DOWN if has_down else ElevatorState.MOVING_UP
        else:
            up_dist = min(self._up_stops) - self.current_floor if has_up else float("inf")
            down_dist = self.current_floor - max(self._down_stops) if has_down else float("inf")
            self.state = (
                ElevatorState.MOVING_UP if up_dist <= down_dist else ElevatorState.MOVING_DOWN
            )
        self._last_direction = (
            Direction.UP if self.state is ElevatorState.MOVING_UP else Direction.DOWN
        )

    def _move_one_floor(self) -> None:
        start = self.current_floor
        self.current_floor += 1 if self.state is ElevatorState.MOVING_UP else -1
        self._log(f"moves {start} -> {self.current_floor}")

        if self.state is ElevatorState.MOVING_UP and self.current_floor in self._up_stops:
            self._up_stops.discard(self.current_floor)
            self._open_door()
        elif self.state is ElevatorState.MOVING_DOWN and self.current_floor in self._down_stops:
            self._down_stops.discard(self.current_floor)
            self._open_door()

    def _log(self, message: str) -> None:
        print(f"  E{self.id} {message}")


class SchedulingStrategy(ABC):
    """Dispatch policy — replaceable without touching the controller."""

    @abstractmethod
    def select(
        self, elevators: List[Elevator], floor: int, direction: Direction
    ) -> Elevator: ...


class NearestAvailableStrategy(SchedulingStrategy):
    def select(
        self, elevators: List[Elevator], floor: int, direction: Direction
    ) -> Elevator:
        # 1. Nearest idle elevator.
        idle = self._nearest(elevators, floor, lambda e: e.is_idle())
        if idle:
            return idle
        # 2. Nearest elevator already moving toward the floor in the same direction.
        on_the_way = self._nearest(
            elevators, floor, lambda e: e.serves_on_the_way(floor, direction)
        )
        if on_the_way:
            return on_the_way
        # 3. Fallback: least busy, then nearest.
        return min(
            elevators,
            key=lambda e: (e.pending_stop_count(), abs(e.current_floor - floor)),
        )

    @staticmethod
    def _nearest(
        elevators: List[Elevator], floor: int, eligible: Callable[[Elevator], bool]
    ) -> Optional[Elevator]:
        candidates = [e for e in elevators if eligible(e)]
        if not candidates:
            return None
        return min(candidates, key=lambda e: abs(e.current_floor - floor))


class ElevatorController:
    """Public facade: hall calls, cabin calls, simulation, and queries."""

    def __init__(
        self,
        floors: int,
        start_floors: List[int],
        strategy: Optional[SchedulingStrategy] = None,
    ) -> None:
        if floors < 2:
            raise ValueError("building needs at least 2 floors")
        if not start_floors:
            raise ValueError("need at least one elevator")
        self._floors = floors
        for f in start_floors:
            self._validate_floor(f)
        self._elevators = [Elevator(i + 1, f) for i, f in enumerate(start_floors)]
        self._strategy = strategy or NearestAvailableStrategy()

    # -- requests ------------------------------------------------------------

    def request_elevator(self, floor: int, direction: Direction) -> int:
        """External hall call. Returns the id of the assigned elevator."""
        self._validate_floor(floor)
        if direction is Direction.IDLE:
            raise ValueError("direction must be UP or DOWN")
        if direction is Direction.UP and floor == self._floors - 1:
            raise ValueError("cannot go UP from the top floor")
        if direction is Direction.DOWN and floor == 0:
            raise ValueError("cannot go DOWN from the ground floor")

        chosen = self._strategy.select(self._elevators, floor, direction)
        print(
            f"External request: floor {floor} {direction.value}"
            f" -> assigned E{chosen.id}"
        )
        chosen.add_stop(floor)
        return chosen.id

    def select_floor(self, elevator_id: int, floor: int) -> None:
        """Internal cabin call: floor button pressed inside an elevator."""
        self._validate_floor(floor)
        print(f"Internal request: E{elevator_id} -> floor {floor}")
        self._elevator_by_id(elevator_id).add_stop(floor)

    # -- simulation ----------------------------------------------------------

    def step(self) -> bool:
        """One tick for every elevator. Returns True if anything moved."""
        active = False
        for elevator in self._elevators:
            active = elevator.step() or active
        return active

    def run_until_idle(self, max_steps: int = 1000) -> None:
        for _ in range(max_steps):
            if not self.step():
                break

    # -- queries -------------------------------------------------------------

    def print_status(self) -> None:
        print("Status:")
        for e in self._elevators:
            print(
                f"  E{e.id} -> Floor {e.current_floor},"
                f" Direction: {e.direction.value},"
                f" State: {e.state.value},"
                f" Queue: {e.pending_stops()}"
            )

    # -- internals -----------------------------------------------------------

    def _validate_floor(self, floor: int) -> None:
        if not 0 <= floor < self._floors:
            raise ValueError(f"floor {floor} outside [0, {self._floors - 1}]")

    def _elevator_by_id(self, elevator_id: int) -> Elevator:
        for e in self._elevators:
            if e.id == elevator_id:
                return e
        raise ValueError(f"no elevator with id {elevator_id}")


def main() -> None:
    print("=== Scenario 1: example from the problem statement ===")
    # 10 floors (0-9), E1 starts at floor 0, E2 starts at floor 5.
    # Note: PROBLEM.md's example claims E1 is nearest to floor 3, but E2 at
    # floor 5 (distance 2) beats E1 at floor 0 (distance 3) — the rule wins.
    controller = ElevatorController(floors=10, start_floors=[0, 5])

    car = controller.request_elevator(3, Direction.UP)  # E2 (nearest idle)
    controller.run_until_idle()                         # E2: 5 -> 3, doors open/close

    controller.select_floor(car, 7)                  # rider who boarded presses 7
    controller.request_elevator(2, Direction.DOWN)   # E2 busy going up -> E1 assigned
    controller.run_until_idle()

    controller.print_status()
    # Expected: E1 -> Floor 2, IDLE, [] | E2 -> Floor 7, IDLE, []

    print("\n=== Scenario 2: nearest idle + SCAN ordering ===")
    car2 = controller.request_elevator(4, Direction.DOWN)  # E1 at 2 vs E2 at 7 -> E1
    controller.select_floor(car2, 1)                 # second stop queued below
    controller.run_until_idle()  # from idle, E1 heads to the NEAREST stop first:
    #                              2 -> 1 (open), then reverses up 1 -> 4 (open)
    controller.print_status()

    print("\n=== Scenario 3: invalid requests are rejected ===")
    for floor, direction in [(9, Direction.UP), (42, Direction.DOWN)]:
        try:
            controller.request_elevator(floor, direction)
        except ValueError as exc:
            print(f"  rejected: {exc}")


if __name__ == "__main__":
    main()
