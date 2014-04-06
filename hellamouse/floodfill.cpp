#include <QueueList.h>
#include "core.h"

int floodfillArr[LENGTH][LENGTH]; //does it have 16 bits or 8? if 8, might need walls arrays.
QueueList <int> queue;


int getFFVal(int row, int col) {
  return floodfillArr[row][col];
}

void setFFVal(int row, int col, int val){
  floodfillArr[row][col] = val;
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
int getWalls(int row, int col) {
  int val = getFFVal(row, col);
  return (val >> 8) | 255;
}

//==============================
bool wallExists(int row, int col, int dir) {
  //  if(corner == UNDEFINED) { return false; }
  //
  //	int index = calculateWallIndex(row, col, dir);
  //	boolean *arr = dir == 0 || dir == 2 ? wallsHorizontal : wallsVertical;
  //	
  //	return (index < 0 || index > AREA_WALLS) ? true : arr[index];

  //deal with corners and sides here
  int walls = getWalls(row, col);
  if (walls & dir == dir){
    return true;
  }
  return false;
}

//Sets wall value of this coordinate point
void setWalls(int row, int col, int walls) {
  //  if(corner == UNDEFINED) { 
  //    return; 
  //  }

  //  int index = calculateWallIndex(row, col, dir);
  //  boolean *arr = dir == 0 || dir == 2 ? wallsHorizontal : wallsVertical;
  //
  //  // Ensure we don't go outside the walls range
  //  if(index < 0 || index > AREA_WALLS) { 
  //    return; 
  //  }
  //
  //  // Set the value
  //  arr[index] = state;

  int val = getFFVal(row, col);
  setFFVal(row, col, (val & (255 << 8)) | (walls << 8));
}

//Set row and col to the coordinates in that direction
void getAdjacentCell(int &row, int &col, int dir) {
  //int subject = corner == 3 ? 1 : -1;

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

int minNeighbor(int x, int y) {
  int small = 300; //val should never be greater than 255
  int row = x; 
  int col = y;
  if(!wallExists(x, y, NORTH)) {
    getAdjacentCell(row, col, NORTH);
    if (getFFVal(row, col) < small) {
      small = getFFVal(row, col);
    }
  }

  // Check the cel to the east
  if(!wallExists(x, y, EAST)) {
    row = x; 
    col = y;
    getAdjacentCell(row, col, EAST);
    if (getFFVal(row, col) < small) {
      small = getFFVal(row, col);
    }
  }

  // Check the cell to the south
  if(!wallExists(x, y, SOUTH)) {
    row = x; 
    col = y;
    getAdjacentCell(row, col, SOUTH);
    if (getFFVal(row, col) < small) {
      small = getFFVal(row, col);
    }
  }

  // Check the cell to the west
  if(!wallExists(x, y, WEST)) {
    row = x; 
    col = y;
    getAdjacentCell(row, col, WEST);
    if (getFFVal(row, col) < small) {
      small = getFFVal(row, col);
    }
  }
}

int rowColToI(int row, int col) {
  return row * LENGTH + col;
}

void iToRowCol(int &row, int &col, int i) {
  row = (int) i / LENGTH;
  col = 0;
}

//Update values taht need to be updated.
void updateFloodfill(int x, int y, int newWalls) {
  int row, col;
  queue.push(rowColToI(x, y));

  //for each new wall, add the adjacent cell to the queue
  // for (int j = 0; j < 4; j++) {
  //bitwise stuff
  // Check the cell to the north
  if(wallExists(row, col, NORTH)) {
    int row = x; 
    int col = y;
    getAdjacentCell(row, col, NORTH);
    queue.push(rowColToI(row, col));
  }

  // Check the cel to the east
  if(wallExists(row, col, EAST)) {
    row = x;  
    col = y;
    getAdjacentCell(row, col, EAST);
    queue.push(rowColToI(row, col));
  }

  // Check the cell to the south
  if(wallExists(row, col, SOUTH)) {
    row = x; 
    col = y;
    getAdjacentCell(row, col, SOUTH);
    queue.push(rowColToI(row, col));
  }

  // Check the cell to the west
  if(wallExists(row, col, WEST)) {
    row = x; 
    col = y;
    getAdjacentCell(row, col, WEST);
    queue.push(rowColToI(row, col));
  }
  // }

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
    setFFVal(row, col, curVal | (val & 255));

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
      setFFVal(row, col, UNDEFINED);
    }
  }

  //set center to 0
  setFFVal(7, 7, 0);
  setFFVal(7, 8, 0);
  setFFVal(8, 7, 0);
  setFFVal(8, 8, 0);

  //starting position
  setWalls(0,0, 14);  
  //TODO: fill in the mze with inital numbers
  updateFloodfill(7, 7, 1);
}

