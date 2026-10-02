#include <Arduino.h>
#include "Button.h"

Button::Button(int pin) : ledPin(pin) {}

void Button::begin() {
    pinMode(ledPin, OUTPUT);
    updateLED();
}

void Button::press() {
    pressed = true;
    updateLED();
}

void Button::reset() {
    pressed = false;
    updateLED();
}

bool Button::isPressed() const {
    return pressed;
}

void Button::updateLED() {
    digitalWrite(ledPin, pressed ? HIGH : LOW);
}