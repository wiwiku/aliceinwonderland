//
//  IRsensor.cpp
//  
//
//  Created by William Ku on 4/14/14.
//
//

#include "Arduino.h"
#include "IRsensor.h"

IRsensor::IRsensor(int pin, double c1, double c2, double thresCm) {
    _pin = pin;
    _c1 = c1;
    _c2 = c2;
    _thresCm = thresCm;
}

double IRsensor::getVal() {
    return analogRead(_pin);
}

double IRsensor::getCm() {
    return pow((analogRead(_pin)/_c1), (1/_c2));
}

boolean IRsensor::hasWall() {
    return getCm() < _thresCm;
}