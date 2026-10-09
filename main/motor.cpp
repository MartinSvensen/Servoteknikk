#include "Motor.h"
#include <Arduino.h>

void Motor::begin()
{
    pinMode(20, INPUT_PULLUP);
    pinMode(21, INPUT_PULLUP);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);

    digitalWrite(6, LOW);
    stop();

    unsigned long now = millis();

    for (int i = 0; i < 3; i++)
    {
        readTimes[i] = now;
    }
    lastTime = micros();
}

void Motor::encoderTrack()
{
    if (digitalRead(21) == 1)
    {
        encoderReadout--;
    }
    else
    {
        encoderReadout++;
    }
    // keep track of times for the last three reads
    readTimes[0] = readTimes[1];
    readTimes[1] = readTimes[2];
    readTimes[2] = millis();

    // keep track of values for the last three reads
    reads[0] = reads[1];
    reads[1] = reads[2];
    reads[2] = encoderReadout;
}

long Motor::getPosition() const
{
    noInterrupts();
    long position = encoderReadout;
    interrupts();

    return position;
}

float Motor::PID_frame(int setpoint)
{
    long sensor = getPosition();

    float P = 0.9f;
    float I = 0.0f;
    float D = -100.0f;

    long error = sensor - setpoint;

    // P-leddet
    float sum = P * error;

    // I-leddet
    integral += numInteg(error, previousError);
    sum += I * integral;
    previousError = error;

    // D-leddet
    if (D != 0)
    {
        sum += D * numDeriv();
    }

    return sum;
}

void Motor::stop()
{
    digitalWrite(5, LOW);
    analogWrite(7, 0);
}

void Motor::update(int setpoint)
{
    float output = PID_frame(setpoint);

    if (output == 0.0f)
    {
        stop();
        return;
    }

    int motorState = output >= 0 ? HIGH : LOW;
    float pwm = abs(output);

    pwm = constrain(pwm, 10.0f, 100.0f);

    digitalWrite(5, LOW);
    digitalWrite(6, motorState);
    analogWrite(7, (int)pwm);
}

float Motor::numInteg(long error, long previousError)
{
    unsigned long currentTime = micros();
    unsigned long elapsed = currentTime - lastTime;
    lastTime = currentTime;

    float dt = elapsed * 1e-6f;

    float averageError =
        0.5f * ((float)error + (float)previousError);

    return averageError * dt;
}

float Motor::numDeriv()
{
    long positions[3];
    unsigned long times[3];

    // Kopier hele målehistorikken uten avbrudd
    noInterrupts();

    for (int i = 0; i < 3; i++)
    {
        positions[i] = reads[i];
        times[i] = readTimes[i];
    }

    interrupts();

    if (millis() - times[2] >= 100)
    {
        return 0;
    }

    float fStep = (times[2] - times[1]);
    float bStep = (times[1] - times[0]);

    if (fStep <= 0 || bStep <= 0)
    {
        return 0;
    }

    float numerator =
        -fStep * fStep * (float)positions[0] + (fStep * fStep - bStep * bStep) * (float)positions[1] + bStep * bStep * (float)positions[2];

    float denominator =
        fStep * bStep * (fStep + bStep);

    long derivative = (long)(numerator / denominator);
    return derivative;
}