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
 * External Sensor Board pin assignments
 */

#include "env_validate.h"

#define BOARD_INFO_NAME      "Arduino Mega 2560"
#define DEFAULT_MACHINE_NAME "External Sensor Board"
#define BOARD_WEBSITE_URL    "https://github.com/kforth/Marlin-ExternalSensors"

//
// Filament Runout Sensors
//
#define FIL_RUNOUT_PIN                         2

//
// Temperature Sensors
//
#define TEMP_0_PIN                            13
#define TEMP_1_PIN                            14

//
// Limit Switches
//
#define X_STOP_PIN                            18
#define Y_STOP_PIN                            19
#define Z_STOP_PIN                            20

//
// Heaters / Fans
//
#define HEATER_0_PIN                           4
#define HEATER_1_PIN                           5

//
// Steppers
//
#define X_STEP_PIN                            22
#define X_DIR_PIN                             23
#define X_ENABLE_PIN                          57

#define Y_STEP_PIN                            25
#define Y_DIR_PIN                             26
#define Y_ENABLE_PIN                          24

#define Z_STEP_PIN                            29
#define Z_DIR_PIN                             39
#define Z_ENABLE_PIN                          28

#define E0_STEP_PIN                           55
#define E0_DIR_PIN                            56
#define E0_ENABLE_PIN                         54

// #define E1_STEP_PIN                           58
// #define E1_DIR_PIN                            59
// #define E1_ENABLE_PIN                         57