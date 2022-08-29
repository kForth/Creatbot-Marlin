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

/**
 * Conditionals_LCD.h
 * Conditionals that need to be set before Configuration_adv.h or pins.h
 * Called after Configuration.h and Version.h
 */

/**
 * These defines must be simple constants for use in REPEAT, etc.
 *
 *  NUM_TEMP_SENSORS    - Number of Selectable Sensors
 *
 */
#ifdef NUM_TEMP_SENSORS
  #define HAS_TEMP_SENSORS 1
  #if NUM_TEMP_SENSORS > 1
    #define HAS_MULTI_TEMP_SENSORS 1
  #endif
#else
  #define NUM_TEMP_SENSORS 0
#endif

// Helper macros for hotend arrays
#define TEMP_SENSOR_LOOP() for (int8_t e = 0; e < NUM_TEMP_SENSORS; e++)
#define ARRAY_BY_TEMP_SENSOR(V...) ARRAY_N(NUM_TEMP_SENSORS, V)

/**
 * Fill in undefined Filament Sensor options
 */
#if ENABLED(FILAMENT_RUNOUT_SENSOR)
  #if NUM_RUNOUT_SENSORS >= 1
    #ifndef FIL_RUNOUT1_STATE
      #define FIL_RUNOUT1_STATE FIL_RUNOUT_STATE
    #endif
    #ifndef FIL_RUNOUT1_PULLUP
      #define FIL_RUNOUT1_PULLUP FIL_RUNOUT_PULLUP
    #endif
    #ifndef FIL_RUNOUT1_PULLDOWN
      #define FIL_RUNOUT1_PULLDOWN FIL_RUNOUT_PULLDOWN
    #endif
  #endif
  #if NUM_RUNOUT_SENSORS >= 2
    #ifndef FIL_RUNOUT2_STATE
      #define FIL_RUNOUT2_STATE FIL_RUNOUT_STATE
    #endif
    #ifndef FIL_RUNOUT2_PULLUP
      #define FIL_RUNOUT2_PULLUP FIL_RUNOUT_PULLUP
    #endif
    #ifndef FIL_RUNOUT2_PULLDOWN
      #define FIL_RUNOUT2_PULLDOWN FIL_RUNOUT_PULLDOWN
    #endif
  #endif
  #if NUM_RUNOUT_SENSORS >= 3
    #ifndef FIL_RUNOUT3_STATE
      #define FIL_RUNOUT3_STATE FIL_RUNOUT_STATE
    #endif
    #ifndef FIL_RUNOUT3_PULLUP
      #define FIL_RUNOUT3_PULLUP FIL_RUNOUT_PULLUP
    #endif
    #ifndef FIL_RUNOUT3_PULLDOWN
      #define FIL_RUNOUT3_PULLDOWN FIL_RUNOUT_PULLDOWN
    #endif
  #endif
  #if NUM_RUNOUT_SENSORS >= 4
    #ifndef FIL_RUNOUT4_STATE
      #define FIL_RUNOUT4_STATE FIL_RUNOUT_STATE
    #endif
    #ifndef FIL_RUNOUT4_PULLUP
      #define FIL_RUNOUT4_PULLUP FIL_RUNOUT_PULLUP
    #endif
    #ifndef FIL_RUNOUT4_PULLDOWN
      #define FIL_RUNOUT4_PULLDOWN FIL_RUNOUT_PULLDOWN
    #endif
  #endif
  #if NUM_RUNOUT_SENSORS >= 5
    #ifndef FIL_RUNOUT5_STATE
      #define FIL_RUNOUT5_STATE FIL_RUNOUT_STATE
    #endif
    #ifndef FIL_RUNOUT5_PULLUP
      #define FIL_RUNOUT5_PULLUP FIL_RUNOUT_PULLUP
    #endif
    #ifndef FIL_RUNOUT5_PULLDOWN
      #define FIL_RUNOUT5_PULLDOWN FIL_RUNOUT_PULLDOWN
    #endif
  #endif
  #if NUM_RUNOUT_SENSORS >= 6
    #ifndef FIL_RUNOUT6_STATE
      #define FIL_RUNOUT6_STATE FIL_RUNOUT_STATE
    #endif
    #ifndef FIL_RUNOUT6_PULLUP
      #define FIL_RUNOUT6_PULLUP FIL_RUNOUT_PULLUP
    #endif
    #ifndef FIL_RUNOUT6_PULLDOWN
      #define FIL_RUNOUT6_PULLDOWN FIL_RUNOUT_PULLDOWN
    #endif
  #endif
  #if NUM_RUNOUT_SENSORS >= 7
    #ifndef FIL_RUNOUT7_STATE
      #define FIL_RUNOUT7_STATE FIL_RUNOUT_STATE
    #endif
    #ifndef FIL_RUNOUT7_PULLUP
      #define FIL_RUNOUT7_PULLUP FIL_RUNOUT_PULLUP
    #endif
    #ifndef FIL_RUNOUT7_PULLDOWN
      #define FIL_RUNOUT7_PULLDOWN FIL_RUNOUT_PULLDOWN
    #endif
  #endif
  #if NUM_RUNOUT_SENSORS >= 8
    #ifndef FIL_RUNOUT8_STATE
      #define FIL_RUNOUT8_STATE FIL_RUNOUT_STATE
    #endif
    #ifndef FIL_RUNOUT8_PULLUP
      #define FIL_RUNOUT8_PULLUP FIL_RUNOUT_PULLUP
    #endif
    #ifndef FIL_RUNOUT8_PULLDOWN
      #define FIL_RUNOUT8_PULLDOWN FIL_RUNOUT_PULLDOWN
    #endif
  #endif
#endif // FILAMENT_RUNOUT_SENSOR

//
// Serial Port Info
//
#if defined(SERIAL_PORT)
  #define NUM_SERIAL 1
#endif
#if SERIAL_PORT == -1
  #define HAS_USB_SERIAL 1
#endif
#if SERIAL_PORT == -2
  #define HAS_ETHERNET 1
#endif

/**
 * LED Backlight INDEX END
 */
#if defined(NEOPIXEL_BKGD_INDEX_FIRST) && !defined(NEOPIXEL_BKGD_INDEX_LAST)
  #define NEOPIXEL_BKGD_INDEX_LAST NEOPIXEL_BKGD_INDEX_FIRST
#endif
