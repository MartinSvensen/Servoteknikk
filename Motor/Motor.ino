volatile long int encoderReadout = 0;
int motorState = 1;
int reads[3] = {0, 0, 0};
int readTimes[3] = {millis(), millis(), millis()};
float integral = 0;
int error;
int previousError;
unsigned long lastTime = micros(); //last time for integration
int deadBandLimit = 9;

void setup() {
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(20), encoderTrack, RISING);
}

void loop() {
  float PID_sum = PID_frame(1024);
  //direction handling
  if (PID_sum >= 0) {
    motorState = 1;
  } else {
    motorState = 0;
  }
  PID_sum = abs(PID_sum);

  
  //dead band compansation
  if (PID_sum < deadBandLimit) {
    PID_sum = deadBandLimit;
  }
  
  digitalWrite(5, 0);
  digitalWrite(6, motorState);
  analogWrite(7, PID_sum);
  Serial.println(encoderReadout);
}

// attached interrupt function for quadrature encoder
void encoderTrack(){
  if (digitalRead(21) == 1) {
    encoderReadout--;
  } else {
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

float PID_frame(int setpoint) {
  long int sensor = encoderReadout; //lock in sensor read for this loop iteration
  float P = 1;
  float D = 0;
  float I = 0;
  int error = sensor - setpoint;
  //P
  float sum = P * error;
  //I
  integral += numInteg(error, previousError);
  sum += I * integral;
  previousError = error;
  //D
  sum += D * numDeriv(reads, readTimes);
  return sum;
}

float numInteg(int Ierror, int IpreviousError) {
  unsigned long currentTime = micros();
  unsigned long dt = currentTime - lastTime;
  lastTime = currentTime;

  float trapezoid = ((Ierror + IpreviousError) / 2) * dt;
  return trapezoid;
}

float numDeriv(int lastThreeReads[3], int lastThreeTimes[3]) {
  int fStep = lastThreeTimes[2] - lastThreeTimes[1];
  int bStep = lastThreeTimes[1] - lastThreeTimes[0];
  
  float derivative = ((-1*fStep*fStep*lastThreeReads[0]) + ((fStep*fStep - bStep*bStep)*lastThreeReads[1]) + (bStep*bStep*lastThreeReads[2])) / (fStep*bStep*(fStep + bStep));
  return derivative;
}
