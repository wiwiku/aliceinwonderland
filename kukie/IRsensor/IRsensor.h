//
//  IRsensor.h
//  
//
//  Created by William Ku on 4/14/14.
//
//

#ifndef _IRsensor_h
#define _IRsensor_h

#include "Arduino.h"

class IRsensor {
public:
    IRsensor(int pin, double c1, double c2, double thresCm);
    double getVal();
    double getCm();
    boolean hasWall();
private:
    int _pin;
    double _c1, _c2, _thresCm;
};

#endif
