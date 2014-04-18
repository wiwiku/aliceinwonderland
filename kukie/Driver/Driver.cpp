//
//  Driver.cpp
//
//  Implements control for the motor driver.
//      HH = short brake
//      HL = forward
//      LH = backward
//      LL = stop
//  pwm takes a percentage (range: -100 ~ 100... negative means backward)
//
//  Created by William Ku on 4/15/14.
//
//

#include "Arduino.h"
#include "Driver.h"

Driver::Driver(int a1, int a2, int pwmapin, int b1, int b2, int pwmbpin) {
    pinMode(a1, OUTPUT);
    pinMode(a2, OUTPUT);
    pinMode(pwmapin, OUTPUT);
    pinMode(b1, OUTPUT);
    pinMode(b2, OUTPUT);
    pinMode(pwmbpin, OUTPUT);
    _a1 = a1; _a2 = a2; _pwmapin = pwmapin;
    _b1 = b1; _b2 = b2; _pwmbpin = pwmbpin;
    stop();
}

void Driver::setPWM(int a, int b) {
    int a1state, a2state, b1state, b2state;
    _pwma = min(max(a, -100), 100);
    _pwmb = min(max(b, -100), 100);

    if (_pwma > 0) {
        a1state = HIGH; a2state = LOW;
    } else if (_pwma < 0) {
        a1state = LOW; a2state = HIGH;
    } else {
        a1state = LOW; a2state = LOW;
    }
    if (_pwmb > 0) {
        b1state = HIGH; b2state = LOW;
    } else if (_pwmb < 0) {
        b1state = LOW; b2state = HIGH;
    } else {
        b1state = LOW; b2state = LOW;
    }

    digitalWrite(_a1, a1state);
    digitalWrite(_a2, a2state);
    analogWrite(_pwmapin, map(abs(_pwma), 0, 100, 0, 255));
    digitalWrite(_b1, b1state);
    digitalWrite(_b2, b2state);
    analogWrite(_pwmbpin, map(abs(_pwmb), 0, 100, 0, 255));
}

int Driver::getPWMA() {
    return _pwma;
}

int Driver::getPWMB() {
    return _pwmb;
}

// Stops the car, but car may coast.
void Driver::stop() {
    setPWM(0, 0);
}

// Brake is stronger than stop (which coasts).
void Driver::brake() {
    _pwma = 0;
    _pwmb = 0;
    digitalWrite(_a1, HIGH);
    digitalWrite(_a2, HIGH);
    analogWrite(_pwmapin, _pwma);
    digitalWrite(_b1, HIGH);
    digitalWrite(_b2, HIGH);
    analogWrite(_pwmbpin, _pwmb);
}