#include <QueueList.h>
#include <EEPROM.h>
#include "core.h"

/* row is the x-value, col is the y-value */
int floodfillArr[LENGTH][LENGTH];

/* queue stores points that need to be rechecked in updating floodfill
 each element consists of the floodfill value (1st 8 bits) and the 
 array index (last 8 bits) */
QueueList <int> queue;

/* Queue accessor methods for debugging purposes */
boolean qEmpty() {
  return queue.isEmpty();
}
int qPop() {
  return queue.pop();
}
void qPush(int i){
  queue.push(i);
}
int qCount() {
  return queue.count();
}
int qPeek() {
  return queue.peek();
}

/* Returns the stored value at the specified point */
int getArrVal(int row, int col) {
  return floodfillArr[row][col];
}

/* Sets the stored value at the specified point */
void setArrVal(int row, int col, int val){
  floodfillArr[row][col] = val;
}

/* Returns the first 8 bits of the value at the specified point */
int getWalls(int row, int col) {
  return (getArrVal(row, col) >> 8) & 15;
}

/* Returns the flood fill score */
int getFFScore(int row, int col) {
  return floodfillArr[row][col] & 255;
}

/* Sets the last half of the floodfill value (the computed score) */
void setFFScore(int row, int col, int score) {
  setArrVal(row, col, (score & 255) | (getWalls(row, col) << 8));
}

/* Check if a wall in the given direction exists */
bool wallExists(int row, int col, int dir) {
  if ((row <= 0 && (dir==WEST)) || (col <= 0 && (dir == SOUTH)) || (row >= LENGTH-1 && dir == EAST) || (col >= LENGTH -1 && dir == NORTH)) {
    return true;
  } 
  else if ((getWalls(row, col) & dir) == dir){
    return true;
  }
  return false;
}

/* Check if a new wall in the given direction exists */
bool newWallExists(int newWalls, int dir) {
  return (newWalls & (dir & 15)) > 0;
}

/* Sets wall value [0-15] of this coordinate point */
void addNewWalls(int row, int col, int walls) {
  if (walls >= 0 && walls < 16) {
    setArrVal(row, col, (getFFScore(row, col) | ((getWalls(row,col) | walls) << 8)));
  }
  //sync wall values for adjacent cells
  if (newWallExists(walls, NORTH) && col < 15) {
    setArrVal(row, col+1, (getFFScore(row, col+1) | ((getWalls(row, col+1) | SOUTH) << 8)));
  }
  if (newWallExists(walls, EAST) && row < 15) {
    setArrVal(row+1, col, (getFFScore(row+1, col) | ((getWalls(row+1, col) | WEST) << 8)));
  }
  if (newWallExists(walls, SOUTH) && col > 0) {
    setArrVal(row, col-1, (getFFScore(row, col-1) | ((getWalls(row, col-1) | NORTH) << 8)));
  }
  if (newWallExists(walls, WEST) && row > 0) {
    setArrVal(row-1, col, (getFFScore(row-1, col) | ((getWalls(row-1, col) | EAST) << 8)));
  }
}

/* Calculates the index if the 2D array were flattened */
int rowColToI(int row, int col) {
  return row + (LENGTH * col);
}

/* Sets row and col to the values in a 2D array given the index in a 1D array */
void iToRowCol(int &row, int &col, int i) {
  col = (int) i / LENGTH;
  row = i - (col*16);
}

/* Set row and col to the coordinates in that direction */
void getAdjacentCell(int &row, int &col, int dir) {
  //int subject = corner == 3 ? 1 : -1;
  //TODO: check sides - what if there is no adjacent cell?
  //technically should not be called when there is a wall on that side
  switch(dir) {
  case NORTH: 
    col = col + 1; 
    break;
  case EAST: 
    row = row + 1; 
    break;
  case SOUTH: 
    col = col - 1; 
    break;
  case WEST: 
    row = row - 1; 
    break;
  }
}

/* Bounds checking */
void pushIfValid(int x, int y) {
  if (x >= 0 && x < LENGTH && y >= 0 && y < LENGTH) {
    queue.push( rowColToI(x, y));
  }
}

/*For each cell in the queue, check that its value is 1+lowest neighbor. If not, set it and check its neighbors*/
void calculateFFValues(int x, int y, boolean returnState) {
  while(!queue.isEmpty()) {
    int i = queue.pop() & 255;//double-check
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

    small++; //value that it should be
    int curVal = getFFScore(row, col); //value that it is

    if(curVal != UNDEFINED && curVal == small) { 
      continue; 
    }

    // Floodfill this cell  
    setFFScore(row, col, small);

    // Check the cell to the north
    if(!wallExists(row, col, NORTH)) {
      queue.push( rowColToI(row, col+1));
    }

    // Check the cel to the east
    if(!wallExists(row, col, EAST)) {
      queue.push( rowColToI(row+1, col));
    }

    // Check the cell to the south
    if(!wallExists(row, col, SOUTH)) {
      queue.push( rowColToI(row, col-1));
    }

    // Check the cell to the west
    if(!wallExists(row, col, WEST)) {
      queue.push( rowColToI(row-1, col));
    }
  }
}

/* Update values that need to be updated */
void updateFloodfill(int x, int y, int newWalls, boolean returnState) {

  /*Adds the cell in question and cells whose walls have changed to queue*/
  queue.push(rowColToI(x, y));

  //for each new wall, add the adjacent cell to the queue
  if(newWallExists(newWalls, EAST) || getFFScore(x+1, y) == UNDEFINED) {
    pushIfValid(x+1, y);
  }

  if(newWallExists(newWalls, NORTH) || getFFScore(x, y+1) == UNDEFINED) {
    pushIfValid(x, y+1);
  }

  if(newWallExists(newWalls, WEST) || getFFScore(x-1, y) == UNDEFINED) {
    pushIfValid(x-1, y);
  }

  if(newWallExists(newWalls, SOUTH) || getFFScore(x, y-1) == UNDEFINED) {
    pushIfValid(x, y-1);
  }
  calculateFFValues(x, y, returnState);
}

/* Initial starting maze values */
void initializeFloodfill(boolean returnState) {
  for(int i = 0; i < LENGTH; i++) {
    for(int j = 0; j < LENGTH; j++) {
      setFFScore(i, j, UNDEFINED);
    }
  }

  //set center to 0
  setFFScore(7, 7, 0);
  setFFScore(7, 8, 0);
  setFFScore(8, 7, 0);
  setFFScore(8, 8, 0);

  //set the initial cell values
  updateFloodfill(7, 7, 12, returnState);
}

/* Sets (0,0) to be the goal state and recalculates floodfill values */
void flipFFScore(boolean returnState) {
  for(int i = 0; i < LENGTH; i++) {
    for(int j = 0; j < LENGTH; j++) {
      setFFScore(i, j, UNDEFINED);
    }
  }
  setFFScore(0,0,0);
  updateFloodfill(0,0,3, returnState);
}

/* Read values stored in EEPROM */
int readMazeFromMem() {
  for (int mazei = 0; mazei < LENGTH; mazei++) {
    for (int mazej = 0; mazej < LENGTH; mazej++) 
      addNewWalls(mazei, mazej, EEPROM.read(rowColToI(mazei, mazej)));
  }
  return EEPROM.read(256);//for curRun var
}

/* Writes walls to EEPROM. If only checks for what is 0, might overlap with previously saved info */
void writeMazeToMem(int curRun) {
  for (int mazei = 0; mazei < LENGTH; mazei++) {
    for (int mazej = 0; mazej < LENGTH; mazej++) 
      EEPROM.write(rowColToI(mazei, mazej), getWalls(mazei, mazej));
  }
  EEPROM.write(256, curRun);
}

