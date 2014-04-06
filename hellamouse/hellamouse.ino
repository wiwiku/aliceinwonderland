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
int dir = NORTH;
int nextRow, nextCol, nextVal;

boolean done = false;

void setup() {
  Serial.begin(9600);
  initializeThings();

  boolean north, east, south, west;
  senseWalls();
}

void initializeThings() {
  initializeFloodfill();
}

void loop() {
  walls = getWalls(x,y);

  //Check for goal state
  if (getFFVal(x, y) == 0) {
    //do something? switch to backtrack mode
    return;
  }

  // If only one possible move, move.
  if ((ALLWALLS - walls) == curDir) {
    moveTo(x, y, curDir);
    Serial.println("Move straight");
    return;
  } 

  //sense what walls are surrounding the mouse
  int newWalls = senseWalls();

  //if they differ, from what we know, then we've found new walls
  if (newWalls != walls) {
    updateFloodfill(x, y, newWalls);
    setWalls(x, y, newWalls);
  }

  nextVal = getFFVal(x,y);
  //TIP: For first steps, go straight until there is more than one option. Then start updating floodfill.
  //See what the options are
  for (int dir = 1; dir <= 8; dir*2) { //check the four directions
    boolean hasWall = wallExists(x, y, dir);
    int row = x; 
    int col = y;
    if (!hasWall) {
      // TODO deal with edges and corners (probably in wallExists)
      getAdjacentCell(row, col, dir);

      int val = getFFVal(row, col);

      // Go straight if tie and if possible or set to smallest value
      if (val == nextVal && dir == curDir || val < nextVal) {
        nextRow = row;
        nextCol = col;
        nextVal = val;
      }
    }

    curDir = x == nextCol? (y > nextRow ? NORTH:SOUTH) : 
    (x > nextCol ? EAST:WEST);
    x = nextCol; 
    y = nextRow;
  }

  moveTo(x,y, curDir);
}

