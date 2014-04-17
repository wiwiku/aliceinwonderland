//
//  IRsensor.cpp
//  
//
//  Created by William Ku on 4/14/14.
//
//

#include "Arduino.h"
#include "IRsensor.h"

IRsensor::IRsensor(int pin, float c1, float c2, float thresCm) {
    _pin = pin;
    _c1 = c1;
    _c2 = c2;
    _thresCm = thresCm;
}

float IRsensor::getVal() {
    return analogRead(_pin);
}

float IRsensor::getCm() {
    return pow((analogRead(_pin)/_c1), (1/_c2));
}

boolean IRsensor::hasWall() {
    return getCm() < _thresCm;
}