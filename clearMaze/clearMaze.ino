#include <EEPROM.h>
#define LENGTH 16
/* Calculates the index if the 2D array were flattened */
int rowColToI(int row, int col) {
  return row + (LENGTH * col);
}

void setup() {
  Serial.begin(9600);
  for (int mazei = 0; mazei < LENGTH; mazei++) {
    for (int mazej = 0; mazej < LENGTH; mazej++) 
      EEPROM.write(rowColToI(mazei, mazej), 0);
  }
  EEPROM.write(256, 0);
  Serial.println("Done clearing");
}

void loop() {
}

