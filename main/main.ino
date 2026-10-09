#include "Button.h"
#include "Elevator.h"
#include "Motor.h"
#include "door.h"

CabinButton cabinButtons[8] = {
    CabinButton{1, 49}, CabinButton{2, 48},
    CabinButton{3, 47}, CabinButton{4, 46},
    CabinButton{5, 45}, CabinButton{6, 44},
    CabinButton{7, 43}, CabinButton{8, 42}};

Elevator elevator;
Motor motor;
Door door;

void encoderInterrupt()
{
    motor.encoderTrack();
}

void setup()
{
    Serial.begin(9600);
    motor.begin();
    door.begin();

    attachInterrupt(
        digitalPinToInterrupt(20),
        encoderInterrupt,
        RISING);
    for (int i = 0; i < 8; ++i)
    {
        cabinButtons[i].begin();
    }

    Serial.println("Velg etasje 1-8:");
}

void loop()
{
    door.update();
    static unsigned long lastUpdate = 0;
    static unsigned long lastPrint = 0;
    static bool checkingArrival = false;
    static unsigned long arrivalStart = 0;
    static int previousTargetFloor = -1;
    static bool doorCycleActive = false;
    static bool doorOpenTimerStarted = false;
    static unsigned long doorOpenStart = 0;

    unsigned long now = millis();

    if (doorCycleActive)
    {
        if (door.isOpen() && !door.isMoving())
        {
            if (!doorOpenTimerStarted)
            {
                doorOpenStart = now;
                doorOpenTimerStarted = true;
            }

            if (now - doorOpenStart >= 2000)
            {
                door.close();
            }
        }

        if (door.isClosed() && !door.isMoving())
        {
            doorCycleActive = false;
            doorOpenTimerStarted = false;
        }
    }

    if (now - lastUpdate >= 10)
    {
        lastUpdate = now;

        if (!doorCycleActive)
        {
            elevator.updateDirection();
            elevator.updateTargetFloor(motor.getPosition());
        }

        int targetFloor = elevator.getTargetFloor();
        int targetPosition = targetFloor * 2096;

        motor.update(targetPosition);

        // Et nytt mål trenger en ny ankomstsjekk
        if (targetFloor != previousTargetFloor)
        {
            checkingArrival = false;
            previousTargetFloor = targetFloor;
        }

        long positionError = motor.getPosition() - targetPosition;

        if (!doorCycleActive &&
            elevator.getDirection() != IDLE &&
            abs(positionError) <= 5)
        {

            if (!checkingArrival)
            {
                arrivalStart = now;
                checkingArrival = true;
            }

            if (now - arrivalStart >= 300)
            {
                elevator.updateCurrentFloor(targetFloor);
                elevator.clearRequest(targetFloor);
                cabinButtons[targetFloor].reset();

                doorCycleActive = true;
                doorOpenTimerStarted = false;
                door.open();

                checkingArrival = false;
            }
        }
        else
        {
            checkingArrival = false;
        }

        if (now - lastPrint >= 100)
        {
            lastPrint = now;
            Serial.println(motor.getPosition());
        }

        if (Serial.available() > 0)
        {
            char key = Serial.read();
            int choice = key - '0';

            if (choice >= 1 && choice <= 8)
            {
                if (choice - 1 == elevator.getCurrentFloor())
                {
                    cabinButtons[choice - 1].reset();
                }
                else
                {
                    cabinButtons[choice - 1].press();
                }

                for (int index = 0; index < 8; ++index)
                {
                    if (cabinButtons[index].isPressed())
                    {
                        elevator.addCabinRequest(index);
                    }
                }

                elevator.printUpRequests();
            }
        }
    }
}