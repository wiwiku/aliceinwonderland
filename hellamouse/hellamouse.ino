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
  Serial.println("The maze WALLS are currently...");
  for (int mazei = LENGTH-1; mazei >= 0; mazei--) {
    Serial.print(mazei);
    Serial.print("   ");
    for (int mazej = 0; mazej < LENGTH; mazej++) {
      Serial.print(getWalls(mazej, mazei));
      Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("The maze VALUES are currently...");
  for (int mazei = LENGTH-1; mazei >= 0; mazei--) {
    Serial.print(mazei);
    Serial.print("   ");
    for (int mazej = 0; mazej < LENGTH; mazej++) {
      Serial.print(getFFScore(mazej, mazei));
      Serial.print(" ");
    }
    Serial.println("");
  }
}
/*For each cell in the queue, check that its value is 1+lowest neighbor. If not, set it and check its neighbors*/
void calc(int x, int y) {
  while(!qEmpty()) {  
  Serial.print("Queue size: ");
  Serial.println(qCount());
  Serial.print("What is inside: ");
  Serial.println(qPeek());
    int i = qPop() & 255;//double-check
    int row, col;
    iToRowCol( row, col, i );
    Serial.print("I'm at: ");
    Serial.print(row);
    Serial.print(", ");
    Serial.println(col);

    int small = getFFScore(row, col); //there should be some neighbor smaller than it
    if ((row == 7 || row == 8) && (col == 7 || col == 8)) {
      small--;
    } else {
      if(!wallExists(row, col, EAST)) {
        if (getFFScore(row+1, col) < small) {
          small = getFFScore(row+1, col);
        }
      }
  
      if(!wallExists(row, col, NORTH)) {
        if (getFFScore(row, col+1) < small) {
          small = getFFScore(row, col+1);
        }
      }
  
      if(!wallExists(row, col, WEST)) {
        if (getFFScore(row-1, col) < small) {
          small = getFFScore(row-1, col);
        }
      }
  
      if(!wallExists(row, col, SOUTH)) {
        if (getFFScore(row, col-1) < small) {
          small = getFFScore(row, col-1);
        }
      }
    }

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

void initializeThings() {
  initializeFloodfill();  
  calc(7,7);
  
  if (DEBUG) { printMazeInfo(); }
  
  Serial.println("values");
  //check that the values are right after initilization
  Serial.println(getWalls(0,0));
  Serial.println(getFFScore(0,0));

  Serial.println(getWalls(7,7));
  Serial.println(getFFScore(7,7));
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

  Serial.println("one");
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
        Serial.print(newWalls);
        Serial.println("");
      } 
      else {
        Serial.println("Buggy wall value");
      }
    } 
  }

  Serial.println("three");
  if (!DEBUG) {
    //sense what walls are surrounding the mouse
    newWalls = senseWalls();
  }

  //if they differ, from what we know, then we've found new walls
  if (newWalls > walls) {
    Serial.println("WE NEED TO FLOODFILL!!");
    addNewWalls(x, y, newWalls);
    updateFloodfill(x, y, newWalls);
    calc(x,y);
    Serial.print("Queue size right now is: ");
    Serial.println(qCount());
    Serial.println("The walls have changed!!");
  }

  if (DEBUG) { printMazeInfo(); }

//  if (DEBUG) {
//    nextRow++;
//    if (nextRow >= LENGTH) {
//      nextRow = 0;
//      nextCol++;
//    }
//    if (nextCol >= LENGTH) {
//      nextCol = 0;
//      nextRow = 0;
//    }
//    x = nextCol;
//    y = nextRow;
//  }

    Serial.println("four");
      val = getFFScore(x,y);
    //TIP: For first steps, go straight until there is more than one option. Then start updating floodfill.
    //See what the options are
      for (int pow = 0; pow < 4; pow++) { //check the four directions
        dir = 1<<pow;
        hasWall = wallExists(x, y, dir);
        row = x; 
        col = y;
        Serial.print("Checking: ");
        Serial.println(dir);
        if (!hasWall) {
          Serial.println("There isn't a wall here.");
          getAdjacentCell(row, col, dir);
                  Serial.print("WE are now checking: ");
  Serial.print(row);
  Serial.print(",");
  Serial.println(col);
    
          nextVal = getFFScore(row, col);
          Serial.print("The score of this cell is: ");
          Serial.println(nextVal);
          
          Serial.println("five");
          // Go straight if tie and if possible or set to smallest value
          if (val == nextVal && dir == curDir || val > nextVal) {
            Serial.println("It's the next best thing!");
            nextRow = row;
            nextCol = col;
            nextVal = val;
          }
        }
  

      }
        Serial.println("six");
        curDir = x == nextCol? (y > nextRow ? NORTH:SOUTH) : 
        (x > nextCol ? EAST:WEST);
        x = nextRow; 
        y = nextCol;
  Serial.print("WE are now heading to: ");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print("    And the direction is: ");
  Serial.println(curDir);

  moveTo(x,y, curDir);
}










