const int ledPins[] = {49, 48, 47, 46, 45, 44, 43, 42};

void setup() {
    Serial.begin(9600);

    for (int i = 0; i < 8; i++) {
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW);
    }
}

void loop() {
    if (Serial.available() > 0) {
        char key = Serial.read();

        if (key >= '0' && key <= '7') {
            int index = key - '0';

            for (int i = 0; i < 8; i++) {
                digitalWrite(ledPins[i], LOW);
            }

            digitalWrite(ledPins[index], HIGH);
        }
    }
}