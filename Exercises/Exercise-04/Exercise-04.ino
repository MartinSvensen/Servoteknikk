int joyX = A1;
int joyY = A2;

int joyValX;
int joyValY;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  joyValX = analogRead(joyX);
  joyValY = analogRead(joyY);

  Serial.print("The x-value is: ");
  Serial.println(joyValX);
 
  Serial.print("The y-value is: ");
  Serial.println(joyValY);

   delay(3000);
}
