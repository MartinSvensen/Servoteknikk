#ifndef BUTTON_H
#define BUTTON_H

// Direction selected by a passenger waiting outside the elevator.
enum class HallDirection { Up, Down };

// Represents a button with an LED indicating its request state.
class Button {
public:
    Button(int ledPin);

    void begin();
    void press();
    void reset();
    bool isPressed() const;

private:
    bool pressed{false};
    int ledPin;

    // Synchronizes the LED with the stored button state.
    void updateLED();
};

// Hall button associated with a floor and travel direction.
class FloorButton : public Button {
public:
    FloorButton(int floor, HallDirection direction, int ledPin)
        : Button(ledPin), floor(floor), direction(direction) {}

private:
    int floor;
    HallDirection direction;
};

// Cabin button associated with a destination floor.
class CabinButton : public Button {
public:
    CabinButton(int floor, int ledPin)
        : Button(ledPin), targetFloor(floor) {}

private:
    int targetFloor;
};

#endif