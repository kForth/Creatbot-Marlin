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

#include "../inc/MarlinConfig.h"

#define _UxGT(a) a

// Fallback if no language is set. DON'T CHANGE
#ifndef LCD_LANGUAGE
  #define LCD_LANGUAGE en
#endif

// For character-based LCD controllers (DISPLAY_CHARSET_HD44780)
#define JAPANESE 1
#define WESTERN  2
#define CYRILLIC 3

// NOTE: IF YOU CHANGE LANGUAGE FILES OR MERGE A FILE WITH CHANGES
//
//   ==> ALWAYS TRY TO COMPILE MARLIN WITH/WITHOUT "ULTIPANEL" / "ULTRA_LCD" / "SDSUPPORT" #define IN "Configuration.h"
//   ==> ALSO TRY ALL AVAILABLE LANGUAGE OPTIONS
// See also https://marlinfw.org/docs/development/lcd_language.html

// Languages
// an         Aragonese
// bg         Bulgarian
// ca         Catalan
// cz         Czech
// da         Danish
// de         German
// el         Greek (Greece)
// el_CY      Greek (Cyprus)
// en         English
// es         Spanish
// eu         Basque-Euskera
// fi         Finnish
// fr         French
// gl         Galician
// hr         Croatian
// hu         Hungarian
// it         Italian
// jp_kana    Japanese
// ko_KR      Korean (South Korea)
// nl         Dutch
// pl         Polish
// pt         Portuguese
// pt_br      Portuguese (Brazilian)
// ro         Romanian
// ru         Russian
// sk         Slovak
// sv         Swedish
// tr         Turkish
// uk         Ukrainian
// vi         Vietnamese
// zh_CN      Chinese (Simplified)
// zh_TW      Chinese (Traditional)

#ifdef DEFAULT_SOURCE_CODE_URL
  #undef  SOURCE_CODE_URL
  #define SOURCE_CODE_URL DEFAULT_SOURCE_CODE_URL
#endif

#ifdef CUSTOM_MACHINE_NAME
  #undef  MACHINE_NAME
  #define MACHINE_NAME CUSTOM_MACHINE_NAME
#elif defined(DEFAULT_MACHINE_NAME)
  #undef  MACHINE_NAME
  #define MACHINE_NAME DEFAULT_MACHINE_NAME
#endif

#ifndef MACHINE_UUID
  #define MACHINE_UUID DEFAULT_MACHINE_UUID
#endif

#define MARLIN_WEBSITE_URL "marlinfw.org"

//#if !defined(STRING_SPLASH_LINE3) && defined(WEBSITE_URL)
//  #define STRING_SPLASH_LINE3 WEBSITE_URL
//#endif

//
// Common Serial Console Messages
// Don't change these strings because serial hosts look for them.
//

#define STR_ENQUEUEING                      "enqueueing \""
#define STR_POWERUP                         "PowerUp"
#define STR_EXTERNAL_RESET                  " External Reset"
#define STR_BROWNOUT_RESET                  " Brown out Reset"
#define STR_WATCHDOG_RESET                  " Watchdog Reset"
#define STR_SOFTWARE_RESET                  " Software Reset"
#define STR_FREE_MEMORY                     " Free Memory: "
#define STR_OK                              "ok"
#define STR_WAIT                            "wait"
#define STR_DONE                            "Done."
#define STR_STATS                           "Stats: "
#define STR_ERR_LINE_NO                     "Line Number is not Last Line Number+1, Last Line: "
#define STR_ERR_CHECKSUM_MISMATCH           "checksum mismatch, Last Line: "
#define STR_ERR_NO_CHECKSUM                 "No Checksum with line number, Last Line: "
#define STR_WATCHDOG_FIRED                  "Watchdog timeout. Reset required."
#define STR_ERR_KILLED                      "Printer halted. kill() called!"
#define STR_ERR_STOPPED                     "Printer stopped due to errors. Fix the error and use M999 to restart. (Temperature is reset. Set it after restarting)"
#define STR_ERR_SERIAL_MISMATCH             "Serial status mismatch"
#define STR_BUSY_PROCESSING                 "busy: processing"
#define STR_BUSY_PAUSED_FOR_USER            "busy: paused for user"
#define STR_BUSY_PAUSED_FOR_INPUT           "busy: paused for input"
#define STR_RESEND                          "Resend: "
#define STR_UNKNOWN_COMMAND                 "Unknown command: \""
#define STR_FILAMENT                        "Filament"
#define STR_INVALID_EXTRUDER                "Invalid Extruder"

#define STR_ERR_PROTECTED_PIN               "Protected Pin"
#define STR_M119_REPORT                     "Reporting endstop status"
#define STR_ON                              "ON"
#define STR_OFF                             "OFF"
#define STR_DI_HIT                     "TRIGGERED"
#define STR_DI_OPEN                    "open"
#define STR_DUPLICATION_MODE                "Duplication mode: "
#define STR_SOFT_MIN                        "  Min: "
#define STR_SOFT_MAX                        "  Max: "

#define STR_DI_HIT_LIST                    "digital inputs triggered: "
#define STR_ERR_COLD_EXTRUDE_STOP           " cold extrusion prevented"
#define STR_ERR_LONG_EXTRUDE_STOP           " too long extrusion prevented"
#define STR_ERR_HOTEND_TOO_COLD             "Hotend too cold"
#define STR_ERR_EEPROM_WRITE                "Error writing to EEPROM!"

#define STR_FILAMENT_CHANGE_HEAT_LCD        "Press button to heat nozzle"
#define STR_FILAMENT_CHANGE_INSERT_LCD      "Insert filament and press button"
#define STR_FILAMENT_CHANGE_WAIT_LCD        "Press button to resume"
#define STR_FILAMENT_CHANGE_HEAT_M108       "Send M108 to heat nozzle"
#define STR_FILAMENT_CHANGE_INSERT_M108     "Insert filament and send M108"
#define STR_FILAMENT_CHANGE_WAIT_M108       "Send M108 to resume"

#define STR_STOP_PRE                        "!! STOP called because of "
#define STR_STOP_POST                       " error - restart with M999"
#define STR_STOP_BLTOUCH                    "BLTouch"
#define STR_STOP_UNHOMED                    "unhomed"
#define STR_KILL_PRE                        "!! KILL caused by "
#define STR_KILL_INACTIVE_TIME              "too much inactive time - current command: "
#define STR_KILL_BUTTON                     "KILL button/pin"

// temperature.cpp strings
#define STR_BIAS                            " bias: "
#define STR_D_COLON                         " d: "
#define STR_T_MIN                           " min: "
#define STR_T_MAX                           " max: "
#define STR_KU                              " Ku: "
#define STR_TU                              " Tu: "

// Debug
#define STR_DEBUG_PREFIX                    "DEBUG:"
#define STR_DEBUG_OFF                       "off"
#define STR_DEBUG_ECHO                      "ECHO"
#define STR_DEBUG_INFO                      "INFO"
#define STR_DEBUG_ERRORS                    "ERRORS"
#define STR_DEBUG_DRYRUN                    "DRYRUN"
#define STR_DEBUG_COMMUNICATION             "COMMUNICATION"
#define STR_DEBUG_DETAIL                    "DETAIL"

// Settings Report Strings
#define STR_FILAMENT_RUNOUT_SENSOR          "Filament runout sensor"
#define STR_TEMPERATURE_UNITS               "Temperature Units"
#define STR_USER_THERMISTORS                "User thermistors"

// Use superscripts, if possible. Evaluated at point of use.
#define SUPERSCRIPT_TWO   TERN(NOT_EXTENDED_ISO10646_1_5X7, "^2", "²")
#define SUPERSCRIPT_THREE TERN(NOT_EXTENDED_ISO10646_1_5X7, "^3", "³")
