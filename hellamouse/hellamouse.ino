#include <QueueList.h>
#include "sensors.h"
#include "core.h"
#include "floodfill.h"
#include "motors.h"

//memory
int x = 0;
int y = 0;
int curDir = NORTH;
int walls = 0;
int newWalls = 0;
int dir = NORTH;
int nextRow, nextCol, nextVal;

boolean done = false;

void setup() {
  Serial.begin(9600);
  initializeThings();
  senseWalls();
}

void initializeThings() {
  initializeFloodfill();  
  if (DEBUG) {
    Serial.println("The maze walls are currently...");
    for (int mazej = LENGTH-1; mazej >= 0; mazej--) {
      Serial.println(mazej);
      Serial.println("   ");
      for (int mazei = 0; mazei < LENGTH; mazei++) {
        Serial.print(getWalls(mazei, mazej));
        Serial.print(" ");
      }
      Serial.println("");
    }
  }
  Serial.println("values");
  //check that the values are right after initilization
  Serial.println(getWalls(0,0));
  Serial.println(getFFScore(0,0));
  
  Serial.println(getWalls(7,7));
  Serial.println(getFFScore(0,0));
  if (DEBUG) {
    Serial.println("The maze values are currently...");
      for (int mazej = LENGTH-1; mazej >= 0; mazej--) {
      Serial.println(mazej);
      Serial.println("   ");
    for (int mazei = 0; mazei < LENGTH; mazei++) {
        Serial.print(getFFScore(mazei, mazej));
        Serial.print(" ");
      }
      Serial.println("");
    }
  }
}

void loop() {
  walls = getWalls(x,y);  

    //Check for goal state
    if (getFFScore(x, y) == 0) {
      Serial.println("Goal");
      //do something? switch to backtrack mode
      return;
    }
  Serial.println("one");
  // If only one possible move, move.
  //  if ((ALLWALLS - walls) == curDir) {
  //    Serial.println("one.5");
  //    moveTo(x, y, curDir);
  //    //Serial.println("Move straight");
  //    return;
  //  } 

  Serial.println("two");
  if (DEBUG) {
    Serial.println("Input new walls sum (N=1, E=2, S=4, W=8): ");
  delay(8000);
      if (Serial.available() > 0) {
        Serial.println("two.5");
        char incomingBytes[2];
        Serial.readBytesUntil('\n', incomingBytes, 2);
        int incomingVal = atoi(incomingBytes);

        if (incomingVal > 0 && incomingVal <= 15) {
          newWalls = incomingVal;
          Serial.print("I received: ");
          Serial.print(newWalls);
          Serial.println("");
        } 
        else {
          Serial.println("Buggy wall value!!!!");
        }
      } 
  }

  Serial.println("three");
  if (!DEBUG) {
    //sense what walls are surrounding the mouse
    newWalls = senseWalls();
  }

  //if they differ, from what we know, then we've found new walls
  if (newWalls != walls) {
    updateFloodfill(x, y, newWalls);
    addNewWalls(x, y, newWalls);
    Serial.println("The walls have changed!!");
  }
  

  if (DEBUG) {
    Serial.println("The maze is currently...");
    for (int mazei = LENGTH-1; mazei >= 0; mazei--) {
      
      Serial.println(mazej);
      Serial.println("   ");
      for (int mazej = 0; mazej < LENGTH; mazej++) {
        //      Serial.print(mazej, DEC);
        //      Serial.print(",");
        //      Serial.print(mazei, DEC);
        //      Serial.print(": ");
        Serial.print(getWalls(mazej, mazei));
        Serial.print(" ");
      }
      Serial.println("");
    }
  }
  if (DEBUG) {
    nextRow++;
    if (nextRow >= LENGTH) {
      nextRow = 0;
      nextCol++;
    }
    if (nextCol >= LENGTH) {
      nextCol = 0;
      nextRow = 0;
    }
    x = nextCol;
    y = nextRow;
  }
  Serial.println("four");
  //  nextVal = getFFVal(x,y);
  //TIP: For first steps, go straight until there is more than one option. Then start updating floodfill.
  //See what the options are
  //  for (int pow = 1; pow <= 4; pow++) { //check the four directions
  //    int dir = 1<<pow;
  //    boolean hasWall = wallExists(x, y, (1<<dir));
  //    int row = x; 
  //    int col = y;
  //    if (!hasWall) {
  //      getAdjacentCell(row, col, dir);
  //
  //      int val = getFFVal(row, col);
  //      Serial.println("five");
  //      // Go straight if tie and if possible or set to smallest value
  //      if (val == nextVal && dir == curDir || val < nextVal) {
  //        nextRow = row;
  //        nextCol = col;
  //        nextVal = val;
  //      }
  //    }

  //    Serial.println("six");
  //    curDir = x == nextCol? (y > nextRow ? NORTH:SOUTH) : 
  //    (x > nextCol ? EAST:WEST);
  //    x = nextCol; 
  //    y = nextRow;
  //  }
  Serial.print("WE are now at: ");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print("||||");
  Serial.print("Wall value is: ");
  Serial.println(getWalls(x,y));
  
  moveTo(x,y, curDir);
}








