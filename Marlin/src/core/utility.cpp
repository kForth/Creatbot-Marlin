/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "utility.h"

#include "../MarlinCore.h"
#include "../module/temperature.h"

void safe_delay(millis_t ms) {
  while (ms > 50) {
    ms -= 50;
    delay(50);
    tempSensors.task();
  }
  delay(ms);
  tempSensors.task(); // This keeps us safe if too many small safe_delay() calls are made
}

// A delay to provide brittle hosts time to receive bytes
#if ENABLED(SERIAL_OVERRUN_PROTECTION)

  #include "../gcode/gcode.h" // for set_autoreport_paused

  void serial_delay(const millis_t ms) {
    const bool was = gcode.set_autoreport_paused(true);
    safe_delay(ms);
    gcode.set_autoreport_paused(was);
  }
#endif
