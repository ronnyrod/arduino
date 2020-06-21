/*

SimpleController.cpp - Implementation of SimpleController library - 

Control a analog variable using an analog sensor and a digital output

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
#include "SimpleController.h"

/**
 * Constructor - define sensor and actuators
 * @param _sensors: Array of sensors
 * @param _actuators: Array of actuators
 */
SimpleController::SimpleController(struct Sensor _sensors[], int _senSize, struct Actuator _actuators[], int _actSize) {
    senSize = _senSize;
    actSize = _actSize;
    sensors = _sensors;
    actuators = _actuators;
}
/**
 * Setup controller - Configures actuators as digital outputs
 **/
void SimpleController::init() {
    for(int i=0;i<actSize;i++) {
         pinMode(actuators[i].pin, OUTPUT);
    }   
}
/**
 * Update all sensors 
 * 
 **/
void SimpleController::updateSensors() {
    for(int i=0;i<senSize;i++) {
        sensors[i].val = analogRead(sensors[i].pin);
        if(sensors[i].val < sensors[i].min) {
            sensors[i].state = UN_RANGE;
        } else if(sensors[i].val >= sensors[i].min && sensors[i].val <= sensors[i].max ) {
            sensors[i].state = IN_RANGE;            
        } else {
            sensors[i].state = OV_RANGE;
        }
    }
}

/**
 * Read sensor(s) and modify actuator state according to range
 * @param mesuare_callback: external function to determinate final sensor state
 **/
void SimpleController::control(int(*mesuare_callback)(int,Sensor*)) {    
    updateSensors();
    lastSensorState = mesuare_callback(senSize,sensors);
    for(int i=0;i<actSize;i++) {
        digitalWrite(actuators[i].pin, actuators[i].states[lastSensorState]);         
    }     
}
/**
 * Read sensor(s) and modify actuator state according to range
 * Note: this fucntion uses last sensor state to determinate actuators states
 **/
void SimpleController::control() {
    updateSensors();
    lastSensorState = sensors[senSize-1].state;
    for(int i=0;i<actSize;i++) {
        digitalWrite(actuators[i].pin, actuators[i].states[lastSensorState]);         
    }     
}
/**
 * Last sensor state (UN_RANGE | IN_RANGE | OV_RANGE)
 **/
int SimpleController::state() {
    return lastSensorState;
}
/**
 * Destructor
 **/
SimpleController::~SimpleController() {

} 