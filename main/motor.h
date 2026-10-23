#ifndef MOTOR_H
#define MOTOR_H

class Motor
{
private:
    // Encoder position and recent samples used for velocity estimation.
    volatile long int encoderReadout{0};
    int reads[3]{0, 0, 0};
    unsigned long readTimes[3]{0, 0, 0};

    // PID controller state.
    float integral{0};
    long previousError{0};
    unsigned long lastTime{0};

    float PID_frame(int setpoint);
    float numInteg(long error, long previousError);
    float numDeriv();
    void stop();

public:
    // Initialize motor and encoder pins.
    void begin();
    // Update encoder position on an interrupt.
    void encoderTrack();
    long getPosition() const;
    // Apply PID control using the target encoder position.
    void update(int setpoint);
};

#endif