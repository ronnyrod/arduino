/*

DayLightController.h - DayLightController library - 

Control sun light hours

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
#ifndef DayLightController_h
#define DayLightController_h

#include "Arduino.h"

#define DAY_HOURS 24
#define MIL_PER_DAY 86400000
#define MIL_PER_HOUR 3600000
#define DEF_DAY_LIGHT_HOURS 8

class DayLightController {
private:
    bool sunny = true;
    byte maxDayLightHours = DEF_DAY_LIGHT_HOURS;
    byte nHours = 0;
    byte nDays = 0;
public:
    DayLightController(bool,byte);
    void update(void(*)(void),void(*)(void));
    bool isSunny();
    byte days();
    byte hours();    
    ~DayLightController();
};
#endif