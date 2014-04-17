#include "Wire.h"
#include "Gyroscope.h"
#include "Driver.h"
#include "Encoder.h"
#include "IRsensor.h"

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
int accumulatedDegrees = 0;

int prevZRate = 0;

#define LOW_FILTER 5
#define UPDATE_GYRO 10 //in milliseconds
#define GYRO_CALIBRATION 2.61
#define SAMPLE_RATE 1
#define GYRO_OFFSET 50
#define LEFT 20
#define RIGHT 5

int gyro_offset = 0;

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
  umouse.setPWM(LEFT, RIGHT);
  if (LEFT > RIGHT) {
    gyro_offset = LEFT/RIGHT;  
  } else {
    gyro_offset = RIGHT/LEFT;  
  }
}

void loop() {
 
  //Create variables to hold the output rates.
  int xRate, yRate, zRate;
  //Read the x,y and z output rates from the gyroscope.
  zRate = gyro.readZ() - zOff;
  //Print the output rates to the terminal, seperated by a TAB character.
//  Serial.print(xRate);
//  Serial.print('\t');
//  Serial.print(yRate);
//  Serial.print('\t');
//  Serial.println(zRate);  
  
  if (analogRead(7) > 1000) {
    umouse.stop();   
  } else {
    if (abs(actualDegreesChanged) > 90) {
      umouse.stop();
    } 
  }
  
  long difference = millis() - previousTime;      
  if (difference > UPDATE_GYRO) { // 10 millisecond passed
    if (abs(zRate) > LOW_FILTER) {
      degreesChanged += (zRate*SAMPLE_RATE); //rate * time in ms * 1 s / 1000 ms 
    }
    
    
     if (abs(prevZRate) > LOW_FILTER) {   
       
       int numSamples = (difference/SAMPLE_RATE)-2; //subtract top and bottom value
       
       if (numSamples < 0) {
         numSamples = 0;  
       } 
       
       //Serial.println(numSamples);
       if (numSamples != 0) {
         int errorZRate = (zRate - prevZRate)/numSamples;
         //Serial.println(errorZRate);

         for (int i = 1; i <= numSamples; i++) { 
            //degreesChanged += ((zRate)*SAMPLE_RATE);
            degreesChanged += ((prevZRate + i*errorZRate)*SAMPLE_RATE);
         }
       }
     }
//     
     actualDegreesChanged = degreesChanged/1000;
    //Serial.println(accumulatedDegrees*difference)/1000; //rate * time in ms * 1 s / 1000 ms        
      Serial.println("Degrees Turned:" + String(actualDegreesChanged) + ";degreesChanged " + String(degreesChanged) + "; Rate: " + String(zRate) + "; Diff " + String(difference) );
      prevZRate = zRate;
      previousTime = millis();
      
   }

 
  //Wait 10ms before reading the values again. (Remember, the output rate was set to 100hz and 1reading per 10ms = 100hz.)
}
