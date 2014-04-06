#include <QueueList.h>
#include "sensors.h"
#include "core.h"
#include "floodfill.h"
#include "motors.h"

//memory
int prevCol = 0;
int prevRow = 0;
int x = 0;
int y = 0;
int curDirection = NORTH;
int walls = 0;
int dir = NORTH;

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
  //get what we have stored for the walls
  //int walls;

  walls = getWalls(x,y);

  int nextRow, nextCol, nextVal;

  //Check for goal state
  if (getFFVal(x, y) == 0) {
    //do something? switch to backtrack mode
    return;
  }

  // If only one possible move, move.
  if ((ALLWALLS - walls) == curDirection) {
    //move straight
    Serial.println("Move straight");
    //next iteration of this loop() method
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
      if (val == nextVal && dir == curDirection || val < nextVal) {
        nextRow = row;
        nextCol = col;
        nextVal = val;
      }
    }

    curDirection = x == nextCol? (y > nextRow ? NORTH:SOUTH) : 
    (x > nextCol ? EAST:WEST);
    x = nextCol; 
    y = nextRow;
  }

  //which direction needs to face and coordinates to move to
  moveTo(x,y, curDirection);
}

