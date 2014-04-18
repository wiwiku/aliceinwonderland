const byte DEBUG = 1;
const byte LENGTH = 16;

const byte NORTH = 1;
const byte EAST = 2;
const byte SOUTH = 4;
const byte WEST = 8;
const byte ALLWALLS = 15;
const byte UNDEFINED = 255; //we won't have a spiral maze. if this val is > 255, better change the flip method

