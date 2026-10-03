#ifndef ELEVATOR_H
#define ELEVATOR_H

enum Direction {
    UP,
    DOWN,
    IDLE
};

class Elevator {
private:
    static const int FLOOR_COUNT{8};

    int currentFloor_;
    int targetFloor_;
    Direction direction_;

    bool upRequests_[FLOOR_COUNT];
    bool downRequests_[FLOOR_COUNT];

    bool hasRequestAbove();
    bool hasRequestBelow();

public:
    Elevator();

    // Debugging
    void printUpRequests() const;

    // Requests
    void addHallRequest(int floor, Direction requestDirection);
    void addCabinRequest(int floor);
    void clearRequest(int floor);

    // State
    void updateCurrentFloor(int floor);
    int getCurrentFloor();

    void updateDirection();
    Direction getDirection();

    void updateTargetFloor(long position);
    int getTargetFloor();

    // Decision logic
    bool shouldStop();
};

#endif