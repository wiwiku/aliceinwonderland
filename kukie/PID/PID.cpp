//
//  PID.cpp
//  
//
//  Created by William Ku on 4/17/14.
//
//

#include "Arduino.h"
#include "PID.h"

PID::PID(float kp, float kd) {
    _kp = kp;
    _kd = kd;
    _prevep = 0;
}

float PID::getPIDterm(float lcm, float rcm) {
    float ep = lcm - rcm, pidterm;
    pidterm = _kp * ep + _kd * (ep - _prevep);
    _prevep = ep;
    return pidterm;
}