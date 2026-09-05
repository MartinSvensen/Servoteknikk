int ledPins[]{49, 48, 47, 46, 45, 44, 43, 42};
int buttonPins[]{22, 23, 24, 25, 26, 27, 28, 29};
int buttonValue[8];

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < 8; i++) {
        buttonValue[i] = digitalRead(buttonPins[i]);
        if (buttonValue[i] == 1) {
          Serial.print("Button ");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(buttonValue[i]);
        }
        while (buttonValue[i] == 1) {
          digitalWrite(ledPins[i], HIGH);
          delay(100);

          digitalWrite(ledPins[i], LOW);
          delay(100);

          buttonValue[i] = digitalRead(buttonPins[i]);
        }
    }
}
