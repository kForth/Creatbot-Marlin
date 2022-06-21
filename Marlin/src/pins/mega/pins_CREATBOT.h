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
 * CreatBot pin assignments
 */

#include "env_validate.h"

#if E_STEPPERS > 3 || HOTENDS > 3
  #error "CreatBot supports up to 3 hotends / E-steppers. Comment this line to keep going."
#endif

#define BOARD_INFO_NAME "CreatBot"

//
// Servos
//
#define SERVO0_PIN         5

//
// Limit Switches
//

#if MODEL == PEEK300
	#define X_STOP_PIN          35
	#define Y_STOP_PIN          37
	#define Z_STOP_PIN          33

	#undef Y_HOME_DIR
	#define Y_HOME_DIR 1
	#define USE_YMAX_PLUG
#else
	#define X_STOP_PIN          37
	#define Y_STOP_PIN          35
	#define Z_STOP_PIN          33
#endif

//
// Z Probe (when not Z_MIN_PIN)
//
#ifndef Z_MIN_PROBE_PIN
  #define Z_MIN_PROBE_PIN  32
#endif

//
// Steppers
//
#ifdef BOARD_VERSION_GT_V95
  #define X_STEP_PIN         16
  #define X_DIR_PIN          17
  #define X_ENABLE_PIN       48

  #define Y_STEP_PIN         47
  #define Y_DIR_PIN          54
  #define Y_ENABLE_PIN       55

  #define Z_STEP_PIN         56
  #define Z_DIR_PIN          57
  #define Z_ENABLE_PIN       62

  #define E0_STEP_PIN        22
  #define E0_DIR_PIN         23
  #define E0_ENABLE_PIN      24

  #define E1_STEP_PIN        25
  #define E1_DIR_PIN         26
  #define E1_ENABLE_PIN      27

  #define E2_STEP_PIN        28
  #define E2_DIR_PIN         29
  #define E2_ENABLE_PIN      39
#else // BOARD_VERSION_LT_V95
  #define X_STEP_PIN         17
  #define X_DIR_PIN          16
  #define X_ENABLE_PIN       48

  #define Y_STEP_PIN         54
  #define Y_DIR_PIN          47
  #define Y_ENABLE_PIN       55

  #define Z_STEP_PIN         57
  #define Z_DIR_PIN          56
  #define Z_ENABLE_PIN       62

  #define E0_STEP_PIN        23
  #define E0_DIR_PIN         22
  #define E0_ENABLE_PIN      24

  #define E1_STEP_PIN        26
  #define E1_DIR_PIN         25
  #define E1_ENABLE_PIN      27

  #define E2_STEP_PIN        29
  #define E2_DIR_PIN         28
  #define E2_ENABLE_PIN      39
#endif //BOARD_VERSION_GT_V95

//
// Temperature Sensors
//
#define TEMP_0_PIN       15
#define TEMP_1_PIN       14
#define TEMP_2_PIN       13
#define TEMP_BED_PIN     11

//
// Heaters / Fans
//
#define HEATER_0_PIN        2
#define HEATER_1_PIN        3
#define HEATER_2_PIN        6 // Also used by HEATER_CHAMBER_PIN
#define HEATER_3_PIN        8
#define HEATER_BED_PIN      9

#define FAN_PIN             7
// #define FAN1_PIN            8

#ifdef HAS_CHAMBER_HEATER
	#define TEMP_CHAMBER_PIN			12
	#define HEATER_CHAMBER_PIN		6
#endif

#ifdef HAS_CHAMBER_FAN
  #define FAN3_PIN          10 //AIR_FAN_PIN
  #define CHAMBER_FAN_INDEX 3
#endif

//
// Misc. Functions
//
#define SDSS               53
#define LED_PIN     13
#define PS_ON_PIN          45
#define KILL_PIN           46
// #define KILL_PIN_STATE LOW // LOW or HIGH?

//
// LCD / Controller
//
#define SD_DETECT_PIN      49
#define BEEPER_PIN         44
#define LCD_PINS_RS        19
#define LCD_PINS_ENABLE    42
#define LCD_PINS_D4        18
#define LCD_PINS_D5        38
#define LCD_PINS_D6        41
#define LCD_PINS_D7        40
#define BTN_EN1            11
#define BTN_EN2            12
#define BTN_ENC            43

#ifdef POWER_MANAGEMENT
	#define POWER_LOSS_PIN	 41 //PS_OFF_PIN

	#define SUICIDE_PIN			40
  #define SUICIDE_PIN_STATE HIGH
#endif

#ifdef MY_KEYPAD
	#define SHIFT_CLK			82
	#define SHIFT_LD			83
	#define SHIFT_OUT			38
#endif

#define FIL_RUNOUT_PIN		36
#define FIL_RUNOUT2_PIN		34
// #define FIL_RUNOUT3_PIN		32

#ifdef ACCIDENT_DETECT
	#define ACCIDENT_PIN		84
#endif

#ifdef COLOR_LED
	#define LED_RED_PIN			78
	#define LED_GREEN_PIN		80
	#define LED_BLUE_PIN		79
#endif

#ifdef UDISKSUPPORT
	#define USB_SS		53
	#define USB_MOSI	51
	#define USB_MISO	50
	#define USB_SCK		52
	#define USB_INT		49
	//#define USB_RST		78
	//#define USB_BZ		5
#endif

#ifdef WIFI_SUPPORT
	#define ESP8266_RST		42
#endif
 
// #define DS_DATA_PIN 64 // For old REG_USE_HARDWARE
