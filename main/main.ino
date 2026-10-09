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

enum MenuState
{
    MAIN_MENU,
    CABIN_MENU,
    SELECT_HALL_FLOOR,
    SELECT_HALL_DIRECTION
};

MenuState menuState = MAIN_MENU;
int selectedHallFloor = 0; // Indeks 0-7 tilsvarer etasje 1-8

void printMenu()
{
    Serial.println();

    switch (menuState)
    {
    case MAIN_MENU:
        Serial.println("===== HOVEDMENY =====");
        Serial.println("i: Inne i heisen");
        Serial.println("u: Utenfor heisen");
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
        {
            Serial.println("u: Bestill heisen opp");
        }

        if (selectedHallFloor > 0)
        {
            Serial.println("d: Bestill heisen ned");
        }

        Serial.println("b: Tilbake til etasjevalg");
        break;
    }

    Serial.println("m: Vis menyen igjen");
}

void handleMenu() 
{
    static char pendingKey = '\0';
    static bool hasInput = false;
    static bool invalidInput = false;

    if (Serial.available() == 0)
    {
        return;
    }

    char received = Serial.read();

    // Samle tegn fram til linjeskift.
    if (received != '\n' && received != '\r')
    {
        if (!hasInput)
        {
            pendingKey = received;
            hasInput = true;
        }
        else
        {
            // Flere enn ett tegn gjør hele linjen ugyldig.
            invalidInput = true;
        }

        return;
    }

    // Ignorer tomme linjer.
    if (!hasInput)
    {
        return;
    }

    char key = pendingKey;
    bool rejectInput = invalidInput;

    // Nullstill til neste linje.
    pendingKey = '\0';
    hasInput = false;
    invalidInput = false;

    if (rejectInput)
    {
        Serial.println("Skriv bare ett tegn per linje.");
        return;
    }

    if (key == 'm')
    {
        printMenu();
        return;
    }

    if (key == 'b')
    {
        if (menuState == SELECT_HALL_DIRECTION)
        {
            menuState = SELECT_HALL_FLOOR;
        }
        else
        {
            menuState = MAIN_MENU;
        }

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
        else
        {
            Serial.println("Ugyldig valg.");
        }
        break;

    case CABIN_MENU:
        if (key >= '1' && key <= '8')
        {
            int floor = key - '1';

            if (floor == elevator.getCurrentFloor())
            {
                cabinButtons[floor].reset();
                Serial.println("Dette er sist registrerte etasje.");
            }
            else
            {
                cabinButtons[floor].press();
                elevator.addCabinRequest(floor);

                Serial.print("Bestilt etasje: ");
                Serial.println(floor + 1);
            }
        }
        else
        {
            Serial.println("Velg en etasje fra 1 til 8.");
        }
        break;

    case SELECT_HALL_FLOOR:
        if (key >= '1' && key <= '8')
        {
            selectedHallFloor = key - '1';
            menuState = SELECT_HALL_DIRECTION;
            printMenu();
        }
        else
        {
            Serial.println("Velg en etasje fra 1 til 8.");
        }
        break;

    case SELECT_HALL_DIRECTION:
        if (key == 'u' && selectedHallFloor < 7)
        {
            elevator.addHallRequest(selectedHallFloor, UP);
            Serial.println("Oppbestilling registrert.");
        }
        else if (key == 'd' && selectedHallFloor > 0)
        {
            elevator.addHallRequest(selectedHallFloor, DOWN);
            Serial.println("Nedbestilling registrert.");
        }
        else
        {
            Serial.println("Ugyldig retning for denne etasjen.");
        }
        break;
    }
}

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

    printMenu();
}

void loop()
{
    handleMenu();
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

        motor.update(targetPosition);

        // Et nytt mål trenger en ny ankomstsjekk
        if (targetFloor != previousTargetFloor)
        {
            checkingArrival = false;
            previousTargetFloor = targetFloor;
        }

        long positionError = motor.getPosition() - targetPosition;

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
            // Gir oss posisjonen til motoren hvert 100. millisekund
            //Serial.println(motor.getPosition());
        }
    }
}