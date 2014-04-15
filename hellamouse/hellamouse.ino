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
int nextRow, nextCol, nextVal;
int dir, row, col, val; 
boolean hasWall;

void setup() {
  Serial.begin(9600);
  initializeThings();
}

void printMazeInfo() {
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
  for (int i = 0; i < LENGTH; i++) {
    Serial.print(" ");
    Serial.print(i);
    if (i < 10) {Serial.print(" ");}
    Serial.print(" ");
  }
  Serial.println("");
}

void initializeThings() {
  initializeFloodfill();  

  if (DEBUG) { 
    printMazeInfo(); 
  }
}

void loop() {
  //Check for goal state
  if (getFFScore(x, y) == 0) {
    Serial.println("Goal");
    //do something? switch to backtrack mode
    return;
  }

  walls = getWalls(x,y); 
  newWalls = 0;

  //If only one possible move, move.
  if ((ALLWALLS - walls) == curDir) {
    Serial.println("Can only move straight");
    getAdjacentCell(x, y, curDir);
    moveTo(x, y, curDir);
    return;
  } 

  if (DEBUG) {
    Serial.println("Input new walls sum (N=1, E=2, S=4, W=8): ");
    delay(3000);
    if (Serial.available() > 0) {
      char incomingBytes[2];
      Serial.readBytesUntil('\n', incomingBytes, 2);
      int incomingVal = atoi(incomingBytes);

      if (incomingVal > 0 && incomingVal <= 15) {
        newWalls = incomingVal;
        Serial.print("I received: ");
        Serial.println(newWalls);
      } 
      else {
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
    updateFloodfill(x, y, newWalls);
  }

  if (DEBUG) { 
    printMazeInfo(); 
  }

  val = getFFScore(x,y);
  //TIP: For first steps, go straight until there is more than one option. Then start updating floodfill.
  //See what the options are
  for (int pow = 0; pow < 4; pow++) { //check the four directions
    dir = 1<<pow;
    hasWall = wallExists(x, y, dir);
    row = x; 
    col = y;
    if (!hasWall) {
      getAdjacentCell(row, col, dir);
      nextVal = getFFScore(row, col);

      // Go straight if tie and if possible or set to smallest value
      if (val == nextVal && dir == curDir || val > nextVal) {
        nextRow = row;
        nextCol = col;
        nextVal = val;
      }
    }


  }
  curDir = x == nextCol? (y > nextRow ? NORTH:SOUTH) : 
  (x > nextCol ? EAST:WEST);
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
















