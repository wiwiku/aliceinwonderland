#include <IRsensor.h>
#include <Driver.h>
#include <Encoder.h>

// *** Constants ***
// IR (Analog)
const int left = A3;
const int frontleft = A2;
const int frontright = A1;
const int right = A0;
const int irThCm = 8;

const float lc1 = 1123.1;
const float lc2 = -1.201;
const float flc1 = 1093.8;
const float flc2 = -1.257;
const float frc1 = 1010.0;
const float frc2 = -1.168;
const float rc1 = 897.89;
const float rc2 = -1.178;

IRsensor lsense(left, lc1, lc2, irThCm);
IRsensor flsense(frontleft, flc1, flc2, irThCm);
IRsensor frsense(frontright, frc1, frc2, irThCm);
IRsensor rsense(right, rc1, rc2, irThCm);

// Speed (Analog)
const int dwheelmm = 32;
const int leftwheel = 0; // D2 pin (interrupt 0)
const int rightwheel = 1; // D3 pin (interrupt 1)

Encoder enc(dwheelmm);

// Drive (Digital)
const int ain1 =  13;
const int ain2 =  12;
const int pwma =  10;
const int bin1 =  6;
const int bin2 =  7;
const int pwmb =  11;

Driver umouse(ain1, ain2, pwma, bin1, bin2, pwmb);

// PID
const double kp = 0.3;
const double kd = 0.1;
double ep, prevep, ed;
double rawpterm, rawdterm;
int pidterm;

// *** Variables ***
volatile unsigned long edge = 0;
int pwmIncr = 10;

void setup() {
  Serial.begin(9600);
  enc.setTime(micros());
  //attachInterrupt(leftwheel, increment, RISING);
}

void loop()
{
  //pid(); 
  /*
  Serial.println(enc.getSpeed(micros()));
  int sp = (analogRead(7) > 1000) ? 10 : 0;
  umouse.setPWM(sp, sp);
 */
 
  irdistlib();
  //enclib();
  //drivelib();
}

void pid() {
  //Serial.println(enc.getSpeed(micros()));
  if (analogRead(7) > 1000) {
    double lcm = flsense.getCm();
    double rcm = frsense.getCm();
    /*
    Serial.print(lcm);
    Serial.print("\t");
    Serial.print(rcm);
    Serial.print("\t");
    */
    ep = (lcm < 15 && rcm < 15) ? lcm - rcm : 0;
    rawpterm = kp * ep;
    ed = ep - prevep;
    rawdterm = kd * ed;
    pidterm = floor(rawpterm + rawdterm);
    int lpwm = 70 - pidterm;
    int rpwm = 70 + pidterm;
    umouse.setPWM(lpwm, rpwm);
  } else {
    umouse.setPWM(0, 0);
  }
  /*
  Serial.print(umouse.getPWMA());
  Serial.print("\t");
  Serial.println(umouse.getPWMB());
  */
}

void increment() {
  edge++;
}

void irdistlib() {
    // IR
  Serial.print("Distance:\t");
  Serial.print(lsense.getCm());
  Serial.print("\t");
  Serial.print(flsense.getCm()); 
  Serial.print("\t");
  Serial.print(frsense.getCm()); 
  Serial.print("\t");
  Serial.println(rsense.getCm());
}

void irdistvallib() {
    // IR
  Serial.print("Distance:\t");
  Serial.print(lsense.getVal());
  Serial.print("\t");
  Serial.print(flsense.getVal()); 
  Serial.print("\t");
  Serial.print(frsense.getVal()); 
  Serial.print("\t");
  Serial.println(rsense.getVal());
}

void enclib() {
  Serial.println(enc.getSpeed(micros()));
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
