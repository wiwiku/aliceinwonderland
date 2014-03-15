#include <digitalWriteFast.h>


/*
Sample Code to run the Sparkfun TB6612FNG 1A Dual Motor Driver using Arduino UNO R3
 
 This code conducts a few simple manoeuvres to illustrate the functions:
 - motorDrive(motorNumber, motorDirection, motorSpeed)
 - motorBrake(motorNumber)
 - motorStop(motorNumber)
 - motorsStandby
 
 Connections:
 - Pin 3 ---> PWMA
 - Pin 8 ---> AIN2
 - Pin 9 ---> AIN1
 - Pin 10 ---> STBY
 - Pin 11 ---> BIN1
 - Pin 12 ---> BIN2
 - Pin 5 ---> PWMB
 
 - Motor 1: A01 and A02
 - Motor 2: B01 and B02
 
 Encoder Values: 
 For Motor B, 
 52, 50
 
 For Motor A,
 46, 48
 
 
 */

//define the radius of the wheel
#define RADIUS_WHEEL 5 //5 cm

//Define the Pins

//Motor 1
int pinAIN1 = 9; //Direction
int pinAIN2 = 8; //Direction
int pinPWMA = 3; //Speed

int pinEncoderA1 = 46;
int pinEncoderA2 = 48;

//Motor 2
int pinBIN1 = 11; //Direction
int pinBIN2 = 12; //Direction
int pinPWMB = 5; //Speed

int pinEncoderB1 = 50;
int pinEncoderB2 = 52;

//Standby
int pinSTBY = 10;

//Constants to help remember the parameters
static boolean turnCW = 0;  //for motorDrive function
static boolean turnCCW = 1; //for motorDrive function
static boolean motor1 = 0;  //for motorDrive, motorStop, motorBrake functions
static boolean motor2 = 1;  //for motorDrive, motorStop, motorBrake functions

int lastSendCommand = 0;
int encoderA1 = 0;
int encoderA2 = 0;
int encoderB1 = 0;
int encoderB2 = 0;

int prev_encoderA1 = 0;
int prev_encoderA2 = 0;
int prev_encoderB1 = 0;
int prev_encoderB2 = 0;

int rotate_encoderA1 = 0;
int rotate_encoderA2 = 0;
int rotate_encoderB1 = 0;
int rotate_encoderB2 = 0;

int time_encoderA1 = 0;
int time_encoderA2 = 0;
int time_encoderB1 = 0;
int time_encoderB2 = 0;

int speed_encoderA1 = 0;
int speed_encoderA2 = 0;
int speed_encoderB1 = 0;
int speed_encoderB2 = 0;

void setup()
{
  //Set the PIN Modes
  pinMode(pinPWMA, OUTPUT);
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinAIN2, OUTPUT);

  pinMode(pinPWMB, OUTPUT);
  pinMode(pinBIN1, OUTPUT);
  pinMode(pinBIN2, OUTPUT);

  pinMode(pinSTBY, OUTPUT);
  
  pinMode(pinEncoderA1, INPUT);
  pinMode(pinEncoderA2, INPUT);
  pinMode(pinEncoderB1, INPUT);
  pinMode(pinEncoderB2, INPUT);
  
  digitalWrite(pinSTBY, HIGH);

  Serial.begin(9600);

}

void loop()
{
  
    if (Serial.available()) {
     char character_received = Serial.read();
     if ( character_received == 'w') { //go straight
       motorDrive(motor1, turnCW, 255);
       motorDrive(motor2, turnCW, 255);
     
     } else if (character_received == 'a') { //go left
       motorDrive(motor1, turnCCW, 255);
       motorDrive(motor2, turnCW, 255);
     
     
     } else if (character_received == 's') { //go right
       motorDrive(motor1, turnCCW, 255);
       motorDrive(motor2, turnCCW, 255);
     
     
     
     } else if (character_received == 'd') { // go back
       motorDrive(motor1, turnCW, 255);
       motorDrive(motor2, turnCCW, 255);
     
     
     } else if (character_received == 'b') {
       motorsStandby();  
     
     } else if (character_received == 'c') {
      motorBrake(motor1);
      motorBrake(motor2);  
    }
   }
   
   //motorDrive(motor1, turnCW, 150);
   //motorDrive(motor2, turnCW, 150);
    
  // handleEncoder();
}

void handleEncoder() {
  encoderA1 = (int) digitalRead(pinEncoderA1);
  encoderA2 = (int) digitalRead(pinEncoderA2);
  encoderB1 = (int) digitalRead(pinEncoderB1);
  encoderB2 = (int) digitalRead(pinEncoderB2);
  
  Serial.println("EncoderA1:" + String(encoderA1));
  Serial.println("EncoderA2:" + String(encoderA2));
  Serial.println("EncoderB1:" + String(encoderB1));
  Serial.println("EncoderB2:" + String(encoderB2));
  Serial.println();
  
  
  if (prev_encoderA1 == 0 && encoderA1) { //transition from low to high
    rotate_encoderA1 += 1; 
    if (rotate_encoderA1 == 2) { //full rotation
      rotate_encoderA1 = 0;
      speed_encoderA1 = 2*3.14*RADIUS_WHEEL / (millis() - time_encoderA1);
      time_encoderA1 = millis();
    }  
  }
  
  if (prev_encoderA2 == 0 && encoderA2) { //transition from low to high
    rotate_encoderA2 += 1; 
    if (rotate_encoderA2 == 2) { //full rotation
      rotate_encoderA2 = 0;
      speed_encoderA2 = 2*3.14*RADIUS_WHEEL / (millis() - time_encoderA2);
      time_encoderA2 = millis();
    }  
  }
  
  if (prev_encoderB1 == 0 && encoderB1) { //transition from low to high
    rotate_encoderB1 += 1; 
    if (rotate_encoderB1 == 2) { //full rotation
      rotate_encoderB1 = 0;
      speed_encoderB1 = 2*3.14*RADIUS_WHEEL / (millis() - time_encoderB1);
      time_encoderB1 = millis();
    }  
  }
  
  if (prev_encoderB2 == 0 && encoderB2) { //transition from low to high
    rotate_encoderB2 += 1; 
    if (rotate_encoderB2 == 2) { //full rotation
      rotate_encoderB2 = 0;
      speed_encoderB2 = 2*3.14*RADIUS_WHEEL / (millis() - time_encoderB2);
      time_encoderB2 = millis();
    }  
  }
  
  prev_encoderA1 = encoderA1;
  prev_encoderA2 = encoderA2;
  prev_encoderB1 = encoderB1;
  prev_encoderB1 = encoderB2;
 
}

void motorDrive(boolean motorNumber, boolean motorDirection, int motorSpeed)
{
  /*
  This Drives a specified motor, in a specific direction, at a specified speed:
   - motorNumber: motor1 or motor2 ---> Motor 1 or Motor 2
   - motorDirection: turnCW or turnCCW ---> clockwise or counter-clockwise
   - motorSpeed: 0 to 255 ---> 0 = stop / 255 = fast
   */

  boolean pinIn1;  //Relates to AIN1 or BIN1 (depending on the motor number specified)


  //Specify the Direction to turn the motor
  //Clockwise: AIN1/BIN1 = HIGH and AIN2/BIN2 = LOW
  //Counter-Clockwise: AIN1/BIN1 = LOW and AIN2/BIN2 = HIGH
  if (motorDirection == turnCW)
    pinIn1 = HIGH;
  else
    pinIn1 = LOW;

  //Select the motor to turn, and set the direction and the speed
  if(motorNumber == motor1)
  {
    //Serial.println("Motor1:");
    digitalWrite(pinAIN1, pinIn1);
    digitalWrite(pinAIN2, !pinIn1);  //This is the opposite of the AIN1
    analogWrite(pinPWMA, motorSpeed);
  }
  else
  {
    //Serial.println("Motor2:");
    digitalWrite(pinBIN1, pinIn1);
    digitalWrite(pinBIN2, !pinIn1);  //This is the opposite of the BIN1
    analogWrite(pinPWMB, motorSpeed);
  }



  //Finally , make sure STBY is disabled - pull it HIGH
  digitalWrite(pinSTBY, HIGH);
  lastSendCommand = millis();
}

void motorBrake(boolean motorNumber)
{
  /*
This "Short Brake"s the specified motor, by setting speed to zero
   */

  if (motorNumber == motor1)
    analogWrite(pinPWMA, 0);
  else
    analogWrite(pinPWMB, 0);

}


void motorStop(boolean motorNumber)
{
  /*
  This stops the specified motor by setting both IN pins to LOW
   */
  if (motorNumber == motor1) {
    digitalWrite(pinAIN1, LOW);
    digitalWrite(pinAIN2, LOW);
  }
  else
  {
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, LOW);
  } 
}


void motorsStandby()
{
  /*
  This puts the motors into Standby Mode
   */
  digitalWrite(pinSTBY, LOW);
}



