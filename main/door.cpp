#include "door.h"
#include "dac.h"

const int halfStepSequence[8][4] = {
    {HIGH, HIGH, HIGH, HIGH},
    {LOW,  HIGH, HIGH, HIGH},
    {HIGH, LOW,  HIGH, HIGH},
    {HIGH, LOW,  LOW,  HIGH},
    {HIGH, LOW,  HIGH, LOW},
    {LOW,  HIGH, HIGH, LOW},
    {HIGH, HIGH, HIGH, LOW},
    {HIGH, HIGH, LOW,  HIGH}
};

Door::Door()
    : state{CLOSED},
      halfStepNumber{0}
{

}

void Door::begin()
{
    dac_init();
    set_dac(4095, 4095);

    pinMode(A_ENABLE, OUTPUT);
    pinMode(A_PHASE, OUTPUT);
    pinMode(B_ENABLE, OUTPUT);
    pinMode(B_PHASE, OUTPUT);
}

void Door::open()
{
    if (isMoving()) {
        return;
    }

    targetState = OPEN;

    if (state == CLOSED) {
        moveHalfSteps(STEPS_CLOSED_TO_OPEN, 1);
    }
    else if (state == HALF) {
        moveHalfSteps(STEPS_HALF_TO_OPEN, 1);
    }
}

void Door::close()
{
    if (isMoving()) {
        return;
    }

    targetState = CLOSED;

    if (state == OPEN) {
        moveHalfSteps(STEPS_CLOSED_TO_OPEN, -1);
    }
    else if (state == HALF) {
        moveHalfSteps(STEPS_CLOSED_TO_HALF, -1);
    }
}

void Door::moveToHalf()
{
    if (isMoving()) {
        return;
    }

    targetState = HALF;

    if (state == CLOSED) {
        moveHalfSteps(STEPS_CLOSED_TO_HALF, 1);
    }
    else if (state == OPEN) {
        moveHalfSteps(STEPS_HALF_TO_OPEN, -1);
    }
}

void Door::moveHalfSteps(int numberOfSteps, int direction)
{
    stepsRemaining = numberOfSteps;
    stepDirection = direction;
    lastStepTime = millis();
}

bool Door::isMoving() const
{
    return stepsRemaining > 0;
}

void Door::update()
{
    if (!isMoving()) {
        return;
    }

    unsigned long now = millis();

    if (now - lastStepTime < 5) {
        return;
    }

    lastStepTime = now;

    digitalWrite(A_ENABLE, halfStepSequence[halfStepNumber][0]);
    digitalWrite(A_PHASE,  halfStepSequence[halfStepNumber][1]);
    digitalWrite(B_ENABLE, halfStepSequence[halfStepNumber][2]);
    digitalWrite(B_PHASE,  halfStepSequence[halfStepNumber][3]);

    halfStepNumber += stepDirection;

    if (halfStepNumber > 7) {
        halfStepNumber = 0;
    }

    if (halfStepNumber < 0) {
        halfStepNumber = 7;
    }

    stepsRemaining--;

    if (stepsRemaining == 0) {
        state = targetState;
    }
}

void Door::disableMotor()
{
    digitalWrite(A_ENABLE, LOW);
    digitalWrite(B_ENABLE, LOW);
}

DoorState Door::getState() const
{
    return state;
}

bool Door::isOpen() const
{
    return state == OPEN;
}

bool Door::isClosed() const
{
    return state == CLOSED;
}