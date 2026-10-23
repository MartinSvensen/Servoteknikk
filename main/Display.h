#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Elevator.h"

// Shows the floor nearest to the encoder position and the selected travel direction.
class Display
{
public:
    Display();
    void begin();
    void update(long encoderPosition, Direction direction);

private:
    static const long PULSES_PER_FLOOR = 2096L;
    static const int FLOOR_COUNT = 8;

    LiquidCrystal lcd;
    int lastFloor{-1};
    Direction lastDirection{IDLE};
};

#endif
