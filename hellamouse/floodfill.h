int initializeFloodfill(boolean returnState);
int rowColToI(byte row, byte col);
void iToRowCol(byte &row, byte &col, int i);
byte getWalls(byte row, byte col);
void addNewWalls(byte row, byte col, byte walls);
byte getFFScore(byte row, byte col);
void setFFScore(byte row, byte col, byte score);
bool wallExists(byte row, byte col, byte dir);
bool newWallExists(byte newWalls, byte dir);
int minNeighbor(int x, int y);
void getAdjacentCell(byte &row, byte &col, byte dir);
void updateFloodfill(byte x, byte y, byte newWalls, boolean returnState);
void flipFFScore(boolean returnState);
void pushIfValid(byte x, byte y);
void readMazeFromMem();
void writeMazeToMem();

// Queue accessor methods for debugging
boolean qEmpty();
int qPop();
void qPush(int i);
int qCount();
int qPeek();

void calculateFFValues(byte x, byte y, boolean returnState);
