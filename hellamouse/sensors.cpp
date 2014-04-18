#include "core.h"

/* Return sum of wall values */
int senseWalls(int dir) {
  int walls = 0;
  if (lsensor.hasWall()) {
    if (dir == 1) { walls += SOUTH; }
    else { walls += (dir >> 1); }
  }
  
  if (rsensor.hasWall()) {
    if (dir == 8) { walls += 1; }
    else { walls += (dir << 1); }
  }
  
  if (flsensor.hasWall() && frsensor.hasWall()) {
    walls += 1;
  }
  return walls;
}
