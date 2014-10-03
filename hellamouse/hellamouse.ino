#include <Wire.h>

#include <QueueList.h>
#include "Top.h"
#include <IRsensor.h>
#include <QueueList.h>
#include <EEPROM.h>
#include "core.h"
#include "PID.h"
#include "floodfill.h"
#include "Encoder.h"
#include "Driver.h"
#include "IRsensor.h"
#include "Gyro.h"
#include "Gyroscope.h"

Gyroscope gyro;

int switchPin = 10;

boolean isRunning;

/* Maze, mouse position, and wall values */
int x = 0;
int y = 0;
int curDir;
int walls = 0;
int newWalls = 0;
int nextRow, nextCol, nextVal;
int dir, row, col, val, curRun, sw; 
boolean hasWall, returnState;

/* Components */
IRsensor lsensor(left, lc1, lc2, irThSide);
IRsensor dlsensor(diagleft, dlc1, dlc2, irThDiag);
IRsensor flsensor(frontleft, flc1, flc2, irThFront);
IRsensor frsensor(frontright, frc1, frc2, irThFront);
IRsensor drsensor(diagright, drc1, drc2, irThDiag);
IRsensor rsensor(right, rc1, rc2, irThSide);

Encoder lenc(dwheelmm);

Driver umouse(ain1, ain2, pwma, bin1, bin2, pwmb);

PID pid(kp, kd);

volatile boolean moving = false;

volatile unsigned long stopEdge = 0; 

void calc(int x, int y, boolean returnState, int initSides) {
  while(!qEmpty()) {
    Serial.println(qCount());
    int i = qPop() & 255;//double-check
    int row, col;
    iToRowCol( row, col, i );

    // Get smallest neighbor
    byte small = -1; //getFFScore(row, col); //there should be some neighbor smaller than it
    if (!returnState && (row == 7 || row == 8) && (col == 7 || col == 8)) {
      small = getFFScore(row, col);
      small--;
    } 
    else if (returnState && (row == 0) && (col == 0)) {
      small = getFFScore(row, col);
      small--;
    }  
    else {
      if(!wallExists(row, col, EAST)) {
        if (small == -1 || getFFScore(row+1, col) < small) {
          small = getFFScore(row+1, col);
        }
      }

      if(!wallExists(row, col, NORTH)) {
        if (small == -1 || getFFScore(row, col+1) < small) {
          small = getFFScore(row, col+1);
        }
      }

      if(!wallExists(row, col, WEST)) {
        if (small == -1 || getFFScore(row-1, col) < small) {
          small = getFFScore(row-1, col);
        }
      }

      if(!wallExists(row, col, SOUTH)) {
        if (small == -1 || getFFScore(row, col-1) < small) {
          small = getFFScore(row, col-1);
        }
      }
    }
    Serial.println("One");

    small++; //value that it should be
    int curVal = getFFScore(row, col); //value that it is

    if(curVal != UNDEFINED && curVal == small) { 
      continue; 
    }

    // Floodfill this cell  
    setFFScore(row, col, small);

    // Check the cell to the north
    if(!wallExists(row, col, NORTH)) {
      qPush( rowColToI(row, col+1));
    }

    // Check the cel to the east
    if(!wallExists(row, col, EAST)) {
      qPush( rowColToI(row+1, col));
    }

    // Check the cell to the south
    if(!wallExists(row, col, SOUTH)) {
      qPush( rowColToI(row, col-1));
    }

    // Check the cell to the west
    if(!wallExists(row, col, WEST)) {
      qPush( rowColToI(row-1, col));
    }
  }
}

void printMazeInfo(int x, int y) {
  char vertWall = '|';
  char horiWall = '--';
  char noWall = '  ';
  int thisVal, thisWall;
  Serial.println("The maze is currently....");
  for (int mazei = LENGTH-1; mazei >= 0; mazei--) {
    for (int i = 0; i < 3; i++) {
      for (int mazej = 0; mazej < LENGTH; mazej++) {
        thisWall = getWalls(mazej, mazei);
        if (i == 0) { //line 1 (north walls)
          if (mazej == 0) { 
            Serial.print("       "); 
          }
          Serial.print(" ");
          if ((thisWall & 1) == 1) {
            Serial.print("--");
          } 
          else {
            Serial.print("  ");
          }
          Serial.print(" ");
        } 
        else if (i == 1){  //line 2 (west, value, east)
          if (mazej == 0) { 
            if (mazei < 10){ 
              Serial.print(" "); 
            }
            Serial.print(mazei);
            Serial.print("     "); 
          }
          if ((thisWall & 8) == 8) {
            Serial.print("|");
          } 
          else {
            Serial.print(" ");
          }
          if (mazej == x && mazei == y) {
            Serial.print("XX");
          } 
          else {
            thisVal = getFFScore(mazej, mazei);
            if (thisVal < 10) {
              Serial.print(" ");
            }
            Serial.print(thisVal);
          }
          if ((thisWall & 2) == 2) {
            Serial.print("|");
          } 
          else {
            Serial.print(" ");
          }
        } 
        else if (i == 2) {  //line 3 (south walls)
          if (mazej == 0) { 
            Serial.print("       "); 
          }
          Serial.print(" ");
          if ((thisWall & 4) == 4) {
            Serial.print("--");
          } 
          else {
            Serial.print("  ");
          }
          Serial.print(" ");
        }
      }
      Serial.println("");
    }
  }
  Serial.print("       ");
  for (int i = 0; i < LENGTH; i++) {
    Serial.print(" ");
    Serial.print(i);
    if (i < 10) {
      Serial.print(" ");
    }
    Serial.print(" ");
  }
  Serial.println("");
}

/* SENSORS */
/* Return sum of wall values */
int senseWalls(int dir) {
  int walls = 0;
  if (lsensor.hasWall()) {
    if (dir == 1) { 
      walls += SOUTH; 
    }
    else { 
      walls += (dir >> 1); 
    }
  }

  if (rsensor.hasWall()) {
    if (dir == 8) { 
      walls += 1; 
    }
    else { 
      walls += (dir << 1); 
    }
  }

  if (flsensor.hasWall() && frsensor.hasWall()) {
    walls += 1;
  }
  return walls;
}

void lincrement() {
  ledge++;
  if (moving) {
    if (ledge >= stopEdge) {
      umouse.brake();
      moving = false;
    }
  }
}

void rincrement() {
  redge++;
}

/* DRIVING */
void turn(int ref) {
  if (turnMode == SLOW) {
    gyroK = GYROK_SLOW;
    gyroKd = GYROKD_SLOW;
    minPWM = MIN_SLOW; //SLOWMIN;
    maxPWM = MAX_SLOW; //SLOWMAX;  
    gyroOffset = OFF_SLOW;
    turnRatio = RATIO_SLOW;
  } else if (turnMode == FAST) {
    gyroK =  GYROK_FAST;
    gyroKd = GYROKD_FAST;
  
    minPWM = MIN_FAST; //FASTMIN;
    maxPWM = MAX_FAST; //FASTMAX;
    gyroOffset = OFF_FAST; //16
    turnRatio = RATIO_FAST; //8
    
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

// This function will drive the mouse forward for a specified number of edges.
void driveForward(unsigned long deltaEdge, int fspeed) {
  stopEdge = ledge + deltaEdge;
  moving = true;
  while (moving) {
    if (lsensor.hasWall() && rsensor.hasWall()) { // do pid
      double lcm = dlsensor.getCm();
      double rcm = drsensor.getCm();
      int pidout = floor(pid.getPIDterm(lcm, rcm));
      if (pidout > 0) {
        umouse.setPWM(fspeed, fspeed + pidout);
      } 
      else {
        umouse.setPWM(fspeed - pidout, fspeed);
      }
    } 
    else {
      umouse.setPWM(fspeed, fspeed);
    }
  }
  umouse.brake();
}

/* Where all the moving happens! */
int moveFromTo(int row, int col, int dir, int nextRow, int nextCol){
  int nextDir = row == nextRow? (col > nextCol ? SOUTH:NORTH) : 
  (row > nextRow ? WEST:EAST);

  if (dir == nextDir) {
    driveForward(1*edgePerSq, 20); 
    Serial.println("Moving forward");
  } 
  else if (((dir == 1) && (nextDir == 8)) || (dir >> 1 == nextDir)) {
    turn(90);
    Serial.println("Moving left");
  } 
  else if (((dir == 8) && (nextDir == 1)) || ((dir << 1) == nextDir)) {
    turn(-90);
    Serial.println("Turning right");
  } 
  else if ((dir + nextDir) == 5 || (dir + nextDir) == 10) {
    turn(180);
    Serial.println("Turn around");
  }

  return nextDir;
}

/* Main */
void setup() {
  Serial.begin(9600);
  returnState = false;
  curDir = NORTH;
  isRunning = true;

  pinMode(switchPin, OUTPUT);

  initializeThings();
}

void initializeThings() {
  //If we shouldn't read from mem, mem should be cleared outside of this program
  curRun = readMazeFromMem();
  //curRun = 0;
  initializeFloodfill(returnState);
  calc(7,7,returnState, 15);
  while (true) {
    Serial.println("bob");
  }
  if (DEBUG) { 
    printMazeInfo(0,0); 
  }
}

void loop() {
  sw = digitalRead(10);
  if (sw == LOW) { 
    isRunning = true; 
  }
  if (sw && isRunning) {
    Serial.print("is running");

    //Check for goal state
    if (getFFScore(x, y) == 0) {
      returnState = !returnState;
      if (returnState) {
        curRun++;
        writeMazeToMem(curRun);
        isRunning = false;
//        x = 0;
//        y = 0;
//        curDir = 1;
        return;
//        Serial.println("FLIP");
//        flipFFScore(returnState);
//        //calc(0,0,returnState, 3);
//        Serial.println("done");
      } 
      else { //this is where we've completed one run.
        curRun++;
        writeMazeToMem(curRun);
        initializeFloodfill(returnState);
      }
    }

    walls = getWalls(x,y); 
    newWalls = 0;

    //  //If only one possible move, move.s
    //  if ((ALLWALLS - walls) == curDir || ((ALLWALLS - walls) << 2) == curDir) {
    //    Serial.println("Can only move straight");
    //    getAdjacentCell(x, y, curDir);
    //    moveTo(x, y, curDir);
    //    return;
    //  } 

    if (DEBUG) {
      Serial.println("In:");
      delay(3000);
      if (Serial.available() > 0) {
        char incomingBytes[2];
        Serial.readBytesUntil('\n', incomingBytes, 2);
        int incomingVal = atoi(incomingBytes);
        if (incomingVal > 0 && incomingVal <= 15)
          newWalls = incomingVal;
      } 
    } 
    else if (!DEBUG) {
      //sense what walls are surrounding the mouse
      newWalls = senseWalls(curDir);
    }

    //if they differ, from what we know, then we've found new walls
    if (newWalls > walls) {
      addNewWalls(x, y, newWalls);
      updateFloodfill(x, y, newWalls-walls, returnState, 15);
      calc(x,y,returnState, 15);
    }

    if (DEBUG) { 
      printMazeInfo(x,y); 
    }

    val = getFFScore(x,y);
    //TIP: For first steps, go straight until there is more than one option. Then start updating floodfill.
    //See what the options are
    for (int pow = curRun + 3; pow >= curRun; pow--) { //check the four directions
      dir = 1<<(pow%4);
      hasWall = wallExists(x, y, dir);
      row = x; 
      col = y;
      if (!hasWall) {
        getAdjacentCell(row, col, dir);
        nextVal = getFFScore(row, col);
        // Go straight if tie and if possible or set to smallest value
        if (val > nextVal) {//val == nextVal && dir == curDir || 
          nextRow = row;
          nextCol = col;
          nextVal = val;
        }
      }
    }
    curDir = moveFromTo(x,y, curDir, nextRow, nextCol);

    x = nextRow; 
    y = nextCol;
    Serial.print("Heading to: ");
    Serial.print(x);
    Serial.print(",");
    Serial.println(y);
  }
}




