#include "IRsensor.h"
#include "Driver.h"
#include "Encoder.h"
#include "PID.h"
#include "Top.h"

IRsensor lsensor(left, lc1, lc2, irThSide);
IRsensor dlsensor(diagleft, dlc1, dlc2, irThDiag);
IRsensor flsensor(frontleft, flc1, flc2, irThFront);
IRsensor frsensor(frontright, frc1, frc2, irThFront);
IRsensor drsensor(diagright, drc1, drc2, irThDiag);
IRsensor rsensor(right, rc1, rc2, irThSide);

Encoder lenc(dwheelmm);

Driver umouse(ain1, ain2, pwma, bin1, bin2, pwmb);

PID pid(kp, kd);

float arr[30];
int arrlen = 0;
int arrlimit = 10;

int pwmIncr = 10;

void setup() {
  Serial.begin(9600);
  attachInterrupt(leftwheel, increment, RISING);
}

void loop()
{
  int cm = 90;
  driveForward(cm * edgePerCm);
  //pidlib();
  //irwalllib();
  //enclib();
  //drivelib();
}

void increment() {
  edge++;
}

// This function will drive the mouse forward for a specified number of edges.
void driveForward(unsigned long deltaEdge) {
  unsigned long stopEdge = edge + deltaEdge;
  while (edge < stopEdge) {
    if (lsensor.hasWall() && rsensor.hasWall()) { // do pid
      double lcm = flsensor.getCm();
      double rcm = frsensor.getCm();
      int pidout = floor(pid.getPIDterm(lcm, rcm));
      umouse.setPWM(forwardSpeed - pidout, forwardSpeed + pidout);
    } else {
      umouse.setPWM(forwardSpeed, forwardSpeed);
    }
  }
}

void pidlib() {
  int basespeed = 10;
  if (lsensor.hasWall() && rsensor.hasWall()) {
    double lcm = flsensor.getCm();
    double rcm = frsensor.getCm();
    int pidout = floor(pid.getPIDterm(lcm, rcm));
    umouse.setPWM(basespeed - pidout, basespeed + pidout);
    Serial.print(umouse.getPWMA());
    Serial.print("\t");
    Serial.println(umouse.getPWMB());
  } else {
    umouse.setPWM(basespeed, basespeed);
    Serial.println("no wall");
  }
}

void irdistlib() {
    // IR
  Serial.print("Distance:\t");
  Serial.print(lsensor.getCm());
  Serial.print("\t");
  Serial.print(dlsensor.getCm());
  Serial.print("\t");
  Serial.print(flsensor.getCm()); 
  Serial.print("\t");
  Serial.print(frsensor.getCm()); 
  Serial.print("\t");
  Serial.print(drsensor.getCm()); 
  Serial.print("\t");
  Serial.println(rsensor.getCm());
}

void irvallib() {
    // IR
  Serial.print("Distance:\t");
  Serial.print(lsensor.getVal());
  Serial.print("\t");
  Serial.print(dlsensor.getVal());
  Serial.print("\t");
  Serial.print(flsensor.getVal()); 
  Serial.print("\t");
  Serial.print(frsensor.getVal()); 
  Serial.print("\t");
  Serial.print(drsensor.getVal()); 
  Serial.print("\t");
  Serial.println(rsensor.getVal());
}

void irwalllib() {
    // IR
  Serial.print("Distance:\t");
  Serial.print(lsensor.hasWall());
  Serial.print("\t");
  Serial.print(dlsensor.hasWall());
  Serial.print("\t");
  Serial.print(flsensor.hasWall()); 
  Serial.print("\t");
  Serial.print(frsensor.hasWall()); 
  Serial.print("\t");
  Serial.print(drsensor.hasWall()); 
  Serial.print("\t");
  Serial.println(rsensor.hasWall());
}


void enclib() {
  if (arrlen < arrlimit) {
    if (edge > 50) {
      arr[arrlen] = lenc.getSpeed(micros());
      arrlen++;
    }
  } else {
    float sum = 0;
    for (int i = 2; i < arrlimit; i++) {
      sum += arr[i];
    }
    Serial.println(sum / (arrlimit - 2));
  } 
  //Serial.println(edge);
  //Serial.println(lenc.getSpeed(micros()));
}

void drivelib() {
  // Drive
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == 's') {
      umouse.stop();
    } else if (input == ' ') {
      umouse.brake();
    } else {
      if (input == 'w') {
        umouse.setPWM(umouse.getPWMA() + pwmIncr, umouse.getPWMB() + pwmIncr);
        arrlen = 0;
      } else if (input == 's') {
        umouse.setPWM(umouse.getPWMA() - pwmIncr, umouse.getPWMB() - pwmIncr);
      } else if (input == 'a') {
        umouse.setPWM(umouse.getPWMA() - pwmIncr, umouse.getPWMB() + pwmIncr);
      } else if (input == 'd') {
        umouse.setPWM(umouse.getPWMA() + pwmIncr, umouse.getPWMB() - pwmIncr);
      }
    }
  }
}
