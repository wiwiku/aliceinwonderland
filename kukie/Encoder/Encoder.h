//
//  Encoder.h
//  
//
//  Created by William Ku on 4/15/14.
//
//

#ifndef _Encoder_h
#define _Encoder_h

#include "Arduino.h"

class Encoder {
public:
    Encoder(int wheelDiameterMM);
    unsigned long getPrevTime();
    unsigned long setTime(unsigned long t);
    double getSpeed(unsigned long tMicros);
private:
    int _dia;
    unsigned long _prevtime;
};

#endif
