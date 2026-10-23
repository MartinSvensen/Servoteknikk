# Elevator Control System

An eight-floor elevator control system developed for MAS246 – Servoteknikk at the University of Agder.

## Requirements

- Arduino IDE
- Arduino Mega 2560 or compatible hardware with the required pin mapping
- Connected elevator motor, encoder, door motor, DAC, and indicator LEDs
- All supplied `.ino`, `.cpp`, and `.h` files

## How to Run

1. Place all source files in the same Arduino sketch folder and open `main.ino` in Arduino IDE.
2. Select the correct board and serial port.
3. Compile and upload the sketch.
4. Open the Serial Monitor and set the baud rate to **9600**. Set the line ending to **Newline**.
5. Use the menu to submit elevator requests.

## Serial Monitor Controls

| Input | Action |
| --- | --- |
| `i` | Enter the cabin menu |
| `u` | Enter the hall menu from the main menu; request upward travel in the hall direction menu |
| `d` | Request downward travel in the hall direction menu |
| `1`–`8` | Select a floor |
| `b` | Go back |
| `m` | Display the current menu again |

For a **cabin request**, press `i` and select a destination floor (1–8).

For a **hall request**, press `u`, select the floor you are on (1–8), and then choose `u` (up) or `d` (down). Up is unavailable on floor 8 and down is unavailable on floor 1.

## Notes

The system tracks elevator position using an encoder and serves queued requests according to its direction-based scheduling logic. The door opens at a serviced floor, stays open briefly, and then closes automatically.

The connected hardware must match the pin assignments in the source code. See the project report for hardware details and system design.
