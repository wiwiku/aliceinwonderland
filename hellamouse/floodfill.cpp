#include "QueueList.h"
#include "core.h"
#include "memory.h"

int floodfillArr[LENGTH][LENGTH];

void initializeFloodfill() {
	for(int i = 0; i < LENGTH; i++) {
          for(int j = 0; j < LENGTH; j++) {
		floodfillArr[i][j] = UNDEFINED;
          }
	}
}

//Starting with an empty array, figure out what the new values are based on wall data in memory file		
//TODO: does this update anything? push values to memory
void updateFloodfill() {
	QueueList <String> queue;
	int i = 0;

	// Clean the array
	initializeFloodfill();
	
	// Push middle of maze onto queue.... middle has 4 squares
	queue.push(getFloodfillValue((LENGTH / 2), (LENGTH / 2))); //or maze val?
		
	while(!queue.isEmpty()) {
		int x = queue.pop();
		int z = x & 255; //bits
		
		int row, col;
		zToRowCol( z, row, col );
		
		int val = (int) (x >> 8) & 255;
		int curVal = floodfillArr[ z ];

		// Floodfill this cell
		if((curVal != UNDEFINED && curVal <= val) || z < 0 || z > AREA) { continue; }
		floodfillArr[ z ] = val;

		// Check the cell to the north
		if(!wallExists(row, col, 0)) {
			queue.push( rowColtoZ(row + 1, col) | ((val + 1) << 8) );
		}

		// Check the cel to the east
		if(!wallExists(row, col, 1)) {
			queue.push( rowColtoZ(row, col + 1) | ((val + 1) << 8) );
		}

		// Check the cell to the south
		if(!wallExists(row, col, 2)) {
			queue.push( rowColtoZ(row - 1, col) | ((val + 1) << 8) );
		}

		// Check the cell to the west
		if(!wallExists(row, col, 3)) {
			queue.push( rowColtoZ(row, col - 1) | ((val + 1) << 8) );
		}

		i++;
	}
}
