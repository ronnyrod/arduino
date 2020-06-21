/*

SimpleActuator.h - SimpleActuator library for manage a digital output
according to sensed value

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
#ifndef SimpleActuator_h
#define SimpleActuator_h

#include "Arduino.h"

class SimpleActuator
{
private:
    int pin;
    int minValue = 0;
    int maxValue = 1023;
    bool inRangeValue = true;
public:
    SimpleActuator(int, int, int, bool);
    void init();
    void control(int);
    int status();    
    ~SimpleActuator();
};
#endif