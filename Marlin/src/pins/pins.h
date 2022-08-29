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

// Test the target within the included pins file
#ifdef __MARLIN_DEPS__
  #define NOT_TARGET(V...) 0
#else
  #define NOT_TARGET(V...) NONE(V)
#endif

//
// ATmega2560
//

#if MB(ARDUINO_MEGA2560)
  #include "mega/pins_ARDUINO_MEGA2560.h"      // ATmega2560                             env:mega2560

//
// Custom board (with custom PIO env)
//

#elif MB(CUSTOM)
  #include "pins_custom.h"                      //                                        env:custom

#endif

//
// Post-process pins according to configured settings
//
#include "pins_postprocess.h"
