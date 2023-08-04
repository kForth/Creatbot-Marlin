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
#pragma once

#define BOARD_INFO_NAME "BTT OCTOPUS PRO V1.0"

#if !defined(Z_MIN_PROBE_PIN) && DISABLED(BLTOUCH)
  #define Z_MIN_PROBE_PIN                   PC5   // Probe (Proximity switch) port
#endif

#include "pins_BTT_OCTOPUS_V1_common.h"

// Use Heater 2 for the Heated Bed
#undef HEATER_BED_PIN
#define HEATER_BED_PIN HEATER_2_PIN

//
// Temperature Sensors
//
#if TEMP_SENSOR_IS_MAX_TC(0)  // Onboard Max31865 on SPI1
  #define TEMP_0_CS_PIN                     PF8   // Max31865 CS
  #define TEMP_0_SCK_PIN                    PA5
  #define TEMP_0_MISO_PIN                   PA6
  #define TEMP_0_MOSI_PIN                   PA7
  #define SOFTWARE_SPI                            // Max31865 and LCD SD share a set of SPIs, Set SD to softwareSPI for Max31865
  #define FORCE_SOFT_SPI
  #define MAX31865_SENSOR_WIRES_0 2               // (2-4) Number of wires for the probe connected to a MAX31865 board.
  #define BOGUS_TEMPERATURE_GRACE_PERIOD 60000
#else
  #define TEMP_0_PIN                        PF4   // T0
#endif

#if TEMP_SENSOR_IS_MAX_TC(1)  // MAX31865 Board on SPI3
  #undef TEMP_1_PIN
  #define TEMP_1_CS_PIN                     PA15  // MAX31865 CS
  #define TEMP_1_SCK_PIN                    PB3   // SPI3 SCK
  #define TEMP_1_MISO_PIN                   PB4   // SPI3 MISO
  #define TEMP_1_MOSI_PIN                   PB5   // SPI3 MOSI
  #define MAX31865_SENSOR_WIRES_1 2               // (2-4) Number of wires for the probe connected to a MAX31865 board.
#endif


// // Use T0 as ambient temp sensor
#define TEMP_BOARD_PIN                      PF6 // T2
