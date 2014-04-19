#include "IRsensor.h"
#include "Driver.h"
#include "Encoder.h"
#include "PID.h"
#include "Gyroscope.h"
#include "Gyro.h"
#include <Wire.h>
#include "Top.h"

/* */

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
int arrlen = -1;
int arrlimit = 10;
unsigned long startedge = 0;

boolean done = false;
volatile boolean moving = false;
volatile boolean turnLeft = false;
volatile boolean turnRight = false;
volatile boolean notComplete = false;

volatile unsigned long stopEdge = 0;

int pwmIncr = 10;

void setup() {
  Serial.begin(9600);
  attachInterrupt(leftwheel, lincrement, RISING);
  attachInterrupt(rightwheel, rincrement, RISING);
  
  //Configure the gyroscope
  //Set the gyroscope scale for the outputs to +/-2000 degrees per secon
  gyro.configureDegree((DLPF_FS_SEL_0|DLPF_FS_SEL_1|DLPF_CFG_0));
  gyro.configureSampleRate(9);

  zOff = gyro.readZ();
  umouse.setPWM(pwmLeft, pwmRight);

  
    pinMode(SWITCH, INPUT);
    
  // initialize Timer1
  cli();          // disable global interrupts
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
 
  // set compare match register to desired timer count:
  OCR1A = 157;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  sei();          // enable global interrupts
}

void enableTimerInterrupt() {
 // initialize Timer1
  cli();          // disable global interrupts  
  TIMSK1 |= (1 << OCIE1A);
  sei();          // enable global interrupts
}

void disableTimerInterrupt() {
  cli();          // disable global interrupts  
  TIMSK1 |= (0 << OCIE1A);
  sei();          // enable global interrupts
  
}

void loop()
{
  //int cm = 90;
  //driveForward(cm * edgePerCm);
  //pidlib();
  //irdistlib();
  //enclib();
  //drivelib();
  //umouse.setPWM(10, 10);
  if (!done) {
    switchMode(SLOW);
//    driveForward(edgePerSq);
//    driveForward(edgePerSq, forwardSpeed);
//    driveForward(edgePerSq, forwardSpeed);
    turn(-90);
    delay(1000);
    turn(-90);
    delay(1000);
    turn(-90);
    delay(1000);

    turn(-90);

//    driveForward(edgePerSq, forwardSpeed);
////    driveForward(edgePerSq);
//    turn(-90);
//    driveForward(edgePerSq, forwardSpeed);
////    driveForward(edgePerSq);
//    turn(-90);
////    driveForward(edgePerSq);
////    turn(-90);
    done = true;
  }
//     driveForward(edgePerSq, forwardSpeed);
////    driveForward(edgePerSq);
//    turn(-90);
////    driveForward(edgePerSq);
////    driveForward(edgePerSq);
////    driveForward(edgePerSq);
//    driveForward(edgePerSq, forwardSpeed);
//    turn(-90);
////    driveForward(edgePerSq);
//    driveForward(edgePerSq, forwardSpeed);
//    turn(-90);
    //driveForward(10*edgePerCm);
//    done = true;
//  }

//  Serial.print(ledge);
//  Serial.print("\t");
//  Serial.println(redge);
}

void lincrement() {
  ledge++;
  if (moving || turnRight) {
    if (ledge >= stopEdge) {
      umouse.brake();
      moving = false;
      turnRight = false;
      notComplete = false;

    }
  } 
}

void rincrement() {
  redge++;
  if (moving || turnLeft) {
    if (redge >= stopEdge) {
      umouse.brake();
      moving = false;
      turnLeft = false;
      notComplete = false;
    }
  }
}

//Update gyro's angle rate. 
ISR(TIMER1_COMPA_vect)
{
  digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  if (abs(zRate) > LOW_FILTER) {
    degreesChanged += zRate*SAMPLE_RATE; //rate * time in ms * 1 s / 1000 ms 
  }
}

// This function will drive the mouse forward for a specified number of edges.
void driveForward(unsigned long deltaEdge, int fspeed) {
  stopEdge = ledge + deltaEdge;
  moving = true;
  while (moving) {
    if (flsensor.hasWall() && frsensor.hasWall()) {
      double lcm = flsensor.getCm();
      double rcm = frsensor.getCm();
      double diff = lcm - rcm;
      int pidout = floor(diff*kpForward);
      umouse.setPWM(fspeed - pidout, fspeed + pidout);
    } else if (dlsensor.hasWall() && drsensor.hasWall()) { // do pid
      double lcm = dlsensor.getCm();
      double rcm = drsensor.getCm();
      int pidout = floor(pid.getPIDterm(lcm, rcm));
      umouse.setPWM(fspeed - pidout, fspeed + pidout);
    } 
      else {
      umouse.setPWM(fspeed, fspeed);
    //}
  }
  umouse.brake();
  Serial.print(deltaEdge);
  Serial.print("\t");
  Serial.print(ledge);
  Serial.print("\t");
  Serial.println(stopEdge);
}

// This function will drive the mouse backward for a specified number of edges.
void driveBackward(unsigned long deltaEdge, int bSpeed) {
  int timeout = 30;
  unsigned long prevedge = ledge;
  stopEdge = ledge + deltaEdge;
  moving = true;
  while (moving && timeout > 0) {
    Serial.println(timeout);
    Serial.print("edge   ");
    Serial.println(ledge);
    if (ledge == prevedge) {
      timeout--;
      
    } else {
      timeout = 30;
    }
    if (lsensor.hasWall() && rsensor.hasWall()) { // do pid
      double lcm = dlsensor.getCm();
      double rcm = drsensor.getCm();
      int pidout = floor(pid.getPIDterm(lcm, rcm));
      umouse.setPWM(bSpeed - pidout, bSpeed + pidout);
    } else {
      umouse.setPWM(bSpeed, bSpeed);
    }
  }
  if (timeout <= 0) {
    Serial.println("timeout");
  }
  umouse.brake();
  Serial.print(deltaEdge);
  Serial.print("\t");
  Serial.print(ledge);
  Serial.print("\t");
  Serial.println(stopEdge);
}

void pidlib() {

  int basespeed = 10;
  if (dlsensor.hasWall() && drsensor.hasWall()) {
    double lcm = dlsensor.getCm();
    double rcm = drsensor.getCm();
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
  if (arrlen == -1) {
    startedge = ledge;
    arrlen = 0;
  }
  if (arrlen < arrlimit) {
    if (ledge - startedge > 50) {
      arr[arrlen] = lenc.getSpeed(micros());
      arrlen++;
      startedge = ledge;
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
    if (input == 'x') {
      umouse.stop();
    } else if (input == ' ') {
      umouse.brake();
    } else {
      if (input == 'w') {
        umouse.setPWM(umouse.getPWMA() + pwmIncr, umouse.getPWMB() + pwmIncr);
        arrlen = -1;
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

void switchMode(int mode) {
  turnMode = mode;  
}


void turn(int ref) {
/*
  
  driveForward(11);

  if (turnMode == SLOW || turnMode == UTURN) {
*/
  if (turnMode == SLOW || turnMode == UTURN) {
    gyroK = GYROK_SLOW;
    gyroKd = GYROKD_SLOW;
    minPWM = MIN_SLOW;
    maxPWM = MAX_SLOW;  
    gyroOffset = OFF_SLOW;
    turnRatio = RATIO_SLOW;
    if (turnMode == UTURN) {
      maxPWM = MAX_UTURN;
      gyroOffset = OFF_UTURN;
      turnRatio = RATIO_UTURN;  
      
    }
  } else if (turnMode == FAST) {
    gyroK =  GYROK_FAST;
    gyroKd = GYROKD_FAST;
  
    minPWM = MIN_FAST;
    maxPWM = MAX_FAST;
    gyroOffset = OFF_FAST; //16
    turnRatio = RATIO_FAST; //8
    
  }
  
  
 
  
  enableTimerInterrupt();
  degreesChanged = 0;
  if (turnMode != UTURN) {
    driveForward(FORWARD_BEGINNING, forwardSpeed);
  }
  
  int referenceDegree = 0;
  int diffEdge = 45;
  notComplete = true;
  if (ref > 0) {
    referenceDegree = ref - gyroOffset;
    turnLeft = true;
    stopEdge = redge + diffEdge; 
    umouse.setPWM(0, minPWM);

  } else {
    turnRight = true;
    referenceDegree = ref + gyroOffset;       
    stopEdge = ledge + diffEdge; 
    umouse.setPWM(minPWM, 0);
  }
  
  while(turnLeft || turnRight) {
    
//      zRate = gyro.readZ() - zOff;
//      long difference = millis() - previousTime;      
//   
//      actualDegreesChanged = degreesChanged/1000;
//      errorDegree = referenceDegree - actualDegreesChanged; 
//      int dError = (errorDegree - previousDegreeError)/difference;
//
//     if (errorDegree > 0) {
//      pwmRight = errorDegree*gyroK + dError*gyroKd;
//      if (pwmRight < minPWM) {
//        pwmRight = minPWM; 
//      } else if (pwmRight > maxPWM) {
//        pwmRight = maxPWM; 
//      }    
//      
//       
//      
//      if (turnMode == UTURN) {
//        pwmLeft = pwmRight/turnRatio;
//      } else {
//        pwmLeft = 0;
//      }
//
//      //pwmLeft = pwmRight/turnRatio;
//      //   end
//    } else {
//      pwmLeft = (-1 * errorDegree * gyroK) + dError*gyroKd; 
//      if (pwmLeft < minPWM ) {
//        pwmLeft = minPWM;
//      } else if (pwmLeft > maxPWM) {
//        pwmLeft = maxPWM;  
//      }
//      
//      if (turnMode == UTURN) {
//      
//         pwmRight = pwmLeft/turnRatio;
//      } else {
//        pwmRight = 0;
//      }
//
//       //pwmRight = pwmLeft/turnRatio;
//    
//// end
//    }
//    
//       if (digitalRead(SWITCH) == HIGH) {
//            umouse.stop();
//        }  else {
//         umouse.setPWM(pwmLeft, pwmRight);  
//       }   
    
      
//    if (abs(errorDegree) <= ZERO_MARGIN) {
//         umouse.brake();
//         notComplete = false;
//       } else if (digitalRead(SWITCH) == HIGH) {
//            umouse.stop();
//        }  else {
//         umouse.setPWM(pwmLeft, pwmRight);  
//       }           
//  
//    if (abs(errorDegree) <= ZERO_MARGIN) {
//         umouse.brake();
//         notComplete = false;
//       } else if (digitalRead(SWITCH) == HIGH) {
//            umouse.stop();
//        }  else {
//         umouse.setPWM(pwmLeft, pwmRight);  
//       }   
//  
    
    
//      if (turnLeft && ledge  >= end_leftEdge) {
//         umouse.brake();
//         notComplete = false;
//       } else if (!turnLeft && redge  >= end_rightEdge) {
//         umouse.brake();
//         notComplete = false;   
//       } else if (digitalRead(SWITCH) == HIGH) {
//            umouse.stop();
//        }  else {
//         umouse.setPWM(pwmLeft, pwmRight);  
//       }      
    
//    //Serial.println(accumulatedDegrees*difference)/1000; //rate * time in ms * 1 s / 1000 ms 
      //Serial.println(maxPWM);
      //Serial.println("Degrees Turned:" + String(actualDegreesChanged) + ";pwmLeft" + String(pwmLeft) + ";pwmRight" + String(pwmRight) + "; Rate: " + String(zRate) + "; Diff " + String(difference) );
//      previousDegreeError = errorDegree;
//      prevZRate = zRate;
//      previousTime = millis();
//     Serial.println("ledge \t" + String(ledge) + "\t redge \t" + String(redge) + "\t stopEdge \t" + String(stopEdge)); // + "end_rightEdge \t" + String(end_rightEdge));  
//     Serial.println(turnLeft);
//     Serial.println(turnRight);
//    
     
  }
 if (turnMode != UTURN) {
    //
   driveForward(FORWARD_END, forwardSpeed);
  }
  umouse.brake();
   disableTimerInterrupt();
}
