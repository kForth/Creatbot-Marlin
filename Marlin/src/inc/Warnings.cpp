/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2021 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
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

/**
 * Warnings.cpp
 * Test configuration values and give warnings at compile-time.
 */

#include "MarlinConfig.h"

//
// Warnings! Located here so they will appear just once in the build output.
//

#if ENABLED(MARLIN_DEV_MODE)
  #warning "WARNING! Disable MARLIN_DEV_MODE for the final build!"
#endif

// Safety Features
#if DISABLED(USE_WATCHDOG)
  #warning "Safety Alert! Enable USE_WATCHDOG for the final build!"
#endif
#if ANY_TEMP_SENSOR_IS(998) || ANY_TEMP_SENSOR_IS(999)
  #warning "Warning! Don't use dummy thermistors (998/999) for final build!"
#endif

#if CANNOT_EMBED_CONFIGURATION
  #warning "Disabled CONFIGURATION_EMBEDDING because the target usually has less flash storage. Define FORCE_CONFIG_EMBED to override."
#endif

/**
 * Maple environment
 */
#ifdef __STM32F1__
  #warning "Maple build environments are deprecated. Please use a non-Maple build environment. Report issues to the Marlin Firmware project."
#endif
