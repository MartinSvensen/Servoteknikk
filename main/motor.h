#ifndef MOTOR_H
#define MOTOR_H

class Motor
{
private:
    // Medlemsvariabler kommer her
    volatile long int encoderReadout{0};
    int reads[3]{0, 0, 0};
    unsigned long readTimes[3]{0, 0, 0};

    float integral{0};
    long previousError{0};
    unsigned long lastTime{0};

    float PID_frame(int setpoint);
    float numInteg(long error, long previousError);
    float numDeriv();
    void stop();

public:
    // Metoder kommer her
    void begin();
    void encoderTrack();
    long getPosition() const;
    void update(int setpoint);
};

#endif