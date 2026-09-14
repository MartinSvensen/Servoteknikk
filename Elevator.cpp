#include "Elevator.h"

Elevator::Elevator() {
    currentFloor = 0;
    targetFloor = 0;
    direction = IDLE;

    for (int i = 0; i < FLOOR_COUNT; i++) {
        upRequests[i] = false;
        downRequests[i] = false;
    }
}


// Hjelpefunksjoner for updateDirection() for å sjekke om det finnes forespørsler over eller under heisen

bool Elevator::hasRequestAbove() {
    for (int floor = currentFloor + 1; floor < FLOOR_COUNT; floor++) {
        if (upRequests[floor] || downRequests[floor]) {
            return true;
        }
    }

    return false;
}
bool Elevator::hasRequestBelow() {
    for (int floor = currentFloor - 1; floor >= 0; floor--) {
        if (upRequests[floor] || downRequests[floor]) {
            return true;
        }
    }

    return false;
}

// Legger til og fjerner forespørsler fra hallen og kabinen.

void Elevator::addHallRequest(int floor, Direction requestDirection) {
    if (floor < 0 || floor >= FLOOR_COUNT) {
        return;
    }

    if (requestDirection == UP) {
        upRequests[floor] = true;
    }
    else if (requestDirection == DOWN) {
        downRequests[floor] = true;
    }
}
void Elevator::addCabinRequest(int floor) {
    if (floor < 0 || floor >= FLOOR_COUNT) {
        return;
    }

    if (floor > currentFloor) {
        upRequests[floor] = true;
    }
    else if (floor < currentFloor) {
        downRequests[floor] = true;
    }
}
void Elevator::clearRequest(int floor) {
    if (floor < 0 || floor >= FLOOR_COUNT) {
        return;
    }

    upRequests[floor] = false;
    downRequests[floor] = false;
}

// Brukes når posisjonssystemet har bestemt hvilken etasje heisen er på.

void Elevator::updateCurrentFloor(int floor) {
    if (floor < 0 || floor >= FLOOR_COUNT) {
        return;
    }

    currentFloor = floor;
}
int Elevator::getCurrentFloor() {
    return currentFloor;
}

/*
Når heisen er IDLE, prioriteres forespørsler over currentFloor.
Hvis ingen finnes over, sjekkes forespørsler under.

TODO:
Når posisjonssystemet er implementert, kan denne strategien
endres til å velge retning basert på nærmeste forespørsel.
*/

void Elevator::updateDirection() {
    if (direction == UP) {
        if (hasRequestAbove()) {
            return;
        }

        if (hasRequestBelow()) {
            direction = DOWN;
        }
        else {
            direction = IDLE;
        }
    }

    else if (direction == DOWN) {
        if (hasRequestBelow()) {
            return;
        }

        if (hasRequestAbove()) {
            direction = UP;
        }
        else {
            direction = IDLE;
        }
    }

    else if (direction == IDLE) {
        for (int floor = currentFloor + 1; floor < FLOOR_COUNT; floor++) {
            if (upRequests[floor] || downRequests[floor]) {
                direction = UP;
                return;
            }
        }

        for (int floor = currentFloor - 1; floor >= 0; floor--) {
            if (upRequests[floor] || downRequests[floor]) {
                direction = DOWN;
                return;
            }
        }
    }
}
Direction Elevator::getDirection() {
    return direction;
}

void Elevator::updateTargetFloor() {
    if (direction == UP) {
        for (int floor = currentFloor + 1; floor < FLOOR_COUNT; floor++) {
            if (upRequests[floor] || downRequests[floor]) {
                targetFloor = floor;
                return;
            }
        }
    }

    else if (direction == DOWN) {
        for (int floor = currentFloor - 1; floor >= 0; floor--) {
            if (upRequests[floor] || downRequests[floor]) {
                targetFloor = floor;
                return;
            }
        }
    }
}
int Elevator::getTargetFloor() {
    return targetFloor;
}

/*
currentFloor må være oppdatert før shouldStop() kalles.

Hvis heisen er på en etasje med en forespørsel i samme retning
som heisen går, skal den stoppe.

Hvis forespørselen er i motsatt retning, stopper heisen bare
dersom det ikke finnes flere forespørsler videre i gjeldende retning.
*/

bool Elevator::shouldStop() {
    if (direction == UP) {
        if (upRequests[currentFloor]) {
            return true;
        }

        if (!hasRequestAbove() && downRequests[currentFloor]) {
            return true;
        }
    }

    else if (direction == DOWN) {
        if (downRequests[currentFloor]) {
            return true;
        }

        if (!hasRequestBelow() && upRequests[currentFloor]) {
            return true;
        }
    }

    return false;
}


