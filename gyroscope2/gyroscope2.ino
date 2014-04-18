#include "Wire.h"
#include "Gyroscope.h"
#include "Driver.h"
#include "Encoder.h"
#include "IRsensor.h"
#include "PID.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "Top.h"
#include "Gyro.h"

IRsensor lsensor(left, lc1, lc2, irThCm);
IRsensor dlsensor(diagleft, dlc1, dlc2, irThCm);
IRsensor flsensor(frontleft, flc1, flc2, irThCm);
IRsensor frsensor(frontright, frc1, frc2, irThCm);
IRsensor drsensor(diagright, drc1, drc2, irThCm);
IRsensor rsensor(right, rc1, rc2, irThCm);

Encoder lenc(dwheelmm);

Driver umouse(ain1, ain2, pwma, bin1, bin2, pwmb);
PID pid(kp, kd);

Gyroscope gyro;

#define LOW_FILTER 3
#define UPDATE_GYRO 10 //in milliseconds
#define GYRO_CALIBRATION 2.61
#define SAMPLE_RATE 10
#define LEFT 20
#define RIGHT 15
#define SLOWMAX 30
#define FASTMAX 100
#define SLOWMIN 5
#define FASTMIN 5

#define MARGIN_ERROR 0
#define ZERO_MARGIN 1

#define GYROK_SLOW .2
#define GYROKD_SLOW .1
#define GYROK_FAST 

float gyroK = .2; //2
float gyroKd = .1; //1.4

#define FAST_TURN 0
#define SLOW_STEADY 1
int gyroState = FAST_TURN;

#define SLOW 0
#define FAST 1
int turnMode = FAST;
int maxPWM = 0;
int minPWM = 0;
int gyroOffset = 0;
int turnRatio = 0;

int xOff, yOff, zOff = 0;
long previousTime = 0;
long degreesChanged = 0;
int actualDegreesChanged = 0;
volatile int accumulatedDegrees = 0;
int prevZRate = 0;


int errorDegree = 0;
int previousDegreeError = 0;
int refDegree = 90;
int pwmLeft = 0;
int pwmRight = 0;


int gyro_offset = 0;
int zRate = 0;
int LEDPIN = 13;
int SWITCH = 10;

boolean stopEverything = false;

//RateAxz = (AdcGyroXZ * Vref / 1023 - VzeroRate) / Sensitivity Eq.3
//RateAyz = (AdcGyroYZ * Vref / 1023 - VzeroRate) / Sensitivity

void setup() {
  Serial.begin(9600);  
  
  //Configure the gyroscope
  //Set the gyroscope scale for the outputs to +/-2000 degrees per secon
  gyro.configureDegree((DLPF_FS_SEL_0|DLPF_FS_SEL_1|DLPF_CFG_0));
  gyro.configureSampleRate(9);

  zOff = gyro.readZ();
  umouse.setPWM(pwmLeft, pwmRight);
  if (LEFT > RIGHT) {
    gyro_offset = LEFT/RIGHT;  
  } else {
    gyro_offset = RIGHT/LEFT;  
  }
  
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

ISR(TIMER1_COMPA_vect)
{
  //digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  if (abs(zRate) > LOW_FILTER) {
    degreesChanged += zRate*SAMPLE_RATE; //rate * time in ms * 1 s / 1000 ms 
  }
}

void loop() {
  if (!stopEverything) {
    turn(-90);
    //turn(90);
    stopEverything = true;
  }    
   //}
  //Wait 10ms before reading the values again. (Remember, the output rate was set to 100hz and 1reading per 10ms = 100hz.)
}

void switchMode(int mode) {
  turnMode = mode;  
}


void turn(int ref) {
  if (turnMode == SLOW) {
    gyroK = 0.2;
    gyroKd = 0.1;
    minPWM = SLOWMIN;
    maxPWM = SLOWMAX;  
    gyroOffset = 4;
    turnRatio = 4;
  } else if (turnMode == FAST) {
    gyroK =  0.8;
    gyroKd = 0.5;
  
    minPWM = FASTMIN;
    maxPWM = FASTMAX;
    gyroOffset = 16;
    turnRatio = 8;
    
  }
  
  int referenceDegree = 0;
  if (ref > 0) {
    referenceDegree = ref - gyroOffset; 
  } else {
    referenceDegree = ref + gyroOffset;       
  }
  boolean notComplete = true;
  degreesChanged = 0;
  while(notComplete) {
      long difference = millis() - previousTime;      
      zRate = gyro.readZ() - zOff;
     
      actualDegreesChanged = degreesChanged/1000;
      errorDegree = referenceDegree - actualDegreesChanged; 
      int dError = (errorDegree - previousDegreeError)/difference;

     if (errorDegree > 0) {
      pwmRight = errorDegree*gyroK + dError*gyroKd;
      if (pwmRight < minPWM) {
        pwmRight = minPWM; 
      } else if (pwmRight > maxPWM) {
        pwmRight = maxPWM; 
      }      
      pwmLeft = pwmRight/turnRatio;
    } else {
      pwmLeft = (-1 * errorDegree * gyroK) + dError*gyroKd; 
      if (pwmLeft < minPWM ) {
        pwmLeft = minPWM;
      } else if (pwmLeft > maxPWM) {
        pwmLeft = maxPWM;  
      }
      
       pwmRight = pwmLeft/turnRatio;
    
    }
  
    if (abs(errorDegree) <= ZERO_MARGIN) {
         umouse.brake();
         notComplete = false;
       } else if (digitalRead(SWITCH) == HIGH) {
            umouse.stop();
        }  else {
         umouse.setPWM(pwmLeft, pwmRight);  
       }           
    
//    //Serial.println(accumulatedDegrees*difference)/1000; //rate * time in ms * 1 s / 1000 ms        
      //Serial.println("Degrees Turned:" + String(actualDegreesChanged) + ";degreesChanged " + String(degreesChanged) + "; Rate: " + String(zRate) + "; Diff " + String(difference) );
      previousDegreeError = errorDegree;
      prevZRate = zRate;
      previousTime = millis();
     
  }
  
}

