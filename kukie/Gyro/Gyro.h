Gyroscope gyro;

#define LOW_FILTER 3
#define UPDATE_GYRO 10 //in milliseconds
#define GYRO_CALIBRATION 2.61
#define SAMPLE_RATE 10
#define LEFT 20
#define RIGHT 15
#define SLOWMAX 30
#define FASTMAX 100
#define SLOWMIN 5
#define FASTMIN 5

#define MARGIN_ERROR 0
#define ZERO_MARGIN 1

#define GYROK_SLOW .2
#define GYROKD_SLOW .1
#define GYROK_FAST .8
#define GYROKD_FAST .5
#define OFF_SLOW 4
#define RATIO_SLOW 4
#define OFF_FAST 16
#define RATIO_FAST 8

float gyroK = .2; //2
float gyroKd = .1; //1.4

#define SLOW 0
#define FAST 1
#define UTURN 2

int turnMode = FAST;
int maxPWM = 0;
int minPWM = 0;
int gyroOffset = 0;
int turnRatio = 0;

int xOff, yOff, zOff = 0;
long previousTime = 0;
long degreesChanged = 0;
int actualDegreesChanged = 0;
volatile int accumulatedDegrees = 0;
int prevZRate = 0;


int errorDegree = 0;
int previousDegreeError = 0;
int refDegree = 90;
int pwmLeft = 0;
int pwmRight = 0;


int gyro_offset = 0;
int zRate = 0;
int LEDPIN = 13;
int SWITCH = 10;