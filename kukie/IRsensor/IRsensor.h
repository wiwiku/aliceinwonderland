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
    IRsensor(int pin, float c1, float c2, float thresCm);
    float getVal();
    float getCm();
    boolean hasWall();
private:
    int _pin;
    float _c1, _c2, _thresCm;
};

#endif
