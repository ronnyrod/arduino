/*
DayLightController.cpp - DayLightController library - 

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

#include "DayLightController.h"

/**
 * Constructor
 */
DayLightController::DayLightController(bool _initialSunny, byte _maxDayLightHours){
    sunny = _initialSunny;
    maxDayLightHours = _maxDayLightHours;
}
/**
 * Update internal registers
 * @param onSunRise - Callback executes when sunny state starts
 * @param onSunSet - Callback executes when sunny state finishes
 **/
void DayLightController::update(void(*onSunRise)(void),void(*onSunSet)(void)) {
    unsigned long  currTime = millis();
    nDays = (byte) ((float)currTime / MIL_PER_DAY);
    nHours  = (byte)((float)(currTime - nDays * MIL_PER_DAY) / MIL_PER_HOUR);    
    if(sunny) {
        if(nHours>=maxDayLightHours) {
            sunny = false;
            onSunSet();            
        }
    } else {
        if(nHours <= 0) {
            sunny = true;
            onSunRise();
        }
    }
}
bool DayLightController::isSunny() {
    return sunny;
}
byte DayLightController::days() {
    return nDays;
}
byte DayLightController::hours() {
    return nHours;
}
/**
 * Destructor
 **/
DayLightController::~DayLightController()
{
}