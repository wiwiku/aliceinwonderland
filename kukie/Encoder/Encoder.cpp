//
//  Encoder.cpp
//  
//
//  Created by William Ku on 4/15/14.
//
//

#include "Arduino.h"
#include "Encoder.h"

Encoder::Encoder(int wheelDiameterMM) {
    _dia = wheelDiameterMM;
    _prevtime = 0;
}

unsigned long Encoder::getPrevTime() {
    return _prevtime;
}

unsigned long Encoder::setTime(unsigned long t) {
    _prevtime = t;
}

double Encoder::getSpeed(unsigned long tMicros) {
    extern unsigned long edge;
    double speed = edge * (_dia / 10) * M_PI / 3 / 10 / ((double) (tMicros - _prevtime) / 1000000);
    _prevtime = tMicros;
    edge = 0;
    return speed;
}
