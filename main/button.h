#ifndef BUTTON_H
#define BUTTON_H

enum class HallDirection { Up, Down };

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

    void updateLED();
};

class FloorButton : public Button {
public:
    FloorButton(int floor, HallDirection direction, int ledPin)
        : Button(ledPin), floor(floor), direction(direction) {}

private:
    int floor;
    HallDirection direction;
};

class CabinButton : public Button {
public:
    CabinButton(int floor, int ledPin)
        : Button(ledPin), targetFloor(floor) {}

private:
    int targetFloor;
};

#endif