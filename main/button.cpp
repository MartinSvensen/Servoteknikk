#include <Arduino.h>
#include "Button.h"

Button::Button(int pin) : ledPin(pin) {}

void Button::begin() {
    pinMode(ledPin, OUTPUT);
    updateLED();
}

// Keep the LED synchronized with the button request state.
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

// Indicate an active request by turning on the assigned LED.
void Button::updateLED() {
    digitalWrite(ledPin, pressed ? HIGH : LOW);
}