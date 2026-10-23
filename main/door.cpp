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

// Ignore new movement commands until the current movement is complete.
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

// Schedule a non-blocking movement, executed one step at a time in update().
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

// Advance the stepper motor without blocking the main loop.
void Door::update()
{
    if (!isMoving()) {
        return;
    }

    unsigned long now = millis();

    if (now - lastStepTime < 2) { // Limit stepping to one step every 2 ms.
        return;
    }

    lastStepTime = now; 

    stepNumber += stepDirection; 

    if (stepNumber >= stepsPerCycle) { // Wrap the phase index after a complete cycle.
        stepNumber = 0;
    }

    if (stepNumber < 0) { // Wrap the phase index in the reverse direction.
        stepNumber = stepsPerCycle-1; 
    }

    stepsRemaining--;

    if (stepsRemaining == 0) { // Update the logical door state only after movement finishes.
        state = targetState;
    }

  float angle = TWO_PI * stepNumber / stepsPerCycle; // Convert the step index to an electrical phase angle.

  float currentA = cos(angle)*4095; // Generate sinusoidal coil drive values.
  float currentB = sin(angle)*4095;

 
  digitalWrite(A_PHASE, currentA >= 0 ? HIGH : LOW); // Set coil polarity based on the sign of each value.
  digitalWrite(B_PHASE, currentB >= 0 ? HIGH : LOW);


  int valueA = (int)roundf(fabsf(currentA)); // The DAC receives magnitudes; polarity is controlled separately.
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