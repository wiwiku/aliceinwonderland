#include <QueueList.h>
#include "core.h"

int floodfillArr[LENGTH][LENGTH];

/* queue stores points that need to be rechecked in updating floodfill
  each element consists of the floodfill value (1st 8 bits) and the 
  array index (last 8 bits) */
QueueList <int> queue;

/* Returns the floodfill value at the specified point */
int getFFVal(int row, int col) {
  return floodfillArr[row][col];
}

/* Sets the floodfill value at the specified point */
void setFFVal(int row, int col, int val){
  floodfillArr[row][col] = val;
}

/* Sets the last half of the floodfill value (the computed score) */
void setFFScore(int row, int col, int score) {
  setFFVal(row, col, (score & 255) | (getFFVal(row, col) & (255 << 8)));
}

/* Calculates the index if the 2D array were flattened */
int rowColToI(int row, int col) {
  return row * LENGTH + col;
}

/* Sets row and col to the values in a 2D array given the index in a 1D array */
void iToRowCol(int &row, int &col, int i) {
  row = (int) i / LENGTH;
  col = 0;
}

/* Returns the first 8 bits of the value at the specified point */
int getWalls(int row, int col) {
  return (getFFVal(row, col) >> 8) | 255;
}

/*int calculateWallIndex(int row, int col, int dir) {
 int index;
 int corner;
 
 // Up or down
 if(dir == 0 || dir == 2) {
 index = rowColToI(
 dir == 0 ? row : row - 1, col
 );
 }
 
 // Right or left
 else if(dir == 1 || dir == 3) {
 index = colRowtoZ(
 corner == 3 ? 
 (dir == 1 ? col : col - 1) : 
 (dir == 3 ? col : col - 1),
 row
 );
 }
 
 return index;
 }
 */

/* Check if a wall in the given direction exists */
bool wallExists(int row, int col, int dir) {
  if (row <= 0 || col <= 0 || row >= LENGTH-1 || col >= LENGTH -1) {
    return true;
  } else if (getWalls(row, col) & dir > 0){
    return true;
  }
  
  return false;
}

/* Check if a new wall in the given direction exists */
bool newWallExists(int newWalls, int dir) {
  return newWalls & (dir & 255) > 0;
}

/* Sets wall value of this coordinate point */
void setWalls(int row, int col, int walls) {
  setFFVal(row, col, (getFFVal(row, col) & (255 << 8)) | (walls << 8));
}

/* Set row and col to the coordinates in that direction */
void getAdjacentCell(int &row, int &col, int dir) {
  //int subject = corner == 3 ? 1 : -1;
  //TODO: check sides - what if there is no adjacent cell?
  //technically should not be called when there is a wall on that side.
  switch(dir) {
  case NORTH: 
    row = row + 1; 
    break;
  case EAST: 
    col = col + 1; 
    break;
  case SOUTH: 
    row = row - 1; 
    break;
  case WEST: 
    col = col - 1; 
    break;
  }
}

/* Return value of smallest open neighbor */
int minNeighbor(int row, int col) {
  int small = 300; //val should never be greater than 255
  if(!wallExists(row, col, NORTH)) {
    if (getFFVal(row+1, col) < small) {
      small = getFFVal(row+1, col);
    }
  }

  // Check the cel to the east
  if(!wallExists(row, col, EAST)) {
    if (getFFVal(row, col+1) < small) {
      small = getFFVal(row, col+1);
    }
  }

  // Check the cell to the south
  if(!wallExists(row, col, SOUTH)) {
    if (getFFVal(row-1, col) < small) {
      small = getFFVal(row-1, col);
    }
  }

  // Check the cell to the west
  if(!wallExists(row, col, WEST)) {
    if (getFFVal(row, col-1) < small) {
      small = getFFVal(row, col-1);
    }
  }
  
  return small;
}

//Update values taht need to be updated.
void updateFloodfill(int x, int y, int newWalls) {
  int row, col;
  queue.push(rowColToI(x, y));

  //for each new wall, add the adjacent cell to the queue
  // Check the cell to the north
  if(newWallExists(newWalls, NORTH) || getFFVal(row+1, col) == UNDEFINED) {
    queue.push(rowColToI(row+1, col));
  }

  // Check the cel to the east
  if(newWallExists(newWalls, EAST) || getFFVal(row, col+1) == UNDEFINED) {
    queue.push(rowColToI(row, col+1));
  }

  // Check the cell to the south
  if(newWallExists(newWalls, SOUTH) || getFFVal(row-1, col) == UNDEFINED) {
    queue.push(rowColToI(row-1, col));
  }

  // Check the cell to the west
  if(newWallExists(newWalls, WEST) || getFFVal(row, col-1) == UNDEFINED) {
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
    int curVal = getFFVal(row, col) & 255;

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

//initial starting maze values.
void initializeFloodfill() {
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

  //starting position
  setWalls(0,0, 14);
  
  updateFloodfill(7, 7, 12);
}
