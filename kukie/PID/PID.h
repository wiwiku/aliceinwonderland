//
//  PID.h
//  
//
//  Created by William Ku on 4/17/14.
//
//

#ifndef _PID_h
#define _PID_h

#include "Arduino.h"

class PID {
public:
    PID(float kp, float kd);
    float getPIDterm(float lcm, float rcm);
private:
    float _kp, _kd, _prevep;
};


#endif
