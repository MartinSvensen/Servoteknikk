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
    if (state == CLOSED) {
        moveHalfSteps(STEPS_CLOSED_TO_OPEN, 1);
        state = OPEN;
    }
    else if (state == HALF) {
        moveHalfSteps(STEPS_HALF_TO_OPEN, 1);
        state = OPEN;
    }
}

void Door::close()
{
     if (state == OPEN) {
        moveHalfSteps(STEPS_CLOSED_TO_OPEN, -1);
        state = CLOSED;
    }
    else if (state == HALF) {
        moveHalfSteps(STEPS_CLOSED_TO_HALF, -1);
        state = CLOSED;
    }
}

void Door::moveToHalf()
{
    if (state == CLOSED) {
        moveHalfSteps(STEPS_CLOSED_TO_HALF, 1);
        state = HALF;
    }
    else if (state == OPEN) { 
        moveHalfSteps(STEPS_HALF_TO_OPEN, -1); 
        state = HALF; 
    }
}

void Door::moveHalfSteps(int numberOfSteps, int direction)
{
    for (int i = 0; i < numberOfSteps; i++) {
        digitalWrite(A_ENABLE, halfStepSequence[halfStepNumber][0]);
        digitalWrite(A_PHASE,  halfStepSequence[halfStepNumber][1]);
        digitalWrite(B_ENABLE, halfStepSequence[halfStepNumber][2]);
        digitalWrite(B_PHASE,  halfStepSequence[halfStepNumber][3]);

        halfStepNumber += direction;

        if (halfStepNumber > 7) {
            halfStepNumber = 0;
        }

        if (halfStepNumber < 0) {
            halfStepNumber = 7;
        }

        delay(5);
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