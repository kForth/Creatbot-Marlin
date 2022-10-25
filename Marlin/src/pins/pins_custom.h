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

#if NOT_TARGET(__AVR_ATmega2560__)
  #error "Oops! Select 'Arduino/Genuino Mega or Mega 2560' in 'Tools > Board.'"
#endif

#define BOARD_INFO_NAME "MegaCNCBreakout"

//
// MegaCNCBreakout Stepper Pins
//
#define STEPPER1_DIR                          35
#define STEPPER1_STEP                         37
#define STEPPER1_EN                           39
#define STEPPER1_CS                           43
#define STEPPER1_DIAG                         46

#define STEPPER2_DIR                          31
#define STEPPER2_STEP                         29
#define STEPPER2_EN                           33
#define STEPPER2_CS                           45
#define STEPPER2_DIAG                         44

#define STEPPER3_DIR                          23
#define STEPPER3_STEP                         25
#define STEPPER3_EN                           27
#define STEPPER3_CS                           41
#define STEPPER3_DIAG                         42

#define STEPPER4_DIR                          28
#define STEPPER4_STEP                         30
#define STEPPER4_EN                           32
#define STEPPER4_CS                           36
#define STEPPER4_DIAG                         40

#define STEPPER5_DIR                          22
#define STEPPER5_STEP                         24
#define STEPPER5_EN                           26
#define STEPPER5_CS                           34
#define STEPPER5_DIAG                         38

//
// MegaCNCBreakout Endstop Pins
//
#define ENDSTOP1                              54  // A0
#define ENDSTOP2                              55  // A1
#define ENDSTOP3                              56  // A2
#define ENDSTOP4                              57  // A3
#define ENDSTOP5                              58  // A4

//
// Limit Switches
//
#define X_MIN_PIN                             ENDSTOP1
#define Y_MIN_PIN                             ENDSTOP2
#define Y2_MIN_PIN                            ENDSTOP3
#define Z_MAX_PIN                             ENDSTOP4

//
// Z Probe
//
#define SERVO0_PIN                            3
#define Z_MIN_PROBE_PIN                       62  // A8
#ifdef Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN
  #undef Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN
#endif

//
// LEDs
//
#define RGB_LED_R_PIN                         7
#define RGB_LED_G_PIN                         6
#define RGB_LED_B_PIN                         5
#define RGB_LED_W_PIN                         4

//
// Joystick
//
#define JOY_EN_PIN                            69  // A15
#define JOY_X_PIN                             14  // A14
#define JOY_Y_PIN                             13  // A13
#define JOY_Z_PIN                             12  // A12

// Misc
#define BUZZER_PIN                            9
#define PS_ON_PIN                             48
#define FREEZE_PIN                            64  // A10
#define KILL_PIN                              65  // A11
#define SUICIDE_PIN                           2
#define SUICIDE_PIN_STATE                     HIGH

//
// M3/M4/M5 - Spindle/Laser Control
//
#define SPINDLE_LASER_PWM_PIN                 8   // Hardware PWM
#define SPINDLE_LASER_ENA_PIN                 49  // Pullup or pulldown!
#define SPINDLE_DIR_PIN                       47

//
// Calibration Probe
//
#define CALIBRATION_PIN                       63  // A9

//
// LCD Panel
//
// #define LCD_PINS_RS 19
// #define LCD_PINS_ENABLE 18
// #define LCD_PINS_D4 17
// #define LCD_PINS_D5 16
// #define LCD_PINS_D6 15
// #define LCD_PINS_D7 14
// #define BTN_EN1                               59  // A5
// #define BTN_EN2                               60  // A6
// #define BTN_ENC                               61  // A7

//
// Stepper Drivers
//
#define X_DIR_PIN                             STEPPER3_DIR
#define X_STEP_PIN                            STEPPER3_STEP
#define X_ENABLE_PIN                          STEPPER3_EN
#define X_CS_PIN                              STEPPER3_CS

#define Y_DIR_PIN                             STEPPER1_DIR
#define Y_STEP_PIN                            STEPPER1_STEP
#define Y_ENABLE_PIN                          STEPPER1_EN
#define Y_CS_PIN                              STEPPER1_CS

#define Y2_DIR_PIN                            STEPPER2_DIR
#define Y2_STEP_PIN                           STEPPER2_STEP
#define Y2_ENABLE_PIN                         STEPPER2_EN
#define Y2_CS_PIN                             STEPPER2_CS

#define Z_DIR_PIN                             STEPPER4_DIR
#define Z_STEP_PIN                            STEPPER4_STEP
#define Z_ENABLE_PIN                          STEPPER4_EN
#define Z_CS_PIN                              STEPPER4_CS