// LLD Machine Coding – Elevator Control System (C++)
//
// Build & run:
//   g++ -std=c++17 -Wall -Wextra elevator_system.cpp -o elevator_system && ./elevator_system
//
// Design overview:
//   - Elevator            : state machine (IDLE, MOVING_UP, MOVING_DOWN, DOOR_OPEN)
//                           with two stop sets (above / below current floor),
//                           advanced one floor per step() — the SCAN algorithm.
//   - SchedulingStrategy  : pluggable dispatch policy (Strategy pattern).
//   - NearestAvailableStrategy : idle-nearest → same-direction on-the-way → least busy.
//   - ElevatorController  : public API — external requests, internal requests,
//                           tick-based simulation, and state queries.

#include <climits>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

enum class Direction { UP, DOWN, IDLE };
enum class ElevatorState { IDLE, MOVING_UP, MOVING_DOWN, DOOR_OPEN };

static std::string toString(Direction d) {
    switch (d) {
        case Direction::UP:   return "UP";
        case Direction::DOWN: return "DOWN";
        default:              return "IDLE";
    }
}

static std::string toString(ElevatorState s) {
    switch (s) {
        case ElevatorState::IDLE:        return "IDLE";
        case ElevatorState::MOVING_UP:   return "MOVING_UP";
        case ElevatorState::MOVING_DOWN: return "MOVING_DOWN";
        default:                         return "DOOR_OPEN";
    }
}

// ---------------------------------------------------------------------------
// Elevator
// ---------------------------------------------------------------------------
class Elevator {
public:
    Elevator(int id, int startFloor) : id_(id), currentFloor_(startFloor) {}

    int id() const { return id_; }
    int currentFloor() const { return currentFloor_; }
    ElevatorState state() const { return state_; }

    Direction direction() const {
        if (state_ == ElevatorState::MOVING_UP)   return Direction::UP;
        if (state_ == ElevatorState::MOVING_DOWN) return Direction::DOWN;
        return Direction::IDLE;
    }

    bool isIdle() const {
        return state_ == ElevatorState::IDLE && upStops_.empty() && downStops_.empty();
    }

    int pendingStopCount() const {
        return static_cast<int>(upStops_.size() + downStops_.size());
    }

    std::vector<int> pendingStops() const {
        std::vector<int> stops(upStops_.begin(), upStops_.end());
        stops.insert(stops.end(), downStops_.begin(), downStops_.end());
        return stops;
    }

    // True if the elevator, continuing on its current path, will pass `floor`
    // while moving in `dir` — such an elevator can pick the rider up en route.
    bool servesOnTheWay(int floor, Direction dir) const {
        if (state_ == ElevatorState::MOVING_UP && dir == Direction::UP)
            return floor >= currentFloor_;
        if (state_ == ElevatorState::MOVING_DOWN && dir == Direction::DOWN)
            return floor <= currentFloor_;
        return false;
    }

    void addStop(int floor) {
        if (floor == currentFloor_) {
            if (state_ == ElevatorState::IDLE) {
                openDoor();
            } else if (state_ == ElevatorState::MOVING_UP) {
                // Already passing this floor — serve it on the way back down.
                downStops_.insert(floor);
            } else if (state_ == ElevatorState::MOVING_DOWN) {
                upStops_.insert(floor);
            }
            // DOOR_OPEN at this floor: rider can walk in, nothing to queue.
            return;
        }
        if (floor > currentFloor_) upStops_.insert(floor);
        else                       downStops_.insert(floor);
    }

    // Advances the simulation by one tick: close doors, pick a direction, or
    // move exactly one floor. Returns false when there was nothing to do.
    bool step() {
        if (state_ == ElevatorState::DOOR_OPEN) {
            closeDoor();
            chooseDirection();
            return true;
        }
        if (state_ == ElevatorState::IDLE) {
            chooseDirection();
            if (state_ == ElevatorState::IDLE) return false;
            if (state_ == ElevatorState::DOOR_OPEN) return true;  // stop was at current floor
        }
        moveOneFloor();
        return true;
    }

private:
    void openDoor() {
        state_ = ElevatorState::DOOR_OPEN;
        log("arrives at floor " + std::to_string(currentFloor_) + " — doors open");
    }

    void closeDoor() {
        state_ = ElevatorState::IDLE;
        log("doors close at floor " + std::to_string(currentFloor_));
    }

    // SCAN policy: keep going in the last direction while stops remain there,
    // otherwise reverse; from a cold idle start, head toward the nearest stop.
    void chooseDirection() {
        // Rare reversal case: a stop registered for the floor we are already on.
        if (upStops_.erase(currentFloor_) || downStops_.erase(currentFloor_)) {
            openDoor();
            return;
        }

        const bool hasUp = !upStops_.empty();
        const bool hasDown = !downStops_.empty();
        if (!hasUp && !hasDown) {
            state_ = ElevatorState::IDLE;
            lastDirection_ = Direction::IDLE;
            return;
        }

        if (lastDirection_ == Direction::UP) {
            state_ = hasUp ? ElevatorState::MOVING_UP : ElevatorState::MOVING_DOWN;
        } else if (lastDirection_ == Direction::DOWN) {
            state_ = hasDown ? ElevatorState::MOVING_DOWN : ElevatorState::MOVING_UP;
        } else {
            const int upDist = hasUp ? *upStops_.begin() - currentFloor_ : INT_MAX;
            const int downDist = hasDown ? currentFloor_ - *downStops_.begin() : INT_MAX;
            state_ = (upDist <= downDist) ? ElevatorState::MOVING_UP : ElevatorState::MOVING_DOWN;
        }
        lastDirection_ = (state_ == ElevatorState::MOVING_UP) ? Direction::UP : Direction::DOWN;
    }

    void moveOneFloor() {
        const int from = currentFloor_;
        currentFloor_ += (state_ == ElevatorState::MOVING_UP) ? 1 : -1;
        log("moves " + std::to_string(from) + " -> " + std::to_string(currentFloor_));

        if (state_ == ElevatorState::MOVING_UP && upStops_.erase(currentFloor_)) {
            openDoor();
        } else if (state_ == ElevatorState::MOVING_DOWN && downStops_.erase(currentFloor_)) {
            openDoor();
        }
    }

    void log(const std::string& message) const {
        std::cout << "  E" << id_ << " " << message << "\n";
    }

    int id_;
    int currentFloor_;
    ElevatorState state_ = ElevatorState::IDLE;
    Direction lastDirection_ = Direction::IDLE;
    std::set<int> upStops_;                       // stops above, ascending
    std::set<int, std::greater<int>> downStops_;  // stops below, descending
};

// ---------------------------------------------------------------------------
// Scheduling strategy (Strategy pattern — replaceable without touching core)
// ---------------------------------------------------------------------------
class SchedulingStrategy {
public:
    virtual ~SchedulingStrategy() = default;
    virtual Elevator* select(const std::vector<std::unique_ptr<Elevator>>& elevators,
                             int floor, Direction dir) const = 0;
};

class NearestAvailableStrategy : public SchedulingStrategy {
public:
    Elevator* select(const std::vector<std::unique_ptr<Elevator>>& elevators,
                     int floor, Direction dir) const override {
        // 1. Nearest idle elevator.
        if (Elevator* e = nearest(elevators, floor, [](const Elevator& e) { return e.isIdle(); }))
            return e;
        // 2. Nearest elevator already moving toward the floor in the same direction.
        if (Elevator* e = nearest(elevators, floor, [floor, dir](const Elevator& e) {
                return e.servesOnTheWay(floor, dir);
            }))
            return e;
        // 3. Fallback: least busy elevator (fewest pending stops, then nearest).
        Elevator* best = nullptr;
        for (const auto& e : elevators) {
            if (!best ||
                e->pendingStopCount() < best->pendingStopCount() ||
                (e->pendingStopCount() == best->pendingStopCount() &&
                 std::abs(e->currentFloor() - floor) < std::abs(best->currentFloor() - floor))) {
                best = e.get();
            }
        }
        return best;
    }

private:
    template <typename Pred>
    static Elevator* nearest(const std::vector<std::unique_ptr<Elevator>>& elevators,
                             int floor, Pred eligible) {
        Elevator* best = nullptr;
        int bestDist = INT_MAX;
        for (const auto& e : elevators) {
            if (!eligible(*e)) continue;
            const int dist = std::abs(e->currentFloor() - floor);
            if (dist < bestDist) {
                bestDist = dist;
                best = e.get();
            }
        }
        return best;
    }
};

// ---------------------------------------------------------------------------
// ElevatorController — public facade
// ---------------------------------------------------------------------------
class ElevatorController {
public:
    ElevatorController(int floors, const std::vector<int>& startFloors,
                       std::unique_ptr<SchedulingStrategy> strategy =
                           std::make_unique<NearestAvailableStrategy>())
        : floors_(floors), strategy_(std::move(strategy)) {
        if (floors < 2) throw std::invalid_argument("building needs at least 2 floors");
        if (startFloors.empty()) throw std::invalid_argument("need at least one elevator");
        for (size_t i = 0; i < startFloors.size(); ++i) {
            validateFloor(startFloors[i]);
            elevators_.push_back(std::make_unique<Elevator>(static_cast<int>(i) + 1, startFloors[i]));
        }
    }

    // External request: hall button on `floor` pressed with direction `dir`.
    // Returns the id of the assigned elevator.
    int requestElevator(int floor, Direction dir) {
        validateFloor(floor);
        if (dir == Direction::IDLE) throw std::invalid_argument("direction must be UP or DOWN");
        if (dir == Direction::UP && floor == floors_ - 1)
            throw std::invalid_argument("cannot go UP from the top floor");
        if (dir == Direction::DOWN && floor == 0)
            throw std::invalid_argument("cannot go DOWN from the ground floor");

        Elevator* chosen = strategy_->select(elevators_, floor, dir);
        std::cout << "External request: floor " << floor << " " << toString(dir)
                  << " -> assigned E" << chosen->id() << "\n";
        chosen->addStop(floor);
        return chosen->id();
    }

    // Internal request: floor button pressed inside elevator `elevatorId`.
    void selectFloor(int elevatorId, int floor) {
        validateFloor(floor);
        std::cout << "Internal request: E" << elevatorId << " -> floor " << floor << "\n";
        elevatorById(elevatorId).addStop(floor);
    }

    // One simulation tick for every elevator. Returns true if anything moved.
    bool step() {
        bool active = false;
        for (auto& e : elevators_) active = e->step() || active;
        return active;
    }

    void runUntilIdle(int maxSteps = 1000) {
        for (int i = 0; i < maxSteps && step(); ++i) {}
    }

    void printStatus() const {
        std::cout << "Status:\n";
        for (const auto& e : elevators_) {
            std::ostringstream queue;
            queue << "[";
            const auto stops = e->pendingStops();
            for (size_t i = 0; i < stops.size(); ++i)
                queue << (i ? ", " : "") << stops[i];
            queue << "]";
            std::cout << "  E" << e->id() << " -> Floor " << e->currentFloor()
                      << ", Direction: " << toString(e->direction())
                      << ", State: " << toString(e->state())
                      << ", Queue: " << queue.str() << "\n";
        }
    }

private:
    void validateFloor(int floor) const {
        if (floor < 0 || floor >= floors_)
            throw std::out_of_range("floor " + std::to_string(floor) + " outside [0, " +
                                    std::to_string(floors_ - 1) + "]");
    }

    Elevator& elevatorById(int id) {
        for (auto& e : elevators_)
            if (e->id() == id) return *e;
        throw std::out_of_range("no elevator with id " + std::to_string(id));
    }

    int floors_;
    std::vector<std::unique_ptr<Elevator>> elevators_;
    std::unique_ptr<SchedulingStrategy> strategy_;
};

// ---------------------------------------------------------------------------
// Demo — reproduces the example scenario from PROBLEM.md, plus an
// on-the-way pickup to show the scheduling strategy at work.
// ---------------------------------------------------------------------------
int main() {
    std::cout << "=== Scenario 1: example from the problem statement ===\n";
    // 10 floors (0-9), E1 starts at floor 0, E2 starts at floor 5.
    // Note: PROBLEM.md's example claims E1 is nearest to floor 3, but E2 at
    // floor 5 (distance 2) beats E1 at floor 0 (distance 3) — the rule wins.
    ElevatorController controller(10, {0, 5});

    int car = controller.requestElevator(3, Direction::UP);  // E2 (nearest idle)
    controller.runUntilIdle();                               // E2: 5 -> 3, doors open/close

    controller.selectFloor(car, 7);                 // rider who boarded presses 7
    controller.requestElevator(2, Direction::DOWN); // E2 busy going up -> E1 assigned
    controller.runUntilIdle();

    controller.printStatus();
    // Expected: E1 -> Floor 2, IDLE, [] | E2 -> Floor 7, IDLE, []

    std::cout << "\n=== Scenario 2: nearest idle + SCAN ordering ===\n";
    int car2 = controller.requestElevator(4, Direction::DOWN); // E1 at 2 vs E2 at 7 -> E1
    controller.selectFloor(car2, 1);                // second stop queued below
    controller.runUntilIdle();  // from idle, E1 heads to the NEAREST stop first:
                                // 2 -> 1 (open), then reverses up 1 -> 4 (open)
    controller.printStatus();

    std::cout << "\n=== Scenario 3: invalid requests are rejected ===\n";
    try {
        controller.requestElevator(9, Direction::UP);
    } catch (const std::exception& ex) {
        std::cout << "  rejected: " << ex.what() << "\n";
    }
    try {
        controller.requestElevator(42, Direction::DOWN);
    } catch (const std::exception& ex) {
        std::cout << "  rejected: " << ex.what() << "\n";
    }

    return 0;
}
