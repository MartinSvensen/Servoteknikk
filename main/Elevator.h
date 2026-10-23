#ifndef ELEVATOR_H
#define ELEVATOR_H

enum Direction {
    UP,
    DOWN,
    IDLE
};

// Manages floor requests and selects the elevator's travel direction and target.
class Elevator {
private:
    static const int FLOOR_COUNT{8};

    // Floors are indexed 0-7 internally (displayed as 1-8).
    int currentFloor_;
    int targetFloor_;
    Direction direction_;

    // Pending requests grouped by requested travel direction.
    bool upRequests_[FLOOR_COUNT];
    bool downRequests_[FLOOR_COUNT];

    bool hasRequestAbove();
    bool hasRequestBelow();

public:
    Elevator();

    // Prints pending upward requests for debugging.
    void printUpRequests() const;

    // Request management.
    bool hasRequest(int floor) const;
    void addHallRequest(int floor, Direction requestDirection);
    void addCabinRequest(int floor);
    void clearRequest(int floor);

    // Current floor and travel direction.
    void updateCurrentFloor(int floor);
    int getCurrentFloor();

    // Keeps the current direction while requests remain ahead.
    void updateDirection();
    Direction getDirection();

    // Selects a pending target ahead of the encoder position.
    void updateTargetFloor(long position);
    int getTargetFloor();

    // Checks whether the elevator should stop at the current floor.
    bool shouldStop();
};

#endif
