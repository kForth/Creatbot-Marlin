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
 * Conditionals_adv.h
 * Conditionals set before pins.h and which depend on Configuration_adv.h.
 */

/**
 * Temperature Sensors; define what sensor(s) we have.
 */

// Temperature sensor IDs
#define HID_E0         0
#define HID_E1         1
#define HID_E2         2
#define HID_E3         3
#define HID_E4         4
#define HID_E5         5
#define HID_E6         6
#define HID_E7         7

#if ANY_TEMP_SENSOR_IS(1000)
  #define HAS_USER_THERMISTORS 1
#endif

#if ENABLED(FILAMENT_RUNOUT_SENSOR)
  #define HAS_FILAMENT_SENSOR 1
  #if NUM_RUNOUT_SENSORS > 1
    #define MULTI_FILAMENT_SENSOR 1
  #endif
  #ifdef FILAMENT_RUNOUT_DISTANCE_MM
    #define HAS_FILAMENT_RUNOUT_DISTANCE 1
  #endif
#endif

#if ANY(BLINKM, RGB_LED, RGBW_LED, PCA9632, PCA9533, NEOPIXEL_LED)
  #define HAS_COLOR_LEDS 1
#else
  #undef LED_POWEROFF_TIMEOUT
#endif

#if !defined(__AVR__) || !defined(USBCON)
  // Define constants and variables for buffering serial data.
  // Use only 0 or powers of 2 greater than 1
  // : [0, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, ...]
  #ifndef RX_BUFFER_SIZE
    #define RX_BUFFER_SIZE 128
  #endif
  // 256 is the max TX buffer limit due to uint8_t head and tail
  // : [0, 4, 8, 16, 32, 64, 128, 256]
  #ifndef TX_BUFFER_SIZE
    #define TX_BUFFER_SIZE 32
  #endif
#else
  // SERIAL_XON_XOFF not supported on USB-native devices
  #undef SERIAL_XON_XOFF
#endif

#if ENABLED(HOST_ACTION_COMMANDS)
  #ifndef ACTION_ON_PAUSE
    #define ACTION_ON_PAUSE   "pause"
  #endif
  #ifndef ACTION_ON_PAUSED
    #define ACTION_ON_PAUSED  "paused"
  #endif
  #ifndef ACTION_ON_RESUME
    #define ACTION_ON_RESUME  "resume"
  #endif
  #ifndef ACTION_ON_RESUMED
    #define ACTION_ON_RESUMED "resumed"
  #endif
  #ifndef ACTION_ON_CANCEL
    #define ACTION_ON_CANCEL  "cancel"
  #endif
  #ifndef ACTION_ON_START
    #define ACTION_ON_START   "start"
  #endif
  #ifndef ACTION_ON_KILL
    #define ACTION_ON_KILL    "poweroff"
  #endif
  #ifndef SHUTDOWN_ACTION
    #define SHUTDOWN_ACTION   "shutdown"
  #endif
  #if HAS_FILAMENT_SENSOR
    #ifndef ACTION_ON_FILAMENT_RUNOUT
      #define ACTION_ON_FILAMENT_RUNOUT "filament_runout"
    #endif
    #ifndef ACTION_REASON_ON_FILAMENT_RUNOUT
      #define ACTION_REASON_ON_FILAMENT_RUNOUT "filament_runout"
    #endif
  #endif
#endif

// Power Monitor sensors
#if EITHER(POWER_MONITOR_CURRENT, POWER_MONITOR_VOLTAGE)
  #define HAS_POWER_MONITOR 1
  #if ENABLED(POWER_MONITOR_CURRENT) && (ENABLED(POWER_MONITOR_VOLTAGE) || defined(POWER_MONITOR_FIXED_VOLTAGE))
    #define HAS_POWER_MONITOR_WATTS 1
  #endif
#endif

// Flag if an EEPROM type is pre-selected
#if ENABLED(EEPROM_SETTINGS) && NONE(I2C_EEPROM, SPI_EEPROM, QSPI_EEPROM, FLASH_EEPROM_EMULATION, SRAM_EEPROM_EMULATION, SDCARD_EEPROM_EMULATION)
  #define NO_EEPROM_SELECTED 1
#endif

// Flag whether hex_print.cpp is used
#if ANY(AUTO_BED_LEVELING_UBL, M100_FREE_MEMORY_WATCHER, DEBUG_GCODE_PARSER, TMC_DEBUG, MARLIN_DEV_MODE, DEBUG_CARDREADER)
  #define NEED_HEX_PRINT 1
#endif

// Flags for Case Light having a color property or a single pin
#if ENABLED(CASE_LIGHT_ENABLE)
  #if EITHER(CASE_LIGHT_USE_NEOPIXEL, CASE_LIGHT_USE_RGB_LED)
    #define CASE_LIGHT_IS_COLOR_LED 1
  #else
    #define NEED_CASE_LIGHT_PIN 1
  #endif
#endif

// Flags for Case Light having a brightness property
#if ENABLED(CASE_LIGHT_ENABLE) && (NONE(CASE_LIGHT_NO_BRIGHTNESS, CASE_LIGHT_IS_COLOR_LED) || ENABLED(CASE_LIGHT_USE_NEOPIXEL))
  #define CASELIGHT_USES_BRIGHTNESS 1
#endif

// AVR are (usually) too limited in resources to store the configuration into the binary
#if ENABLED(CONFIGURATION_EMBEDDING) && !defined(FORCE_CONFIG_EMBED) && (defined(__AVR__) || DISABLED(SDSUPPORT) || EITHER(SDCARD_READONLY, DISABLE_M503))
  #undef CONFIGURATION_EMBEDDING
  #define CANNOT_EMBED_CONFIGURATION defined(__AVR__)
#endif
