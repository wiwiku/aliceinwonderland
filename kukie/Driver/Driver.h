//
//  Driver.h
//  
//
//  Created by William Ku on 4/15/14.
//
//

#ifndef _Driver_h
#define _Driver_h

#include "Arduino.h"

class Driver {
public:
    Driver(int a1, int a2, int pwmapin, int b1, int b2, int pwmbpin);
    void setPWM(int a, int b);
    int getPWMA();
    int getPWMB();
    void stop();
    void shortbrake();
private:
    int _a1, _a2, _pwmapin, _pwma, _b1, _b2, _pwmbpin, _pwmb;
};

#endif
