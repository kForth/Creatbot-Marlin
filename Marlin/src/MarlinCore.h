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
#pragma once

#include "inc/MarlinConfig.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Pass true to keep steppers from timing out
void idle();

void kill(FSTR_P const message=nullptr);
void minkill();

// Global State of the firmware
enum MarlinState : uint8_t {
  MF_INITIALIZING = 0,
  MF_STOPPED,
  MF_KILLED,
  MF_RUNNING,
  MF_SD_COMPLETE,
  MF_PAUSED,
  MF_WAITING,
};

extern MarlinState marlin_state;
inline bool IsRunning() { return marlin_state >= MF_RUNNING; }
inline bool IsStopped() { return marlin_state == MF_STOPPED; }

bool pin_is_protected(const pin_t pin);

extern const char M112_KILL_STR[];
