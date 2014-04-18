#include "core.h"

//TODO: figure out when to take multiple steps or 45 degree turns
int moveFromTo(int row, int col, int dir, int nextRow, int nextCol){
  int nextDir = x == nextRow? (y > nextCol ? SOUTH:NORTH) : 
  (x > nextRow ? WEST:EAST);
  
  int diff = dir - nextDir;
  if (dir == nextDir) {
    //move forward
  } else if (dir) {
   //turn left
  } else if (dir) {
    //turn right
  } else if () {
    //180 in place turn
  }
  
  return nextDir;
}
