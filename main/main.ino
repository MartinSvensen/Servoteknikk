#include "door.h"

Door door;

void setup()
{
    Serial.begin(9600);

    door.begin();

    Serial.println("Start state:");
    Serial.println(door.getState());
    Serial.println(door.isOpen());
    Serial.println(door.isClosed());

    delay(1000);

    door.open();

    Serial.println("After open():");
    Serial.println(door.getState());
    Serial.println(door.isOpen());
    Serial.println(door.isClosed());

    delay(1000);

    door.moveToHalf();

    Serial.println("After moveToHalf():");
    Serial.println(door.getState());
    Serial.println(door.isOpen());
    Serial.println(door.isClosed());

    delay(1000);

    door.close();

    Serial.println("After close():");
    Serial.println(door.getState());
    Serial.println(door.isOpen());
    Serial.println(door.isClosed());

    door.disableMotor();
}

void loop()
{
}