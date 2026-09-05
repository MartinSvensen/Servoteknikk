int potValue;
float threshold{2.5/5.0*1023};
int ledPin{42};

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
}

void loop() {
    potValue = analogRead(A0);

    Serial.println(potValue);

    if (potValue > threshold) {
        digitalWrite(ledPin, HIGH);
    } else {
        digitalWrite(ledPin, LOW);
    }

    delay(100);
}