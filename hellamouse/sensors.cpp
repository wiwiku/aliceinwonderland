#include "core.h"

void initializeSensors() {
  IRsensor lsensor(left, lc1, lc2, irThSide);
  IRsensor dlsensor(diagleft, dlc1, dlc2, irThDiag);
  IRsensor flsensor(frontleft, flc1, flc2, irThFront);
  IRsensor frsensor(frontright, frc1, frc2, irThFront);
  IRsensor drsensor(diagright, drc1, drc2, irThDiag);
  IRsensor rsensor(right, rc1, rc2, irThSide);
}

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
