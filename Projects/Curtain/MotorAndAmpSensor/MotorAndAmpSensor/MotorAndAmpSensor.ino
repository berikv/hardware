
#include <HM10.h>
#include "Timer.h"
#include <SoftwareSerial.h>

/* Configuration */
SoftwareSerial bleSerial(8, 9);

const int aaPin = 3;
const int abPin = 5;
const int ampereSensorPin = A3;
const double maxAmpValue = 515.6;

Timer sampleAmpsTimer;
Timer bleTimer;
Timer updateCurrentPositionTimer;

//HM10SoftwareSerial ble(8, 9);

/* End configuration */

// See https://github.com/KhaosT/HAP-NodeJS/blob/master/lib/gen/HomeKitTypes.js#L493
#define POSITION_OPEN 100
#define POSITION_CLOSED 0
#define POSITION_UNKNOWN 255
byte currentPosition = POSITION_UNKNOWN;
byte startPosition = POSITION_UNKNOWN;

// See https://github.com/KhaosT/HAP-NodeJS/blob/master/lib/gen/HomeKitTypes.js#L1564
byte targetPosition = POSITION_OPEN;

// See https://github.com/KhaosT/HAP-NodeJS/blob/master/lib/gen/HomeKitTypes.js#L1138
#define DECREASING 0
#define INCREASING 1
#define STOPPED 2

int positionState = STOPPED;

#define STEP_COUNT (abs(POSITION_OPEN - POSITION_CLOSED))
#define RATE_UNKNOWN 0
#define FULL_SWEEP_START_TIME_UNKNOWN 0

bool currentPositionKnown = false;
double rate = RATE_UNKNOWN;
long fullSweepStartTime = FULL_SWEEP_START_TIME_UNKNOWN;
long motorStartTime = 0;

void setup() {
  pinMode(aaPin, OUTPUT);
  pinMode(abPin, OUTPUT);
  Serial.begin(115200);  
  bleSerial.begin(9600);

  sampleAmpsTimer.every(5, sampleAmps);
  bleTimer.every(1000, checkBLE);
  updateCurrentPositionTimer.every(100, updateCurrentPosition);
    
  updateMotor();
}

void updateCurrentPosition() {
  if (positionState == STOPPED) { return; }
  if (!currentPositionKnown) { return; }
  if (rate == RATE_UNKNOWN) { return; }

  long timePassed = millis() - motorStartTime;
  //long expectedDuration = long(abs(startPosition - targetPosition)) * fullSweepDuration / 100;
  //long timeLeft = expectedDuration - timePassed;

  int direction = positionState == INCREASING ? 1 : -1;
  
//  Serial.print("rate ");
//  Serial.print(rate);
//  Serial.print(" time passed ");
//  Serial.print(timePassed);
//  Serial.print(" start position ");
//  Serial.print(startPosition);
//  Serial.print(" adjustment ");
//  Serial.println(opening * long(timePassed * rate));
  currentPosition = startPosition + direction * long(timePassed * rate);

  Serial.print("current position: ");
  Serial.println(currentPosition);
  updateMotor();
}

#define shouldClose (currentPositionKnown && currentPosition > targetPosition)
#define shouldOpen (!currentPositionKnown || currentPosition < targetPosition)
#define shouldStop (currentPositionKnown && !(targetPosition == POSITION_CLOSED || targetPosition == POSITION_OPEN) && abs(currentPosition - targetPosition) == 0)

void updateMotor() {
  Serial.print("Current position ");
  Serial.print(currentPositionKnown ? currentPosition : -1);
  Serial.print(" target ");
  Serial.println(targetPosition);

  if ( (shouldStop && positionState == STOPPED)
    || (shouldOpen && positionState == INCREASING)
    || (shouldClose && positionState == DECREASING)) {
      return;
  }

  if (shouldStop) {
    stopMotor();
  } else if (shouldOpen || shouldClose) { 
    startMotor();
  }
}

void startMotor() {
  bool opening = shouldOpen;
  positionState = opening ? INCREASING : DECREASING;

  Serial.print("Start motor to ");
  Serial.println(INCREASING ? "open" : "close");

  digitalWrite(aaPin, 0);
  digitalWrite(abPin, 0);  

  delay(1000);
  for (int i = 0; i < 10; i++) { sampleAmps(); }

  motorStartTime = millis();
  if (currentPositionKnown
    && (currentPosition == POSITION_CLOSED || currentPosition == POSITION_OPEN)
    && (rate == RATE_UNKNOWN || targetPosition == POSITION_CLOSED || targetPosition == POSITION_OPEN)) {
      Serial.println("Fullsweep start");
    fullSweepStartTime = motorStartTime;
  }

  if (currentPositionKnown) {
    startPosition = currentPosition;
  }
  
  digitalWrite(aaPin, opening ? 0 : 1);  
  digitalWrite(abPin, opening ? 1 : 0);
}

void stopMotor() {
  Serial.println("Stop motor");
    positionState = STOPPED;
    digitalWrite(aaPin, 0);
    digitalWrite(abPin, 0);  
}

void loop() {
  sampleAmpsTimer.update();
  bleTimer.update();
  updateCurrentPositionTimer.update();

  if (Serial.available() > 1) {
    char targetPos[3] = {0};
    targetPos[0] = Serial.read();
    targetPos[1] = Serial.read();

    char *endPtr;
    targetPosition = strtol(targetPos, &endPtr, 16);
    updateMotor();
  }
}

double runningAverageAmps = 0;
const double runningAverageRate = 0.05;
void sampleAmps() {
  int ampValue = readAmps(ampereSensorPin);
  if (runningAverageAmps == 0) { runningAverageAmps = ampValue; }
  runningAverageAmps = ampValue * runningAverageRate + (runningAverageAmps * (1 - runningAverageRate));
//  Serial.print("Apms ");
//  Serial.print(ampValue);
//  Serial.print(" average ");
//  Serial.println(runningAverageAmps);

  if (runningAverageAmps > maxAmpValue) {
    Serial.print("Above threshold ");
    Serial.println(maxAmpValue);
    
    currentPositionKnown = true;
    currentPosition = positionState == INCREASING ? POSITION_OPEN : POSITION_CLOSED;

    stopMotor();

    if (fullSweepStartTime != 0) {
      fullSweepStartTime = 0;
      rate = double(millis() - fullSweepStartTime) / STEP_COUNT;
      Serial.print("Rate: ");
      Serial.println(rate);
    }
    
    updateMotor();
  }
}

int readAmps(int pin) {
 return 1024 - analogRead(ampereSensorPin);
}

void checkBLE() {
  char command[3] = {0};

  if (bleSerial.available() < 2) { return; }

  command[0] = bleSerial.read();
  command[1] = bleSerial.read();

  Serial.print("<- ");
  Serial.println(command);

  if (strcmp(command, "CP") == 0) {
    char pos[5];
    sprintf(pos, "CP%02X", currentPosition);
    Serial.print("-> ");
    Serial.println(pos);
    bleSerial.write(pos);
  }

  if (strcmp(command, "PS") == 0) {
    char resp[5];
    sprintf(resp, "PS%02X", positionState);
    Serial.print("-> ");
    Serial.println(resp);
    bleSerial.write(resp);
  }

  if (strcmp(command, "TP") == 0) {
    char targetPos[3] = {0};
    targetPos[0] = bleSerial.read();
    targetPos[1] = bleSerial.read();

    char *endPtr;
    targetPosition = strtol(targetPos, &endPtr, 16);
    
    Serial.print("<- TP");
    Serial.println(targetPosition);
    bleSerial.write("TP");
  }

  updateMotor();
}

