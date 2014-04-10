#include <QueueList.h>
#include "core.h"

/* row is the x-value, col is the y-value */
int floodfillArr[LENGTH][LENGTH];

/* queue stores points that need to be rechecked in updating floodfill
  each element consists of the floodfill value (1st 8 bits) and the 
  array index (last 8 bits) */
QueueList <int> queue;

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

/* Sets wall value [0-15] of this coordinate point */
void addNewWalls(int row, int col, int walls) {
  if (walls >= 0 && walls < 16) {
    setArrVal(row, col, (getFFScore(row, col) | ((getWalls(row,col) | walls) << 8)));
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

/* Check if a wall in the given direction exists */
bool wallExists(int row, int col, int dir) {
  if ((row <= 0 && (dir==WEST)) || (col <= 0 && (dir == SOUTH)) || (row >= LENGTH-1 && dir == EAST) || (col >= LENGTH -1 && dir == NORTH)) {
    return true;
  } else if ((getWalls(row, col) & dir) == dir){
    return true;
  }
  return false;
}

/* Check if a new wall in the given direction exists */
bool newWallExists(int newWalls, int dir) {
  return (newWalls & (dir & 15)) > 0;
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

/* Return value of smallest open neighbor */
int minNeighbor(int row, int col) {
  int small = getFFScore(row, col); //there should be some neighbor smaller than it
  if(!wallExists(row, col, NORTH)) {
    if (getFFScore(row+1, col) < small) {
      small = getFFScore(row+1, col);
    }
  }
  // Check the cel to the east
  if(!wallExists(row, col, EAST)) {
    if (getFFScore(row, col+1) < small) {
      small = getFFScore(row, col+1);
    }
  }
  // Check the cell to the south
  if(!wallExists(row, col, SOUTH)) {
    if (getFFScore(row-1, col) < small) {
      small = getFFScore(row-1, col);
    }
  }
  // Check the cell to the west
  if(!wallExists(row, col, WEST)) {
    if (getFFScore(row, col-1) < small) {
      small = getFFScore(row, col-1);
    }
  }
  return small;
}

/* Update values that need to be updated */
void updateFloodfill(int x, int y, int newWalls) {
  int row, col;
  queue.push(rowColToI(x, y));

  //for each new wall, add the adjacent cell to the queue
  // Check the cell to the north
  if(newWallExists(newWalls, NORTH) || getArrVal(row+1, col) == UNDEFINED) {
    setArrVal(row+1, col, 0);
    queue.push(rowColToI(row+1, col));
  }

  // Check the cel to the east
  if(newWallExists(newWalls, EAST) || getArrVal(row, col+1) == UNDEFINED) {
    setArrVal(row, col+1, 0);
    queue.push(rowColToI(row, col+1));
  }

  // Check the cell to the south
  if(newWallExists(newWalls, SOUTH) || getArrVal(row-1, col) == UNDEFINED) {
    setArrVal(row-1, col, 0);
    queue.push(rowColToI(row-1, col));
  }

  // Check the cell to the west
  if(newWallExists(newWalls, WEST) || getArrVal(row, col-1) == UNDEFINED) {
    setArrVal(row, col-1, 0);
    queue.push(rowColToI(row, col-1));
  }

  //for each thing in the queue, check that it's value is 1+lowest neighbor, if not. set it and add its neighbors
  //if all good. all good.
  while(!queue.isEmpty()) {
    int x = queue.pop();
    int i = x & 255; //bits

    int row, col;
    iToRowCol( i, row, col );

    //int val = (int) (x >> 8) & 255;
    int val = minNeighbor(row, col) + 1;
    int curVal = getFFScore(row, col);

    // Floodfill this cell
    if(curVal != UNDEFINED && curVal == val) { 
      continue; 
    }
    setFFScore(row, col, val);

    // Check the cell to the north
    if(!wallExists(row, col, NORTH)) {
      //queue.push( rowColtoZ(row + 1, col) | ((val) << 8) );
      queue.push( rowColToI(row + 1, col));
    }

    // Check the cel to the east
    if(!wallExists(row, col, EAST)) {
      queue.push( rowColToI(row, col + 1));
    }

    // Check the cell to the south
    if(!wallExists(row, col, SOUTH)) {
      queue.push( rowColToI(row - 1, col));
    }

    // Check the cell to the west
    if(!wallExists(row, col, WEST)) {
      queue.push( rowColToI(row, col - 1));
    }
  }
}

/* Initial starting maze values */
void initializeFloodfill() {
  for(int i = 0; i < LENGTH; i++) {
    for(int j = 0; j < LENGTH; j++) {
      setArrVal(i, j, UNDEFINED);
    }
  }

  //set center to 0
  setFFScore(7, 7, 0);
  setFFScore(7, 8, 0);
  setFFScore(8, 7, 0);
  setFFScore(8, 8, 0);
  
  updateFloodfill(7, 7, 12);
}
