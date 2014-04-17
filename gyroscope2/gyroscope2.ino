#include "Wire.h"
#include "Gyroscope.h"
#include "Driver.h"
#include "Encoder.h"
#include "IRsensor.h"

#include <avr/interrupt.h>
#include <avr/io.h>

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
Gyroscope gyro;

int xOff, yOff, zOff = 0;
long previousTime = 0;
long degreesChanged = 0;
int actualDegreesChanged = 0;
volatile int accumulatedDegrees = 0;

int prevZRate = 0;

#define LOW_FILTER 3
#define UPDATE_GYRO 10 //in milliseconds
#define GYRO_CALIBRATION 2.61
#define SAMPLE_RATE 10
#define GYRO_OFFSET 7
#define LEFT 20
#define RIGHT 5
#define MAXPWM 8
#define MINPWM 0
#define MARGIN_ERROR 30
#define ZERO_MARGIN 1

#define gyroK 1 //2
#define gyroKd 0 //1.4

#define FAST_TURN 0
#define SLOW_STEADY 1
int gyroState = FAST_TURN;

int errorDegree = 0;
int previousDegreeError = 0;
int refDegree = -180;
int pwmLeft = 0;
int pwmRight = 0;

int gyro_offset = 0;
int zRate = 0;
int LEDPIN = 13;

boolean stopEverything = false;

//RateAxz = (AdcGyroXZ * Vref / 1023 - VzeroRate) / Sensitivity Eq.3
//RateAyz = (AdcGyroYZ * Vref / 1023 - VzeroRate) / Sensitivity

void setup() {
  Serial.begin(9600);  
  Serial.println("Set Up");
  
  //gyro.readX();
//  

  Serial.println("End Set Up");
  
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
  
  
  cli();          // disable global interrupts

  //set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 155;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS02) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  sei();
}

ISR(TIMER0_COMPA_vect)
{
  //digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  if (abs(zRate) > LOW_FILTER) {
    degreesChanged += zRate*SAMPLE_RATE; //rate * time in ms * 1 s / 1000 ms 
  }
}

void loop() {
 
      
      long difference = millis() - previousTime;      
      zRate = gyro.readZ() - zOff;
     
     actualDegreesChanged = degreesChanged/1000;
     errorDegree = refDegree - actualDegreesChanged; 
     int diffDegree = errorDegree - previousDegreeError;
     pwmLeft = 5;
     pwmRight = -5;
     
     /*
     if (errorDegree > 0) {
      pwmRight = errorDegree*gyroK + diffDegree*gyroKd;
      if (pwmRight < MINPWM) {
        pwmRight = MINPWM; 
      } else if (pwmRight > MAXPWM) {
        pwmRight = MAXPWM; 
      }
      
      if (errorDegree < MARGIN_ERROR) {
        pwmLeft = 0; 
      } else {
        pwmLeft = MINPWM;    
      } 
      

    } else {
      pwmLeft = (-1 * errorDegree * gyroK) + diffDegree*gyroKd; 
      if (pwmLeft < MINPWM ) {
        pwmLeft = MINPWM;
      } else if (pwmLeft > MAXPWM) {
        pwmLeft = MAXPWM;  
      }
      
      if (abs(errorDegree) < MARGIN_ERROR) {
        pwmRight = 0; 
      } else {
        pwmRight = MINPWM;    
      } 
      
    
    }
    */
    
      //  Serial.println("PWM LEFT: " + String(pwmLeft) + "; PWM RIGHT: " + String(pwmRight));
     if (abs(errorDegree) <= ZERO_MARGIN || stopEverything) {
       umouse.shortbrake();
       stopEverything = true;
     } else if (analogRead(7) > 1000) {
        umouse.stop();   
      } else {
        umouse.setPWM(pwmLeft, pwmRight);
      }
    //Serial.println(accumulatedDegrees*difference)/1000; //rate * time in ms * 1 s / 1000 ms        
     // Serial.println("Degrees Turned:" + String(actualDegreesChanged) + ";degreesChanged " + String(degreesChanged) + "; Rate: " + String(zRate) + "; Diff " + String(difference) );
      previousDegreeError = errorDegree;
      prevZRate = zRate;
      previousTime = millis();
     
   //}
  //Wait 10ms before reading the values again. (Remember, the output rate was set to 100hz and 1reading per 10ms = 100hz.)
}

