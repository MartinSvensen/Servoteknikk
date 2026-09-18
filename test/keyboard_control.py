import serial
import keyboard
import time

arduino = serial.Serial("COM3", 9600)
time.sleep(2)

print("Klar! Trykk 0-7. ESC avslutter.")

last_key = None

while True:
    current_key = None

    for key in "01234567":
        if keyboard.is_pressed(key):
            current_key = key
            break

    if current_key is not None and current_key != last_key:
        arduino.write(current_key.encode())
        print(f"Sendte etasje {current_key}")

    last_key = current_key

    if keyboard.is_pressed("esc"):
        break

    time.sleep(0.01)

arduino.close()