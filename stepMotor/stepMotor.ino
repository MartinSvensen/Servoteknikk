#include <dac.h>

const int A_ENABLE = 69;
const int A_PHASE  = 68;
const int B_ENABLE = 67;
const int B_PHASE  = 66;

const int fullStepSequence[4][2] = {
    {LOW,  LOW},
    {LOW,  HIGH},
    {HIGH, HIGH},
    {HIGH, LOW}
};

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

int fullStepNumber = 0;
int halfStepNumber = 0;

void setup()
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

void loop()
{
    moveStepsHalf(200, -1);
    delay(1000);
    moveStepsHalf(200, 1);
    delay(1000);
}

void moveStepsFull(int numberOfSteps, int direction)
{
    for (int i = 0; i < numberOfSteps; i++)
    {
        stepMotorFull(direction);
        delay(5);
    }
}

void moveStepsHalf(int numberOfSteps, int direction)
{
    for (int i = 0; i < numberOfSteps; i++)
    {
        stepMotorHalf(direction);
        delay(5);
    }
}

void stepMotorFull(int direction)
{
    digitalWrite(A_ENABLE, HIGH);
    digitalWrite(B_ENABLE, HIGH);

    digitalWrite(A_PHASE, fullStepSequence[fullStepNumber][0]);
    digitalWrite(B_PHASE, fullStepSequence[fullStepNumber][1]);

    fullStepNumber += direction;

    if (fullStepNumber > 3)
        fullStepNumber = 0;

    if (fullStepNumber < 0)
        fullStepNumber = 3;
}

void stepMotorHalf(int direction)
{
    digitalWrite(A_ENABLE, halfStepSequence[halfStepNumber][0]);
    digitalWrite(A_PHASE,  halfStepSequence[halfStepNumber][1]);
    digitalWrite(B_ENABLE, halfStepSequence[halfStepNumber][2]);
    digitalWrite(B_PHASE,  halfStepSequence[halfStepNumber][3]);

    halfStepNumber += direction;

    if (halfStepNumber > 7)
        halfStepNumber = 0;

    if (halfStepNumber < 0)
        halfStepNumber = 7;
}