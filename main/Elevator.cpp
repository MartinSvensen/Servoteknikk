#include "Elevator.h"
#include <Arduino.h>

Elevator::Elevator() {
    currentFloor_ = 0;
    targetFloor_ = 0;
    direction_ = IDLE;

    for (int i = 0; i < FLOOR_COUNT; i++) {
        upRequests_[i] = false;
        downRequests_[i] = false;
    }
}

// Used for debugging, no special use in the final implementation.
void Elevator::printUpRequests() const {
    Serial.print("upRequests (etasje 1–8): ");

    for (int i = 0; i < FLOOR_COUNT; ++i) {
        Serial.print(upRequests_[i] ? 1 : 0);
        Serial.print(' ');
    }

    Serial.println();
}

// Hjelpefunksjoner for updateDirection() for å sjekke om det finnes forespørsler over eller under heisen

bool Elevator::hasRequestAbove() {
    for (int floor = currentFloor_ + 1; floor < FLOOR_COUNT; floor++) {
        if (upRequests_[floor] || downRequests_[floor]) {
            return true;
        }
    }

    return false;
}
bool Elevator::hasRequestBelow() {
    for (int floor = currentFloor_ - 1; floor >= 0; floor--) {
        if (upRequests_[floor] || downRequests_[floor]) {
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
        upRequests_[floor] = true;
    }
    else if (requestDirection == DOWN) {
        downRequests_[floor] = true;
    }
}
void Elevator::addCabinRequest(int floor) {
    if (floor < 0 || floor >= FLOOR_COUNT) {
        return;
    }

    if (floor > currentFloor_) {
        upRequests_[floor] = true;
    }
    else if (floor < currentFloor_) {
        downRequests_[floor] = true;
    }
}
void Elevator::clearRequest(int floor) {
    if (floor < 0 || floor >= FLOOR_COUNT) {
        return;
    }

    upRequests_[floor] = false;
    downRequests_[floor] = false;
}

// Brukes når posisjonssystemet har bestemt hvilken etasje heisen er på.

void Elevator::updateCurrentFloor(int floor) {
    if (floor < 0 || floor >= FLOOR_COUNT) {
        return;
    }

    currentFloor_ = floor;
}
int Elevator::getCurrentFloor() {
    return currentFloor_;
}

/*
Når heisen er IDLE, prioriteres forespørsler over currentFloor.
Hvis ingen finnes over, sjekkes forespørsler under.

TODO:
Når posisjonssystemet er implementert, kan denne strategien
endres til å velge retning basert på nærmeste forespørsel.
*/

void Elevator::updateDirection() {
    if (direction_ == UP) {
        if (hasRequestAbove()) {
            return;
        }

        if (hasRequestBelow()) {
            direction_ = DOWN;
        }
        else {
            direction_ = IDLE;
        }
    }

    else if (direction_ == DOWN) {
        if (hasRequestBelow()) {
            return;
        }

        if (hasRequestAbove()) {
            direction_ = UP;
        }
        else {
            direction_ = IDLE;
        }
    }

    else if (direction_ == IDLE) {
        for (int floor = currentFloor_ + 1; floor < FLOOR_COUNT; floor++) {
            if (upRequests_[floor] || downRequests_[floor]) {
                direction_ = UP;
                return;
            }
        }

        for (int floor = currentFloor_ - 1; floor >= 0; floor--) {
            if (upRequests_[floor] || downRequests_[floor]) {
                direction_ = DOWN;
                return;
            }
        }
    }
}
Direction Elevator::getDirection() {
    return direction_;
}

void Elevator::updateTargetFloor(long position) {
    const int pulsesPerFloor = 200;
    long activeTargetPosition = targetFloor_ * pulsesPerFloor;

    if (direction_ == UP) {
        for (int floor = 0; floor < FLOOR_COUNT; floor++) {
            long floorPosition = floor * pulsesPerFloor;

            bool requested =
                upRequests_[floor] || downRequests_[floor];

            bool ahead = floorPosition > position;

            bool beforeTarget =
                floorPosition < activeTargetPosition;

            bool choosingNewTarget =
                targetFloor_ == currentFloor_;

            if (requested && ahead &&
                (choosingNewTarget || beforeTarget)) {
                targetFloor_ = floor;
                return;
            }
        }
    }

    else if (direction_ == DOWN) {
        for (int floor = FLOOR_COUNT - 1; floor >= 0; floor--) {
            long floorPosition = floor * pulsesPerFloor;

            bool requested =
                upRequests_[floor] || downRequests_[floor];

            bool ahead = floorPosition < position;

            bool beforeTarget =
                floorPosition > activeTargetPosition;

            bool choosingNewTarget =
                targetFloor_ == currentFloor_;

            if (requested && ahead &&
                (choosingNewTarget || beforeTarget)) {
                targetFloor_ = floor;
                return;
            }
        }
    }
}

int Elevator::getTargetFloor() {
    return targetFloor_;
}

/*
currentFloor må være oppdatert før shouldStop() kalles.

Hvis heisen er på en etasje med en forespørsel i samme retning
som heisen går, skal den stoppe.

Hvis forespørselen er i motsatt retning, stopper heisen bare
dersom det ikke finnes flere forespørsler videre i gjeldende retning.
*/

bool Elevator::shouldStop() {
    if (direction_ == UP) {
        if (upRequests_[currentFloor_]) {
            return true;
        }

        if (!hasRequestAbove() && downRequests_[currentFloor_]) {
            return true;
        }
    }

    else if (direction_ == DOWN) {
        if (downRequests_[currentFloor_]) {
            return true;
        }

        if (!hasRequestBelow() && upRequests_[currentFloor_]) {
            return true;
        }
    }

    return false;
}


