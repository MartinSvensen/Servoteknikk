#include "Button.h"
#include "Elevator.h"

CabinButton cabinButtons[8] = {
    CabinButton{1, 49}, CabinButton{2, 48},
    CabinButton{3, 47}, CabinButton{4, 46},
    CabinButton{5, 45}, CabinButton{6, 44},
    CabinButton{7, 43}, CabinButton{8, 42}
};

Elevator elevator;

void setup() {
    Serial.begin(9600);

    for (int i = 0; i < 8; ++i) {
        cabinButtons[i].begin();
    }

    Serial.println("Velg etasje 1-8:");
}

void loop() {
    if (Serial.available() > 0) {
        int choice = Serial.parseInt();

        if (choice >= 1 && choice <= 8) {
            if (choice - 1 == elevator.getCurrentFloor()) {
                cabinButtons[choice - 1].reset();
            } else {
                cabinButtons[choice - 1].press();
            }

            for (int index = 0; index < 8; ++index) {
                if (cabinButtons[index].isPressed()) {
                    elevator.addCabinRequest(index);
                }
            }

            elevator.printUpRequests();
        }
    }
}
