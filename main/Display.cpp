#include "Display.h"

namespace
{
    byte arrowUp[8] = {
        B00100, B01110, B11111, B00100,
        B00100, B00100, B00100, B00000};

    byte arrowDown[8] = {
        B00000, B00100, B00100, B00100,
        B00100, B11111, B01110, B00100};
}

Display::Display() : lcd(41, 40, 37, 36, 35, 34) {}

void Display::begin()
{
    lcd.begin(16, 2);
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);

    lcd.createChar(0, arrowUp);
    lcd.createChar(1, arrowDown);

    lcd.setCursor(0, 0);
    lcd.print("Floor:");
    lcd.setCursor(0, 1);
    lcd.print("Direction:");
    // Force first refresh, including the IDLE indicator.
    lastFloor = -1;
    lastDirection = static_cast<Direction>(-1);
}

void Display::update(long encoderPosition, Direction direction)
{
    // Round to the nearest floor (0-based internally), then show floors 1-8.
    long floorIndex = (encoderPosition >= 0)
                          ? (encoderPosition + PULSES_PER_FLOOR / 2) / PULSES_PER_FLOOR
                          : 0;
    floorIndex = constrain(floorIndex, 0L, static_cast<long>(FLOOR_COUNT - 1));
    int floor = static_cast<int>(floorIndex) + 1;

    if (floor != lastFloor)
    {
        lcd.setCursor(6, 0);
        lcd.print("   ");
        lcd.setCursor(6, 0);
        lcd.print(floor);
        lastFloor = floor;
    }

    if (direction != lastDirection)
    {
        lcd.setCursor(10, 1);
        lcd.print("      ");
        lcd.setCursor(10, 1);
        if (direction == UP)
        {
            lcd.write(byte(0));
        }
        else if (direction == DOWN)
        {
            lcd.write(byte(1));
        }
        else
        {
            lcd.print('-');
        }
        lastDirection = direction;
    }
}
