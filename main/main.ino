
#include "Button.h"
#include "Elevator.h"
#include "Motor.h"
#include "door.h"
#include "Display.h"

CabinButton cabinButtons[8] = {
    CabinButton{1, 49}, CabinButton{2, 48},
    CabinButton{3, 47}, CabinButton{4, 46},
    CabinButton{5, 45}, CabinButton{6, 44},
    CabinButton{7, 43}, CabinButton{8, 42}
};

Elevator elevator;
Motor motor;
Door door;
Display display;

enum MenuState
{
    MAIN_MENU,
    CABIN_MENU,
    SELECT_HALL_FLOOR,
    SELECT_HALL_DIRECTION
};

MenuState menuState = MAIN_MENU;
int selectedHallFloor = 0;

// Automated scheduling test
bool testSequenceActive = false;
unsigned long testSequenceStart = 0;
byte nextTestRequest = 0;

// --------------------------------------------------
// Logging
// --------------------------------------------------

const char* directionName(Direction direction)
{
    switch (direction)
    {
    case UP:   return "UP";
    case DOWN: return "DOWN";
    default:   return "IDLE";
    }
}

// CSV format:
// LOG,time_ms,event,floor,target_floor,direction,encoder_position
void logEvent(
    const char* event,
    int floor,
    int targetFloor,
    Direction direction,
    long position)
{
    Serial.print("LOG,");
    Serial.print(millis());
    Serial.print(",");
    Serial.print(event);
    Serial.print(",");
    Serial.print(floor);
    Serial.print(",");
    Serial.print(targetFloor);
    Serial.print(",");
    Serial.print(directionName(direction));
    Serial.print(",");
    Serial.println(position);
}

// --------------------------------------------------
// Menu
// --------------------------------------------------

void printMenu()
{
    Serial.println();

    switch (menuState)
    {
    case MAIN_MENU:
    Serial.println("===== HOVEDMENY =====");
    Serial.println("i: Inne i heisen");
    Serial.println("u: Utenfor heisen");
    Serial.println("t: Start automatisert test");
    break;

    case CABIN_MENU:
        Serial.println("===== INNE I HEISEN =====");
        Serial.println("1-8: Velg etasje");
        Serial.println("b: Tilbake til hovedmenyen");
        break;

    case SELECT_HALL_FLOOR:
        Serial.println("===== UTENFOR HEISEN =====");
        Serial.println("1-8: Hvilken etasje er du i?");
        Serial.println("b: Tilbake til hovedmenyen");
        break;

    case SELECT_HALL_DIRECTION:
        Serial.print("===== ETASJE ");
        Serial.print(selectedHallFloor + 1);
        Serial.println(" =====");

        if (selectedHallFloor < 7)
            Serial.println("u: Bestill heisen opp");

        if (selectedHallFloor > 0)
            Serial.println("d: Bestill heisen ned");

        Serial.println("b: Tilbake til etasjevalg");
        break;
    }

    Serial.println("m: Vis menyen igjen");
}

void handleMenu()
{
    if (Serial.available() == 0)
        return;

    char key = Serial.read();

    if (key == '\n' || key == '\r')
        return;

    if (key == 'm')
    {
        printMenu();
        return;
    }

    if (key == 'b')
    {
        if (menuState == SELECT_HALL_DIRECTION)
            menuState = SELECT_HALL_FLOOR;
        else
            menuState = MAIN_MENU;

        printMenu();
        return;
    }

    switch (menuState)
    {
    case MAIN_MENU:
        if (key == 'i')
        {
            menuState = CABIN_MENU;
            printMenu();
        }
        else if (key == 'u')
        {
            menuState = SELECT_HALL_FLOOR;
            printMenu();
        }

        else if (key == 't')
{
    if (!testSequenceActive)
    {
        testSequenceActive = true;
        testSequenceStart = millis();
        nextTestRequest = 0;

        Serial.println("Testsekvens startet.");

        logEvent(
            "TEST_START",
            elevator.getCurrentFloor() + 1,
            elevator.getTargetFloor() + 1,
            elevator.getDirection(),
            motor.getPosition());
    }
    else
    {
        Serial.println("Testsekvensen kjorer allerede.");
    }
}
        
        else
            Serial.println("Ugyldig valg.");
        break;

    case CABIN_MENU:
        if (key >= '1' && key <= '8')
        {
            int floor = key - '1';

            if (floor == elevator.getCurrentFloor())
            {
                cabinButtons[floor].reset();
                Serial.println(
                    "Dette er sist registrerte etasje.");
            }
            else
            {
                cabinButtons[floor].press();
                elevator.addCabinRequest(floor);

                logEvent(
                    "REQUEST_CABIN",
                    floor + 1,
                    elevator.getTargetFloor() + 1,
                    elevator.getDirection(),
                    motor.getPosition());

                Serial.print("Bestilt etasje: ");
                Serial.println(floor + 1);
            }
        }
        else
            Serial.println("Velg en etasje fra 1 til 8.");
        break;

    case SELECT_HALL_FLOOR:
        if (key >= '1' && key <= '8')
        {
            selectedHallFloor = key - '1';
            menuState = SELECT_HALL_DIRECTION;
            printMenu();
        }
        else
            Serial.println("Velg en etasje fra 1 til 8.");
        break;

    case SELECT_HALL_DIRECTION:
        if (key == 'u' && selectedHallFloor < 7)
        {
            elevator.addHallRequest(selectedHallFloor, UP);

            logEvent(
                "REQUEST_HALL_UP",
                selectedHallFloor + 1,
                elevator.getTargetFloor() + 1,
                elevator.getDirection(),
                motor.getPosition());

            Serial.println("Oppbestilling registrert.");
        }
        else if (key == 'd' && selectedHallFloor > 0)
        {
            elevator.addHallRequest(selectedHallFloor, DOWN);

            logEvent(
                "REQUEST_HALL_DOWN",
                selectedHallFloor + 1,
                elevator.getTargetFloor() + 1,
                elevator.getDirection(),
                motor.getPosition());

            Serial.println("Nedbestilling registrert.");
        }
        else
            Serial.println("Ugyldig retning for denne etasjen.");
        break;
    }
}


void updateTestSequence()
{
    if (!testSequenceActive)
        return;

    unsigned long elapsed = millis() - testSequenceStart;

    // t = 0 s: Request floor 7.
    if (nextTestRequest == 0)
    {
        elevator.addCabinRequest(6);
        cabinButtons[6].press();

        logEvent(
            "TEST_REQUEST_7",
            7,
            elevator.getTargetFloor() + 1,
            elevator.getDirection(),
            motor.getPosition());

        nextTestRequest = 1;
    }

    // t = 1 s: Request floor 3.
    if (nextTestRequest == 1 && elapsed >= 1000)
    {
        elevator.addCabinRequest(2);
        cabinButtons[2].press();

        logEvent(
            "TEST_REQUEST_3",
            3,
            elevator.getTargetFloor() + 1,
            elevator.getDirection(),
            motor.getPosition());

        nextTestRequest = 2;
    }

    // t = 2 s: Request floor 5.
    if (nextTestRequest == 2 && elapsed >= 2000)
    {
        elevator.addCabinRequest(4);
        cabinButtons[4].press();

        logEvent(
            "TEST_REQUEST_5",
            5,
            elevator.getTargetFloor() + 1,
            elevator.getDirection(),
            motor.getPosition());

        nextTestRequest = 3;
        testSequenceActive = false;

        logEvent(
            "TEST_REQUESTS_COMPLETE",
            elevator.getCurrentFloor() + 1,
            elevator.getTargetFloor() + 1,
            elevator.getDirection(),
            motor.getPosition());

        Serial.println("Alle testbestillinger er sendt.");
    }
}


// --------------------------------------------------
// Encoder interrupt
// --------------------------------------------------

void encoderInterrupt()
{
    motor.encoderTrack();
}

// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
    Serial.begin(9600);

    motor.begin();
    door.begin();
    display.begin();

    display.update(
        motor.getPosition(),
        elevator.getDirection());

    attachInterrupt(
        digitalPinToInterrupt(20),
        encoderInterrupt,
        RISING);

    for (int i = 0; i < 8; ++i)
        cabinButtons[i].begin();

    printMenu();
}

// --------------------------------------------------
// Main control loop
// --------------------------------------------------

void loop()
{
    handleMenu();
    updateTestSequence();
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

    // Door cycle: hold open for two seconds.
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

                logEvent(
                    "DOOR_CLOSE_CMD",
                    elevator.getCurrentFloor() + 1,
                    elevator.getTargetFloor() + 1,
                    elevator.getDirection(),
                    motor.getPosition());
            }
        }

        if (door.isClosed() && !door.isMoving())
        {
            logEvent(
                "DOOR_CLOSED",
                elevator.getCurrentFloor() + 1,
                elevator.getTargetFloor() + 1,
                elevator.getDirection(),
                motor.getPosition());

            doorCycleActive = false;
            doorOpenTimerStarted = false;
        }
    }

    // Update elevator control every 10 ms.
    if (now - lastUpdate >= 10)
    {
        lastUpdate = now;

        if (!doorCycleActive)
        {
            int currentFloor = elevator.getCurrentFloor();
            long floorPosition = currentFloor * 2096L;

            bool requestAtCurrentFloor =
                elevator.hasRequest(currentFloor) &&
                elevator.getTargetFloor() == currentFloor &&
                abs(motor.getPosition() - floorPosition) <= 5;

            if (!requestAtCurrentFloor)
            {
                elevator.updateDirection();
                elevator.updateTargetFloor(motor.getPosition());
            }
        }

        int targetFloor = elevator.getTargetFloor();
        int targetPosition = targetFloor * 2096;

        // Log when a new requested target is selected.
        if (targetFloor != previousTargetFloor)
        {
            checkingArrival = false;

            if (elevator.hasRequest(targetFloor))
            {
                logEvent(
                    "TARGET_SELECTED",
                    elevator.getCurrentFloor() + 1,
                    targetFloor + 1,
                    elevator.getDirection(),
                    motor.getPosition());
            }

            previousTargetFloor = targetFloor;
        }

        motor.update(targetPosition);

        display.update(
            motor.getPosition(),
            elevator.getDirection());

        long positionError =
            motor.getPosition() - targetPosition;

        // Confirm arrival for 300 ms before opening the door.
        if (!doorCycleActive &&
            elevator.hasRequest(targetFloor) &&
            abs(positionError) <= 5)
        {
            if (!checkingArrival)
            {
                arrivalStart = now;
                checkingArrival = true;
            }

            if (now - arrivalStart >= 300)
            {
                logEvent(
                    "ARRIVAL",
                    targetFloor + 1,
                    targetFloor + 1,
                    elevator.getDirection(),
                    motor.getPosition());

                elevator.updateCurrentFloor(targetFloor);
                elevator.clearRequest(targetFloor);
                cabinButtons[targetFloor].reset();

                doorCycleActive = true;
                doorOpenTimerStarted = false;

                door.open();

                logEvent(
                    "DOOR_OPEN_CMD",
                    targetFloor + 1,
                    targetFloor + 1,
                    elevator.getDirection(),
                    motor.getPosition());

                checkingArrival = false;
            }
        }
        else
        {
            checkingArrival = false;
        }

        // Log position every 100 ms while the elevator is moving.
if (now - lastPrint >= 100 &&
    elevator.getDirection() != IDLE)
{
    lastPrint = now;

    logEvent(
        "POSITION",
        elevator.getCurrentFloor() + 1,
        targetFloor + 1,
        elevator.getDirection(),
        motor.getPosition());
}
    }
}
