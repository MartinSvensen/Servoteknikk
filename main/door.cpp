#include "door.h"
#include "dac.h"


Door::Door()
    : state{CLOSED},
      stepNumber{0}
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

    digitalWrite(A_ENABLE, HIGH);
    digitalWrite(B_ENABLE, HIGH);
}

void Door::open()
{
    if (isMoving()) {
        return;
    }

    targetState = OPEN;

    if (state == CLOSED) {
        moveSteps(STEPS_CLOSED_TO_OPEN, 1);
    }
    else if (state == HALF) {
        moveSteps(STEPS_HALF_TO_OPEN, 1);
    }
}

void Door::close()
{
    if (isMoving()) {
        return;
    }

    targetState = CLOSED;

    if (state == OPEN) {
        moveSteps(STEPS_CLOSED_TO_OPEN, -1);
    }
    else if (state == HALF) {
        moveSteps(STEPS_CLOSED_TO_HALF, -1);
    }
}

void Door::moveToHalf()
{
    if (isMoving()) {
        return;
    }

    targetState = HALF;

    if (state == CLOSED) {
        moveSteps(STEPS_CLOSED_TO_HALF, 1);
    }
    else if (state == OPEN) {
        moveSteps(STEPS_HALF_TO_OPEN, -1);
    }
}

void Door::moveSteps(int numberOfSteps, int direction)
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

    if (now - lastStepTime < 2) { //Only uppdate motor position every 2 miliseconds
        return;
    }

    lastStepTime = now; 

    stepNumber += stepDirection; //Updating StepNumber in set direction

    if (stepNumber >= stepsPerCycle) { // Looping back to first microstep if the motor has done a whole step in positive direction
        stepNumber = 0;
    }

    if (stepNumber < 0) { // Looping back to last microstep if the motor has done a whole step in negative direction
        stepNumber = stepsPerCycle-1; 
    }

    stepsRemaining--;

    if (stepsRemaining == 0) { //Uppdating state if the motor is at its target
        state = targetState;
    }

  float angle = TWO_PI * stepNumber / stepsPerCycle; //Finding phase/angle in relation to one cycle

  float currentA = cos(angle)*4095; // Finding dac output for both motorcoils in relation to the angle
  float currentB = sin(angle)*4095;

 
  digitalWrite(A_PHASE, currentA >= 0 ? HIGH : LOW); //Setting current direction over coils
  digitalWrite(B_PHASE, currentB >= 0 ? HIGH : LOW);


  int valueA = (int)roundf(fabsf(currentA)); //Sending positive integer values to the dac
  int valueB = (int)roundf(fabsf(currentB));

  set_dac(valueB, valueA);


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