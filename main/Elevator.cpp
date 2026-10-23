#include "Elevator.h"
#include <Arduino.h>

Elevator::Elevator()
{
    currentFloor_ = 0;
    targetFloor_ = 0;
    direction_ = IDLE;

    for (int i = 0; i < FLOOR_COUNT; i++)
    {
        upRequests_[i] = false;
        downRequests_[i] = false;
    }
}

// Prints the request queue for debugging.
void Elevator::printUpRequests() const
{
    Serial.print("upRequests (etasje 1–8): ");

    for (int i = 0; i < FLOOR_COUNT; ++i)
    {
        Serial.print(upRequests_[i] ? 1 : 0);
        Serial.print(' ');
    }

    Serial.println();
}


bool Elevator::hasRequest(int floor) const
{
    if (floor < 0 || floor >= FLOOR_COUNT)
    {
        return false;
    }

    return upRequests_[floor] || downRequests_[floor];
}

// Check for pending requests relative to the current floor.

bool Elevator::hasRequestAbove()
{
    for (int floor = currentFloor_ + 1; floor < FLOOR_COUNT; floor++)
    {
        if (upRequests_[floor] || downRequests_[floor])
        {
            return true;
        }
    }

    return false;
}
bool Elevator::hasRequestBelow()
{
    for (int floor = currentFloor_ - 1; floor >= 0; floor--)
    {
        if (upRequests_[floor] || downRequests_[floor])
        {
            return true;
        }
    }

    return false;
}

// Hall requests retain the requested travel direction.

void Elevator::addHallRequest(int floor, Direction requestDirection)
{
    if (floor < 0 || floor >= FLOOR_COUNT)
    {
        return;
    }

    if (requestDirection == UP)
    {
        upRequests_[floor] = true;
    }
    else if (requestDirection == DOWN)
    {
        downRequests_[floor] = true;
    }
}
// Cabin requests are queued according to the destination floor.
void Elevator::addCabinRequest(int floor)
{
    if (floor < 0 || floor >= FLOOR_COUNT)
    {
        return;
    }

    if (floor > currentFloor_)
    {
        upRequests_[floor] = true;
    }
    else if (floor < currentFloor_)
    {
        downRequests_[floor] = true;
    }
}
void Elevator::clearRequest(int floor)
{
    if (floor < 0 || floor >= FLOOR_COUNT)
    {
        return;
    }

    upRequests_[floor] = false;
    downRequests_[floor] = false;
}

// The caller supplies the floor determined by the position system.

void Elevator::updateCurrentFloor(int floor)
{
    if (floor < 0 || floor >= FLOOR_COUNT)
    {
        return;
    }

    currentFloor_ = floor;
}
int Elevator::getCurrentFloor()
{
    return currentFloor_;
}

// Continue in the current direction while requests remain ahead.
// When idle, search above the current floor before searching below.

void Elevator::updateDirection()
{
    if (direction_ == UP)
    {
        if (hasRequestAbove())
        {
            return;
        }

        if (hasRequestBelow())
        {
            direction_ = DOWN;
        }
        else
        {
            direction_ = IDLE;
        }
    }

    else if (direction_ == DOWN)
    {
        if (hasRequestBelow())
        {
            return;
        }

        if (hasRequestAbove())
        {
            direction_ = UP;
        }
        else
        {
            direction_ = IDLE;
        }
    }

    else if (direction_ == IDLE)
    {
        for (int floor = currentFloor_ + 1; floor < FLOOR_COUNT; floor++)
        {
            if (upRequests_[floor] || downRequests_[floor])
            {
                direction_ = UP;
                return;
            }
        }

        for (int floor = currentFloor_ - 1; floor >= 0; floor--)
        {
            if (upRequests_[floor] || downRequests_[floor])
            {
                direction_ = DOWN;
                return;
            }
        }
    }
}
Direction Elevator::getDirection()
{
    return direction_;
}

// Select a requested floor ahead of the encoder position.
// Replace the target only if the new request is closer in the travel direction.
void Elevator::updateTargetFloor(long position)
{
    const int pulsesPerFloor = 2096;
    long activeTargetPosition = targetFloor_ * pulsesPerFloor;

    if (direction_ == UP)
    {
        for (int floor = 0; floor < FLOOR_COUNT; floor++)
        {
            long floorPosition = floor * pulsesPerFloor;

            bool requested =
                upRequests_[floor] || downRequests_[floor];

            bool ahead = floorPosition > position;

            bool beforeTarget =
                floorPosition < activeTargetPosition;

            bool choosingNewTarget =
                targetFloor_ == currentFloor_;

            if (requested && ahead &&
                (choosingNewTarget || beforeTarget))
            {
                targetFloor_ = floor;
                return;
            }
        }
    }

    else if (direction_ == DOWN)
    {
        for (int floor = FLOOR_COUNT - 1; floor >= 0; floor--)
        {
            long floorPosition = floor * pulsesPerFloor;

            bool requested =
                upRequests_[floor] || downRequests_[floor];

            bool ahead = floorPosition < position;

            bool beforeTarget =
                floorPosition > activeTargetPosition;

            bool choosingNewTarget =
                targetFloor_ == currentFloor_;

            if (requested && ahead &&
                (choosingNewTarget || beforeTarget))
            {
                targetFloor_ = floor;
                return;
            }
        }
    }
}

int Elevator::getTargetFloor()
{
    return targetFloor_;
}

// Requires currentFloor_ to be updated before calling.
// Stop for matching-direction requests, or opposite-direction requests
// when no further requests remain ahead.

bool Elevator::shouldStop()
{
    if (direction_ == UP)
    {
        if (upRequests_[currentFloor_])
        {
            return true;
        }

        if (!hasRequestAbove() && downRequests_[currentFloor_])
        {
            return true;
        }
    }

    else if (direction_ == DOWN)
    {
        if (downRequests_[currentFloor_])
        {
            return true;
        }

        if (!hasRequestBelow() && upRequests_[currentFloor_])
        {
            return true;
        }
    }

    return false;
}
