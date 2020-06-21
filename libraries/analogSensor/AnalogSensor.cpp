/*

AnalogSensor.cpp - AnalogSensor library for Analog sensor devices
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
#include "AnalogSensor.h"
/**
 * Contructor 
 * @param p - Analog pin
 */
AnalogSensor::AnalogSensor(int analogPin) {
    pin = analogPin;
    value = 0;
}
/**
 * Reads analog pin 
 * @return analog value
 */
int AnalogSensor::read(void) {
    value = analogRead(pin);
    return value;
}
/**
 * Executes a callback function after read sensor value
 **/
void AnalogSensor::control(callbackFunc callback) {
    //Read sensor
    read();
    //Executes callback control function 
    callback(lastValue());    
}
/**
 * Last read value from analog pin
 * @return read value
 */
int AnalogSensor::lastValue(void) {
    return value;
}
/**
 * Destructor
 */
AnalogSensor::~AnalogSensor() {   
}