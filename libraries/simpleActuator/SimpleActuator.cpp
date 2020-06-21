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

#include "SimpleActuator.h"

/**
 * Constructor  
 * @param digitalPin - digital pin
 * @param min - min limit 
 * @param max - max limit
 * @param inRange - value to write in case of value in range
 */
SimpleActuator::SimpleActuator(int digitalPin, int min, int max, bool inRange) {
    pin = digitalPin;
    minValue = min;
    maxValue = max;
    inRangeValue = inRange;
}
/**
 * Set up digital pin to work as output.  Call this method at setup 
 * 
 **/
void SimpleActuator::init() {
    pinMode(pin, OUTPUT);
}
/**
 * Manage digital pin according to value and range
 **/
void SimpleActuator::control(int value) {
    if(value>=minValue && value<=maxValue) {
        digitalWrite(pin, inRangeValue);
    } else {
        digitalWrite(pin, !inRangeValue);
    }
}
/**
 * Get actuator status (pin value)
 **/
int SimpleActuator::status() {
    return digitalRead(pin);
}
/**
 * Destructor
 **/
SimpleActuator::~SimpleActuator() {
}