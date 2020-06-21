/*

SimpleController.h - SimpleController library - 

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
#ifndef SimpleController_h
#define SimpleController_h

#include "Arduino.h"

#define UN_RANGE 0
#define IN_RANGE 1
#define OV_RANGE 2


struct Actuator
{
    int pin;
    bool states[3];
};
struct Sensor
{
    int pin;
    int min;
    int max;
    int val;
    byte state;
};

typedef int(*mesuare_callback)(int, Sensor*);

class SimpleController
{
private:
    struct Sensor* sensors;
    struct Actuator* actuators;
    int senSize = 0;
    int actSize = 0;
    int lastSensorState;    
    void updateSensors();
public:
    SimpleController(struct Sensor[],int, struct Actuator[],int);
    void control();
    void control(mesuare_callback);
    int state();    
    void init();
    ~SimpleController();
};
#endif