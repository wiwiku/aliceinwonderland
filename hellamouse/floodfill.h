int initializeFloodfill(void);
int rowColToI(int row, int col);
void iToRowCol(int &row, int &col, int i);
int getWalls(int row, int col);
void addNewWalls(int row, int col, int walls);
int getFFScore(int row, int col);
void setFFScore(int row, int col, int score);
bool wallExists(int row, int col, int dir);
bool newWallExists(int newWalls, int dir);
int minNeighbor(int x, int y);
void getAdjacentCell(int &row, int &col, int dir);
int updateFloodfill(int row, int col, int newWalls);

// Queue accessor methods for debugging
boolean qEmpty();
int qPop();
void qPush(int i);
int qCount();
int qPeek();
