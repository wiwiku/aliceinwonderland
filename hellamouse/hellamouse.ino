#include <QueueList.h>
#include <EEPROM.h>
#include "sensors.h"
#include "core.h"
#include "floodfill.h"
#include "motors.h"

byte x = 0;
byte y = 0;
byte curDir;
byte walls = 0;
byte newWalls = 0;
byte nextRow, nextCol, nextVal;
byte dir, row, col, val; 
boolean hasWall, returnState;

void setup() {
  Serial.begin(9600);
  returnState = false;
  curDir = NORTH;
  initializeThings();
}

void printMazeInfo() {
  char vertWall = '|';
  char horiWall = '--';
  char noWall = '  ';
  byte thisVal, thisWall;
  Serial.println("The maze is currently....");
  for (byte mazei = LENGTH-1; mazei >= 0; mazei--) {
    for (byte i = 0; i < 3; i++) {
      for (byte mazej = 0; mazej < LENGTH; mazej++) {
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
          thisVal = getFFScore(mazej, mazei);
          if (thisVal < 10) {
            Serial.print(" ");
          }
          Serial.print(thisVal);
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
  for (byte i = 0; i < LENGTH; i++) {
    Serial.print(" ");
    Serial.print(i);
    if (i < 10) {Serial.print(" ");}
    Serial.print(" ");
  }
  Serial.println("");
}

void initializeThings() {
  //If we shouldn't read from mem, mem should be cleared outside of this program
  readMazeFromMem();
  initializeFloodfill(returnState);
  calc(7,7,returnState);
  
  if (DEBUG) { 
    printMazeInfo(); 
  }
}

//void update(int x, int y, int newWalls, boolean returnState) {
//  /*Adds the cell in question and cells whose walls have changed to queue*/
//  qPush(rowColToI(x, y));
//  //for each new wall, add the adjacent cell to the queue
//  if(newWallExists(newWalls, EAST) || getFFScore(x+1, y) == UNDEFINED) {
//    pushIfValid(x+1, y);
//  }
//
//  if(newWallExists(newWalls, NORTH) || getFFScore(x, y+1) == UNDEFINED) {
//    pushIfValid(x, y+1);
//  }
//
//  if(newWallExists(newWalls, WEST) || getFFScore(x-1, y) == UNDEFINED) {
//    pushIfValid(x-1, y);
//  }
//
//  if(newWallExists(newWalls, SOUTH) || getFFScore(x, y-1) == UNDEFINED) {
//    pushIfValid(x, y-1);
//  }
//  Serial.print("Queue size is: ");
//  Serial.println(qCount());
//  Serial.print("This is in the queue: ");
//  Serial.println(qPeek());
//   calculateFFValues(x, y, returnState);
//}

void calc(byte x, byte y, boolean returnState) {
  while(!qEmpty()) {
    byte i = qPop() & 255;//double-check
    byte row, col;
    iToRowCol( row, col, i );
    Serial.print("We are checking this coordinate: ");
    Serial.print(row);
    Serial.print(", ");
    Serial.println(col);

    // Get smallest neighbor
    byte small = -1;// = getFFScore(row, col); //there should be some neighbor smaller than it
    if (!returnState && (row == 7 || row == 8) && (col == 7 || col == 8)) {
      small = getFFScore(row, col);
      small --;
    } else if (returnState && (row == 0) && (col == 0)) {
      small = getFFScore(row, col);
      small--;
    }  else {
      if(!wallExists(row, col, EAST)) {
        Serial.println("No east wall");
        if (small == -1 || getFFScore(row+1, col) < small) {
          small = getFFScore(row+1, col);
          Serial.println(small);
        }
      }
  
      if(!wallExists(row, col, NORTH)) {
        Serial.println("No north wall");
        if (small == -1 || getFFScore(row, col+1) < small) {
          small = getFFScore(row, col+1);
          Serial.println(small);
        }
      }
  
      if(!wallExists(row, col, WEST)) {
        Serial.println("No west wall");
        if (small == -1 || getFFScore(row-1, col) < small) {
          small = getFFScore(row-1, col);
          Serial.println(small);
        }
      }
  
      if(!wallExists(row, col, SOUTH)) {
        Serial.println("No south wall");
        if (small == -1 || getFFScore(row, col-1) < small) {
          small = getFFScore(row, col-1);
          Serial.println(small);
        }
      }
    }
        
    Serial.print("Its smallest value is...");
    Serial.print(small);

    small++; //value that it should be
    byte curVal = getFFScore(row, col); //value that it is

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

void loop() {
  //Check for goal state
  if (getFFScore(x, y) == 0) {
    returnState = !returnState;
    if (returnState) {
      flipFFScore(returnState);
      Serial.println("Goal");
    } else {
      //this is where we've completed one run. need to save maze and keep running....
      //Write maze to EPPROM
      writeMazeToMem();
      return;
    }
  }

  walls = getWalls(x,y); 
  newWalls = 0;

//  //If only one possible move, move.
//  if ((ALLWALLS - walls) == curDir || ((ALLWALLS - walls) << 2) == curDir) {
//    Serial.println("Can only move straight");
//    getAdjacentCell(x, y, curDir);
//    moveTo(x, y, curDir);
//    return;
//  } 

  if (DEBUG) {
    Serial.println("Input new walls sum (N=1, E=2, S=4, W=8): ");
    delay(5000);
    if (Serial.available() > 0) {
      char incomingBytes[2];
      Serial.readBytesUntil('\n', incomingBytes, 2);
      byte incomingVal = atoi(incomingBytes);

      if (incomingVal > 0 && incomingVal <= 15) {
        newWalls = incomingVal;
        Serial.print("I received: ");
        Serial.println(newWalls);
      } 
      else {
        Serial.print("I received: ");
        Serial.println(incomingVal);
        Serial.println("Buggy wall value");
      }
    } 
  } else if (!DEBUG) {
    //sense what walls are surrounding the mouse
    newWalls = senseWalls();
  }

  //if they differ, from what we know, then we've found new walls
  if (newWalls > walls) {
    Serial.println("WE NEED TO FLOODFILL!");
    addNewWalls(x, y, newWalls);
    updateFloodfill(x, y, newWalls-walls, returnState);
    //calc(x,y, returnState);
  }

  if (DEBUG) { 
    printMazeInfo(); 
  }

  val = getFFScore(x,y);
  //TIP: For first steps, go straight until there is more than one option. Then start updating floodfill.
  //See what the options are
  for (byte pow = 3; pow >= 0; pow--) { //check the four directions
    dir = 1<<pow;
    Serial.print("Checking this direction: ");
    Serial.println(dir);
    hasWall = wallExists(x, y, dir);
    row = x; 
    col = y;
    if (!hasWall) {
      Serial.println("It is open!");
      getAdjacentCell(row, col, dir);
      nextVal = getFFScore(row, col);
      Serial.print("The value of this square (");
      Serial.print(row);
      Serial.print(", ");
      Serial.print(col);
      Serial.print(") is ");
      Serial.println(nextVal);
      // Go straight if tie and if possible or set to smallest value
      if (val > nextVal) {//val == nextVal && dir == curDir || 
        nextRow = row;
        nextCol = col;
        nextVal = val;
      }
    }
  }
  curDir = x == nextRow? (y > nextCol ? SOUTH:NORTH) : 
  (x > nextRow ? WEST:EAST);
  x = nextRow; 
  y = nextCol;
  Serial.print("We are now HEADING to: ");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print("    And the direction is: ");
  Serial.println(curDir);

  moveTo(x,y, curDir);
}
















