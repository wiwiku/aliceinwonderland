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

#define KP_FRONT_NO_SIDES .6 
#define KD_FRONT_NO_SIDES 0

#define KP_FRONT_AND_LEFT .7
#define KD_FRONT_AND_LEFT 0

#define KP_FRONT_AND_RIGHT .7
#define KD_FRONT_AND_RIGHT 0

#define KP_FRONT_WITH_SIDES .5
#define KD_FRONT_WITH_SIDES 0

PID pid(kp, kd);
PID pid_front_no_sides(KP_FRONT_NO_SIDES, KD_FRONT_NO_SIDES);
PID pid_front_with_sides(KP_FRONT_WITH_SIDES, KD_FRONT_WITH_SIDES);
PID pid_front_and_left(KP_FRONT_AND_LEFT, KD_FRONT_AND_LEFT);
PID pid_front_and_right(KP_FRONT_AND_RIGHT, KD_FRONT_AND_RIGHT);


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

#define FRONT_WITH_SIDES 0
#define FRONT_NO_SIDES 1
#define FRONT_AND_LEFT 2
#define FRONT_AND_RIGHT 3
#define BOTH_DIAGONALS 4
#define LEFT_WALL 5
#define RIGHT_WALL 6
#define NO_WALLS 7
int forwardState = NO_WALLS;
int nextState = NO_WALLS;
int prevState = NO_WALLS;

#define SEE_FRONT 20
#define LEFT_FRONT_IDEAL 12
#define RIGHT_FRONT_IDEAL 12
#define RIGHT_SIDE_IDEAL 2  //4,5 
#define RIGHT_DIAGONAL_IDEAL 5 //9

#define LEFT_SIDE_IDEAL 4 //4,5 
#define LEFT_DIAGONAL_IDEAL 9


#define FRONT_RIGHT_STOP 12
#define RESILIENCE 0

#define KP_STRAIGHT 1
int resilience_counter = 0;
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
  //irvallib();
  //enclib();
  //drivelib();
 // umouse.setPWM(30, 30);
   //updateForwardState();
   //Serial.println("State: " + String(forwardState));
    //driveForward(edgePerSq, forwardSpeed);
   driveStraight_Gyro();

  if (!done) {
    switchMode(SLOW);
    //driveForward(edgePerSq, forwardSpeed);
   // driveForward(edgePerSq, forwardSpeed);
   // driveForward(edgePerSq, forwardSpeed);

//    turn(-90);
//    delay(1000);
//    turn(-90);
//    delay(1000);
//    turn(-90);
//    delay(1000);
//
//    turn(-90);

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

void driveStraight() {
    int diff = redge - ledge;
    int pidout = KP_STRAIGHT*(diff);
    int pwmLeft = 20 + pidout; 
    int pwmRight = 20 - pidout;
    
    if (pwmLeft <= 0) {
      pwmLeft = 0;
    } else if (pwmRight <= 0) {
      pwmRight = 0;  
    }
    umouse.setPWM(pwmLeft, pwmRight);
    Serial.println("ledge \t " + String(ledge) + "\t redge \t" + String(redge));
}

void driveStraight_Gyro() {
  int minPWM = 20;
  int maxPWM = 30;
  
  zRate = gyro.readZ() - zOff;
  long difference = millis() - previousTime;      
 
  actualDegreesChanged = degreesChanged/1000;
  errorDegree = 0 - actualDegreesChanged; 
  int dError = (errorDegree - previousDegreeError)/difference;

 if (errorDegree > 0) {
  pwmRight = errorDegree*KP_STRAIGHT+20;
  if (pwmRight < minPWM) {
    pwmRight = minPWM; 
  } else if (pwmRight > maxPWM) {
    pwmRight = maxPWM; 
  }
  
  pwmLeft =  errorDegree*KP_STRAIGHT;
  if (pwmLeft < minPWM) {
    pwmLeft = minPWM; 
  } else if (pwmLeft > maxPWM) {
    pwmLeft = maxPWM; 
  }
 
 } else {
  pwmLeft = (-1*errorDegree*KP_STRAIGHT)+20;
  if (pwmLeft < minPWM) {
    pwmLeft = minPWM; 
  } else if (pwmLeft > maxPWM) {
    pwmLeft = maxPWM; 
  }
  
  pwmRight =  (-1*errorDegree*KP_STRAIGHT); 
 if (pwmRight < minPWM) {
    pwmRight = minPWM; 
  } else if (pwmRight > maxPWM) {
    pwmRight = maxPWM; 
  }  
 }
 
 umouse.setPWM(pwmLeft, pwmRight);
 //umouse.setPWM(20, 20);

 previousTime = millis();
 Serial.println(minPWM);
 Serial.println(maxPWM);
 Serial.println("Degrees Turned:" + String(actualDegreesChanged) + ";pwmLeft" + String(pwmLeft) + ";pwmRight" + String(pwmRight) + "; Rate: " + String(zRate) + "; Diff " + String(difference) );

}
void lincrement() {
  ledge++;
  
  if (forwardState == BOTH_DIAGONALS || forwardState == LEFT_WALL || forwardState == RIGHT_WALL || forwardState == NO_WALLS) { 
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
//  if (moving || turnLeft) {
//    if (redge >= stopEdge) {
//      umouse.brake();
//      moving = false;
//      turnLeft = false;
//      notComplete = false;
//    }
//  }
}

//Update gyro's angle rate. 
ISR(TIMER1_COMPA_vect)
{
  digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  if (abs(zRate) > LOW_FILTER) {
    degreesChanged += zRate*SAMPLE_RATE; //rate * time in ms * 1 s / 1000 ms 
  }
}

boolean leftWall() {
  return (dlsensor.hasWall() && lsensor.hasWall());  
}

boolean rightWall() {
  return (drsensor.hasWall() && rsensor.hasWall());
}

int updateForwardState() {
  //Determine what state you're in first
  double flcm = flsensor.getCm();
  double frcm = frsensor.getCm();
  if (flcm < SEE_FRONT && frcm < SEE_FRONT) {
    
    if (leftWall() && rightWall()) {
      return FRONT_WITH_SIDES;
    } else if (leftWall()) {
      return FRONT_AND_LEFT;   
    } else if ( rightWall() ) {
      return FRONT_AND_RIGHT;   
    } else {
       return FRONT_NO_SIDES;      
    }
  } else if (leftWall() && rightWall()) {
    return BOTH_DIAGONALS;
  } else if (leftWall()) { //diagonal left or most left
    return LEFT_WALL;
  } else if (rightWall())  {
    return RIGHT_WALL;  
  } else {
    return NO_WALLS;  
  }
    
}
// This function will drive the mouse forward for a specified number of edges.
void driveForward(unsigned long deltaEdge, int fspeed) {
  stopEdge = ledge + deltaEdge;
  moving = true;
  //forwardState = FRONT_NO_SIDES;
  forwardState = updateForwardState();
  resilience_counter = 0;
  while (moving) {
      //irdistlib();

    //output pwm
    switch(forwardState) {
      
      case FRONT_NO_SIDES: {
        double lcm = flsensor.getCm();
        double rcm = frsensor.getCm();

        //check to see if break out of this loop
        if (lcm <= LEFT_FRONT_IDEAL || rcm <= RIGHT_FRONT_IDEAL) {
          moving = false;
          break;  
        }
        
        umouse.setPWM(10, 10);
        
//             
//        if (drsensor.hasWall()) {
//          double drcm = drsensor.getCm();
//          int pidout = floor(pid_front_no_sides.getPIDterm(rcm, drcm));
//          umouse.setPWM(fspeed + pidout, fspeed - pidout);
//        } else if (dlsensor.hasWall()) {
//          double dlcm = dlsensor.getCm();
//          int pidout = floor(pid_front_no_sides.getPIDterm(lcm, dlcm));
//          umouse.setPWM(fspeed - pidout, fspeed + pidout);    
//              
//        } else {
//        
//          //if left sees longer distance, spin more with left wheel and vice versa
//          int pidout = floor(pid_front_no_sides.getPIDterm(lcm, rcm));
//          umouse.setPWM(fspeed + pidout, fspeed - pidout);
//        }
//        break; 
      } 
    

     case FRONT_WITH_SIDES: {       
        double front_lcm = flsensor.getCm();
        double front_rcm = frsensor.getCm();
        //check to see if break out of this loop
        if (front_lcm <= LEFT_FRONT_IDEAL || front_rcm <= RIGHT_FRONT_IDEAL) {
          moving = false;
          break;  
        }
       
       //stay in center
       double lcm = dlsensor.getCm();
       double rcm = drsensor.getCm();
       int pidout = floor(pid_front_with_sides.getPIDterm(lcm, rcm));
       umouse.setPWM(fspeed - pidout, fspeed + pidout);
        
       break;
     } 
     
     case FRONT_AND_LEFT: {
       
        double front_rcm = frsensor.getCm();
        //check to see if break out of this loop
        if (front_rcm <= RIGHT_FRONT_IDEAL) {
          moving = false;
          break;  
        }
        //if leftmost sees shorter, then oriented too much right and vice versa if diagonal left
       double lcm = lsensor.getCm();
       double rcm = dlsensor.getCm();
         
       double diff_lcm = lcm - LEFT_SIDE_IDEAL;
       double diff_rcm = rcm - LEFT_DIAGONAL_IDEAL;
   
       int pidout = floor(pid_front_and_left.getPIDterm(diff_lcm, diff_rcm));
       umouse.setPWM(fspeed + pidout, fspeed - pidout);
       break;  
     }
     
     case FRONT_AND_RIGHT: {
        double front_lcm = flsensor.getCm();
        double front_rcm = frsensor.getCm();

        //check to see if break out of this loop
        if (front_lcm <= FRONT_RIGHT_STOP || front_rcm <= RIGHT_FRONT_IDEAL) {
          moving = false;
          break;  
        }
       
       //if diagonal right sees shorter, then oriented too much left and vice versa if rightmost
       double lcm = drsensor.getCm();
       double rcm = rsensor.getCm();
       
       double diff_lcm = lcm - RIGHT_DIAGONAL_IDEAL;
       double diff_rcm = rcm - RIGHT_SIDE_IDEAL;
       
       int pidout = floor(pid_front_and_right.getPIDterm(diff_lcm, diff_rcm));
       umouse.setPWM(fspeed + pidout, fspeed - pidout);
       break;  
     }
      
      case BOTH_DIAGONALS: {
        
        //stay in center
        double lcm = dlsensor.getCm();
        double rcm = drsensor.getCm();
        int pidout = floor(pid.getPIDterm(lcm, rcm));
        umouse.setPWM(fspeed - pidout, fspeed + pidout);
        break;  
      }
      
      case LEFT_WALL: {
        //if leftmost sees shorter, then oriented too much right and vice versa if diagonal left
       double lcm = lsensor.getCm();
       double rcm = dlsensor.getCm();
         
       double diff_lcm = lcm - LEFT_SIDE_IDEAL;
       double diff_rcm = rcm - LEFT_DIAGONAL_IDEAL;
   
       int pidout = floor(pid.getPIDterm(diff_lcm, diff_rcm));
       umouse.setPWM(fspeed + pidout, fspeed - pidout);
       break;  
    
        break;  
      }
      
      case RIGHT_WALL: {
       //if diagonal right sees shorter, then oriented too much left and vice versa if rightmost
       double lcm = drsensor.getCm();
       double rcm = rsensor.getCm();
       
       double diff_lcm = lcm - RIGHT_DIAGONAL_IDEAL;
       double diff_rcm = rcm - RIGHT_SIDE_IDEAL;
       
       int pidout = floor(pid.getPIDterm(diff_lcm, diff_rcm));
       umouse.setPWM(fspeed + pidout, fspeed - pidout);
       break;  
    
        break;  
      }
      
      case NO_WALLS: { //use pid on encoder counts when time allows
        umouse.setPWM(fspeed, fspeed);
        break;       
      }
    }
    
    //updating state
    nextState = updateForwardState();
    if (nextState == prevState) {
      resilience_counter += 1;  
    } else {
      resilience_counter = 0;
    }
    
    if (resilience_counter >= RESILIENCE || nextState == NO_WALLS) {
      forwardState = nextState;  
    }
//    } else if ( (forwardState == LEFT_WALL && !dlsensor.hasWall())  ||
//              && drsensor.hasWall()

    prevState = nextState;
    Serial.println("State: " + String(forwardState));
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
