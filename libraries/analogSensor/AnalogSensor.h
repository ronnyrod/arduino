/*

AnalogSensor.h - AnalogSensor library for Analog sensor devices
Copyright (c) 2020 ronnyrod.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef AnalogSensor_h
#define AnalogSensor_h

#include "Arduino.h"

typedef void (* callbackFunc)(int,int);

class AnalogSensor
{
private:
    int pin;
    int value;
    int minValue = 0;
    int maxValue = 1023;
public:
    AnalogSensor(int);
    int lastValue(void);
    void range(int, int);
    int read(void);
    int inRange(void);
    void control(callbackFunc);
    ~AnalogSensor();
};
#endif