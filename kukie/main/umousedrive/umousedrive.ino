#include "IRsensor.h"
#include "Driver.h"
#include "Encoder.h"
#include "PID.h"
#include "Top.h"

IRsensor lsensor(left, lc1, lc2, irThCm);
IRsensor dlsensor(diagleft, dlc1, dlc2, irThCm);
IRsensor flsensor(frontleft, flc1, flc2, irThCm);
IRsensor frsensor(frontright, frc1, frc2, irThCm);
IRsensor drsensor(diagright, drc1, drc2, irThCm);
IRsensor rsensor(right, rc1, rc2, irThCm);

Encoder lenc(dwheelmm);

Driver umouse(ain1, ain2, pwma, bin1, bin2, pwmb);

PID pid(kp, kd);

int pwmIncr = 10;

void setup() {
  Serial.begin(9600);
  attachInterrupt(leftwheel, increment, RISING);
}

void loop()
{
  /*
  Serial.println(enc.getSpeed(micros()));
  int sp = (analogRead(7) > 1000) ? 10 : 0;
  umouse.setPWM(sp, sp);
 */
  pidlib();
  //irdistlib();
  //enclib();
  //drivelib();
}

void increment() {
  edge++;
}

void pidlib() {
  if (flsensor.getCm() < 15 && frsensor.getCm() < 15) {
    double lcm = flsensor.getCm();
    double rcm = frsensor.getCm();
    int pidout = floor(pid.getPIDterm(lcm, rcm));
    umouse.setPWM(10 + pidout, 10 - pidout);
    Serial.print(umouse.getPWMA());
    Serial.print("\t");
    Serial.println(umouse.getPWMB());
  } else {
    umouse.setPWM(10, 10);
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

void irdistvallib() {
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

void enclib() {
  Serial.println(lenc.getSpeed(micros()));
}

void drivelib() {
  // Drive
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == ' ') {
      umouse.stop();
    } else {
      if (input == 'w') {
        umouse.setPWM(umouse.getPWMA() + pwmIncr, umouse.getPWMB() + pwmIncr);
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
