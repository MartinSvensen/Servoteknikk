int ledPins[]{49, 48, 47, 46, 45, 44, 43, 42};

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < 8; i++) {
        digitalWrite(ledPins[i], HIGH);
        delay(100);

        digitalWrite(ledPins[i], LOW);
        delay(100);
    }
  
  for (int i = 6; i > 0; i--) {
        digitalWrite(ledPins[i], HIGH);
        delay(100);

        digitalWrite(ledPins[i], LOW);
        delay(100);
    }

}


