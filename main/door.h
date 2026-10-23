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

// 
    const int A_ENABLE = A15; //Setting DAC pins
    const int A_PHASE  = A14;
    const int B_ENABLE = A13;
    const int B_PHASE  = A12;

    const int stepsPerCycle = 4*8;
    static const int STEPS_CLOSED_TO_HALF = 400;
    static const int STEPS_HALF_TO_OPEN   = 400;
    static const int STEPS_CLOSED_TO_OPEN = STEPS_CLOSED_TO_HALF + STEPS_HALF_TO_OPEN;

    int stepNumber;
    DoorState state;

    int stepsRemaining{0};
    int stepDirection{1};
    unsigned long lastStepTime{0};
    DoorState targetState{CLOSED};
    float theta = 0;

    void moveSteps(int numberOfSteps, int direction);

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
    bool hasRequest(int floor) const;
};

#endif