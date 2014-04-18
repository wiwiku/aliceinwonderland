//
//  Encoder.cpp
//
//  Implements an access class for encoder related functionalities.
//
//  Created by William Ku on 4/15/14.
//
//

#include "Arduino.h"
#include "Encoder.h"

Encoder::Encoder(int wheelDiameterMM) {
    _dia = wheelDiameterMM;
    _prevtime = micros();
}

unsigned long Encoder::getPrevTime() {
    return _prevtime;
}

// This should be called after the caller uses the getSpeed function.
unsigned long Encoder::setTime(unsigned long t) {
    _prevtime = t;
}

// Returns the speed using the amount of ticks between last time the function was called and now.
    float Encoder::getSpeed(unsigned long tMicros) {
    extern unsigned long edge;
    float speed = (edge - _prevedge) * (_dia / 10) * M_PI / 3 / 10 / ((float) (tMicros - _prevtime) / 1000000);
    _prevtime = tMicros;
    _prevedge = edge;
    return speed;
}
