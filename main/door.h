#ifndef DOOR_H
#define DOOR_H

#include <Arduino.h>

enum DoorState {
    CLOSED,
    HALF,
    OPEN
};

class Door {
private:
    static const int A_ENABLE = 69;
    static const int A_PHASE  = 68;
    static const int B_ENABLE = 67;
    static const int B_PHASE  = 66;

    static const int STEPS_CLOSED_TO_HALF = 100;
    static const int STEPS_HALF_TO_OPEN   = 100;
    static const int STEPS_CLOSED_TO_OPEN = STEPS_CLOSED_TO_HALF + STEPS_HALF_TO_OPEN;

    int halfStepNumber;
    DoorState state;

    int stepsRemaining{0};
    int stepDirection{1};
    unsigned long lastStepTime{0};
    DoorState targetState{CLOSED};

    void moveHalfSteps(int numberOfSteps, int direction);

public:
    Door();

    void begin();

    void open();
    void close();
    void moveToHalf();
    void disableMotor();

    DoorState getState() const;
    bool isOpen() const;
    bool isClosed() const;
    void update();
    bool isMoving() const;
};

#endif