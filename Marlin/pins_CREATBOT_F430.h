/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * CreatBot pin assignments
 */

#ifndef __AVR_ATmega2560__
  #error "Oops!  Make sure you have 'Arduino Mega' selected from the 'Tools -> Boards' menu."
#endif

// #if E_STEPPERS > 3 || HOTENDS > 3
//   #error "CreatBot supports up to 3 hotends / E-steppers. Comment this line to keep going."
// #endif

#if E_STEPPERS > 2 || HOTENDS > 2
  #error "CreatBot F430 supports up to 2 hotends / E-steppers. Comment this line to keep going."
#endif

#define DEFAULT_MACHINE_NAME "CreatBot F430"
#define BOARD_NAME           "CreatBot F430"

//
// Servos
//
#define SERVO0_PIN  5

//
// Limit Switches
//
#define X_STOP_PIN  37
#define Y_STOP_PIN  35
#define Z_STOP_PIN  33

//
// Z Probe (when not Z_MIN_PIN)
//
#ifndef Z_MIN_PROBE_PIN
  #define Z_MIN_PROBE_PIN   32
#endif

//
// Steppers
//
#ifndef CREATBOT_MOTHERBOARD_GT95
  #define X_STEP_PIN          17
  #define X_DIR_PIN           16
  #define X_ENABLE_PIN        48

  #define Y_STEP_PIN          54
  #define Y_DIR_PIN           47
  #define Y_ENABLE_PIN        55

  #define Z_STEP_PIN          57
  #define Z_DIR_PIN           56
  #define Z_ENABLE_PIN        62 

  #define E0_STEP_PIN         23
  #define E0_DIR_PIN          22
  #define E0_ENABLE_PIN       24 

  #define E1_STEP_PIN         26
  #define E1_DIR_PIN          25
  #define E1_ENABLE_PIN       27

  // #define E2_STEP_PIN        29
  // #define E2_DIR_PIN         28
  // #define E2_ENABLE_PIN      39
#else
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

  // #define E2_STEP_PIN        29
  // #define E2_DIR_PIN         28
  // #define E2_ENABLE_PIN      39
#endif

//
// Temperature Sensors
//
#define TEMP_0_PIN          15
#define TEMP_1_PIN          14
#define TEMP_2_PIN          13
#define TEMP_BED_PIN        11
#define TEMP_CHAMBER_PIN    12

//
// Heaters / Fans
//
#define HEATER_0_PIN          2
#define HEATER_1_PIN          3
// #define HEATER_2_PIN          6
#define HEATER_BED_PIN        9
#define HEATER_CHAMBER_PIN	  6

#define FAN_PIN               7    // Part Cooling Fan
// #define FAN1_PIN              8    // Extruder Cooling Fan(s)
#define FAN3_PIN              10   // Chamber Vent Fan

//
// Misc. Functions
//
#define BEEPER_PIN    44
#define LED_PIN       13
// #define PS_ON_PIN     45
// #define KILL_PIN      46

//
// LCD / Controller
//
#define LCD_PINS_RS         19
#define LCD_PINS_ENABLE     42
#define LCD_PINS_D4         18
#define LCD_PINS_D5         38
#define LCD_PINS_D6         41
#define LCD_PINS_D7         40
#define BTN_EN1             11
#define BTN_EN2             12
#define BTN_ENC             43

//
// Power Supply Control
//
#define PS_OFF_PIN			    41
#define SUICIDE_PIN			    40
#define SUICIDE_PIN_STATE   HIGH  // State of pin that will trigger suicide
#define POWER_LOSS_PIN	    84
#define POWER_LOSS_STATE    LOW   // State of pin indicating power loss

//
// Filament Runout Sensor
//
#define FIL_RUNOUT_PIN		36 //FILAMENT_0_PIN
#define FIL_RUNOUT2_PIN		34 //FILAMENT_1_PIN
// #define FIL_RUNOUT3_PIN	  32 //FILAMENT_2_PIN

//
// USB Disk Reader
//
#define SD_DETECT_PIN   49
// #define SDSS            53
#define USB_SS		      53
#define USB_MOSI        51
#define USB_MISO	      50
#define USB_SCK		      52
#define USB_INT		      49
//#define USB_RST		      78
//#define USB_BZ	  	    5
