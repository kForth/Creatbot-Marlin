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
 * SanityCheck.h
 *
 * Test configuration values for errors at compile-time.
 */

/**
 * Require gcc 4.7 or newer (first included with Arduino 1.6.8) for C++11 features.
 */
#if __cplusplus < 201103L
  #error "Marlin requires C++11 support (gcc >= 4.7, Arduino IDE >= 1.6.8). Please upgrade your toolchain."
#endif

// Strings for sanity check messages
#define _NUM_AXES_STR NUM_AXIS_GANG("X ", "Y ", "Z ", "I ", "J ", "K ", "U ", "V ", "W ")
#define _LOGICAL_AXES_STR LOGICAL_AXIS_GANG("E ", "X ", "Y ", "Z ", "I ", "J ", "K ", "U ", "V ", "W ")

// Make sure macros aren't borked
#define TEST1
#define TEST2 1
#define TEST3 0
#define TEST4 true
#if ENABLED(TEST0) || !ENABLED(TEST2) || ENABLED(TEST3) || !ENABLED(TEST1, TEST2, TEST4)
  #error "ENABLED is borked!"
#endif
#if BOTH(TEST0, TEST1)
  #error "BOTH is borked!"
#endif
#if DISABLED(TEST1) || !DISABLED(TEST3) || DISABLED(TEST4) || DISABLED(TEST0, TEST1, TEST2, TEST4) || !DISABLED(TEST0, TEST3)
  #error "DISABLED is borked!"
#endif
#if !ANY(TEST1, TEST2, TEST3, TEST4) || ANY(TEST0, TEST3)
  #error "ANY is borked!"
#endif
#if NONE(TEST0, TEST1, TEST2, TEST4) || !NONE(TEST0, TEST3)
  #error "NONE is borked!"
#endif
#undef TEST1
#undef TEST2
#undef TEST3
#undef TEST4

/**
 * This is to alert you about non-matching versions of config files.
 *
 * You can edit the version tag in your old config files and try the build again.
 * The checks below will alert you about options that need to be changed, but they won't
 * tell you about new options that you might find useful. So it's recommended to transfer
 * your settings to new Configuration files matching your Marlin version as soon as possible.
 */
#define HEXIFY(H) _CAT(0x,H)
#if !defined(CONFIGURATION_H_VERSION) || HEXIFY(CONFIGURATION_H_VERSION) < HEXIFY(REQUIRED_CONFIGURATION_H_VERSION)
  #error "Your Configuration.h file is for an old version of Marlin. Downgrade Marlin or upgrade your Configuration.h."
#elif HEXIFY(CONFIGURATION_H_VERSION) > HEXIFY(REQUIRED_CONFIGURATION_H_VERSION)
  #error "Your Configuration.h file is for a newer version of Marlin. Upgrade Marlin or downgrade your Configuration.h."
#endif
#if !defined(CONFIGURATION_ADV_H_VERSION) || HEXIFY(CONFIGURATION_ADV_H_VERSION) < HEXIFY(REQUIRED_CONFIGURATION_ADV_H_VERSION)
  #error "Your Configuration_adv.h file is for an old version of Marlin. Downgrade Marlin or upgrade your Configuration_adv.h."
#elif HEXIFY(CONFIGURATION_ADV_H_VERSION) > HEXIFY(REQUIRED_CONFIGURATION_ADV_H_VERSION)
  #error "Your Configuration_adv.h file is for a newer version of Marlin. Upgrade Marlin or downgrade your Configuration_adv.h."
#endif
#undef HEXIFY

/**
 * Warnings for old configurations
 */
#ifndef MOTHERBOARD
  #error "MOTHERBOARD is required."
#endif

#ifndef SHORT_BUILD_VERSION
  #error "SHORT_BUILD_VERSION must be specified."
#elif !defined(DETAILED_BUILD_VERSION)
  #error "BUILD_VERSION must be specified."
#elif !defined(STRING_DISTRIBUTION_DATE)
  #error "STRING_DISTRIBUTION_DATE must be specified."
#elif !defined(PROTOCOL_VERSION)
  #error "PROTOCOL_VERSION must be specified."
#elif !defined(MACHINE_NAME)
  #error "MACHINE_NAME must be specified."
#elif !defined(SOURCE_CODE_URL)
  #error "SOURCE_CODE_URL must be specified."
#elif !defined(DEFAULT_MACHINE_UUID)
  #error "DEFAULT_MACHINE_UUID must be specified."
#elif !defined(WEBSITE_URL)
  #error "WEBSITE_URL must be specified."
#endif

/**
 * Serial
 */
#ifndef SERIAL_PORT
  #error "SERIAL_PORT must be defined."
#endif
#if !(defined(__AVR__) && defined(USBCON))
  #if ENABLED(SERIAL_XON_XOFF) && RX_BUFFER_SIZE < 1024
    #error "SERIAL_XON_XOFF requires RX_BUFFER_SIZE >= 1024 for reliable transfers without drops."
  #elif RX_BUFFER_SIZE && (RX_BUFFER_SIZE < 2 || !IS_POWER_OF_2(RX_BUFFER_SIZE))
    #error "RX_BUFFER_SIZE must be a power of 2 greater than 1."
  #elif TX_BUFFER_SIZE && (TX_BUFFER_SIZE < 2 || TX_BUFFER_SIZE > 256 || !IS_POWER_OF_2(TX_BUFFER_SIZE))
    #error "TX_BUFFER_SIZE must be 0 or a power of 2 between 1 and 256."
  #endif
#elif ANY(SERIAL_XON_XOFF, SERIAL_STATS_MAX_RX_QUEUED, SERIAL_STATS_DROPPED_RX)
  #error "SERIAL_XON_XOFF and SERIAL_STATS_* features not supported on USB-native AVR devices."
#endif

/**
 * Pull Up/Down
 */
#if BOTH(DIGITAL_INPUT_PULLUPS, DIGITAL_INPUT_PULLDOWNS)
  #error "Enable only one of DIGITAL_INPUT_PULLUPS or DIGITAL_INPUT_PULLDOWNS."
#elif BOTH(DIGITAL_INPUT_PULLUP_0, DIGITAL_INPUT_PULLDOWN_0)
  #error "Enable only one of DIGITAL_INPUT_PULLUP_0 or DIGITAL_INPUT_PULLDOWN_0."
#elif BOTH(DIGITAL_INPUT_PULLUP_1, DIGITAL_INPUT_PULLDOWN_1)
  #error "Enable only one of DIGITAL_INPUT_PULLUP_1 or DIGITAL_INPUT_PULLDOWN_1."
#elif BOTH(DIGITAL_INPUT_PULLUP_2, DIGITAL_INPUT_PULLDOWN_2)
  #error "Enable only one of DIGITAL_INPUT_PULLUP_2 or DIGITAL_INPUT_PULLDOWN_2."
#elif BOTH(DIGITAL_INPUT_PULLUP_3, DIGITAL_INPUT_PULLDOWN_3)
  #error "Enable only one of DIGITAL_INPUT_PULLUP_3 or DIGITAL_INPUT_PULLDOWN_3."
#elif BOTH(DIGITAL_INPUT_PULLUP_4, DIGITAL_INPUT_PULLDOWN_4)
  #error "Enable only one of DIGITAL_INPUT_PULLUP_4 or DIGITAL_INPUT_PULLDOWN_4."
#elif BOTH(DIGITAL_INPUT_PULLUP_5, DIGITAL_INPUT_PULLDOWN_5)
  #error "Enable only one of DIGITAL_INPUT_PULLUP_5 or DIGITAL_INPUT_PULLDOWN_5."
#elif BOTH(DIGITAL_INPUT_PULLUP_6, DIGITAL_INPUT_PULLDOWN_6)
  #error "Enable only one of DIGITAL_INPUT_PULLUP_6 or DIGITAL_INPUT_PULLDOWN_6."
#elif BOTH(DIGITAL_INPUT_PULLUP_7, DIGITAL_INPUT_PULLDOWN_7)
  #error "Enable only one of DIGITAL_INPUT_PULLUP_7 or DIGITAL_INPUT_PULLDOWN_7."
#endif
// TODO: Check for more than 8 digital inputs
#if BOTH(FIL_RUNOUT_PULLUP, FIL_RUNOUT_PULLDOWN)
  #error "Enable only one of FIL_RUNOUT_PULLUP or FIL_RUNOUT_PULLDOWN."
#endif

/**
 * I2C Position Encoders
 */
#if ENABLED(I2C_POSITION_ENCODERS)
  #if !BOTH(BABYSTEPPING, BABYSTEP_XY)
    #error "I2C_POSITION_ENCODERS requires BABYSTEPPING and BABYSTEP_XY."
  #elif !WITHIN(I2CPE_ENCODER_CNT, 1, 5)
    #error "I2CPE_ENCODER_CNT must be between 1 and 5."
  #endif
#endif

/**
 * Filament Runout needs one or more pins
 */
#if HAS_FILAMENT_SENSOR
  #if !PIN_EXISTS(FIL_RUNOUT)
    #error "FILAMENT_RUNOUT_SENSOR requires FIL_RUNOUT_PIN."
  #elif NUM_RUNOUT_SENSORS >= 8 && !PIN_EXISTS(FIL_RUNOUT8)
    #error "FIL_RUNOUT8_PIN is required with NUM_RUNOUT_SENSORS >= 8."
  #elif NUM_RUNOUT_SENSORS >= 7 && !PIN_EXISTS(FIL_RUNOUT7)
    #error "FIL_RUNOUT7_PIN is required with NUM_RUNOUT_SENSORS >= 7."
  #elif NUM_RUNOUT_SENSORS >= 6 && !PIN_EXISTS(FIL_RUNOUT6)
    #error "FIL_RUNOUT6_PIN is required with NUM_RUNOUT_SENSORS >= 6."
  #elif NUM_RUNOUT_SENSORS >= 5 && !PIN_EXISTS(FIL_RUNOUT5)
    #error "FIL_RUNOUT5_PIN is required with NUM_RUNOUT_SENSORS >= 5."
  #elif NUM_RUNOUT_SENSORS >= 4 && !PIN_EXISTS(FIL_RUNOUT4)
    #error "FIL_RUNOUT4_PIN is required with NUM_RUNOUT_SENSORS >= 4."
  #elif NUM_RUNOUT_SENSORS >= 3 && !PIN_EXISTS(FIL_RUNOUT3)
    #error "FIL_RUNOUT3_PIN is required with NUM_RUNOUT_SENSORS >= 3."
  #elif NUM_RUNOUT_SENSORS >= 2 && !PIN_EXISTS(FIL_RUNOUT2)
    #error "FIL_RUNOUT2_PIN is required with NUM_RUNOUT_SENSORS >= 2."
  #elif BOTH(FIL_RUNOUT1_PULLUP, FIL_RUNOUT1_PULLDOWN)
    #error "You can't enable FIL_RUNOUT1_PULLUP and FIL_RUNOUT1_PULLDOWN at the same time."
  #elif BOTH(FIL_RUNOUT2_PULLUP, FIL_RUNOUT2_PULLDOWN)
    #error "You can't enable FIL_RUNOUT2_PULLUP and FIL_RUNOUT2_PULLDOWN at the same time."
  #elif BOTH(FIL_RUNOUT3_PULLUP, FIL_RUNOUT3_PULLDOWN)
    #error "You can't enable FIL_RUNOUT3_PULLUP and FIL_RUNOUT3_PULLDOWN at the same time."
  #elif BOTH(FIL_RUNOUT4_PULLUP, FIL_RUNOUT4_PULLDOWN)
    #error "You can't enable FIL_RUNOUT4_PULLUP and FIL_RUNOUT4_PULLDOWN at the same time."
  #elif BOTH(FIL_RUNOUT5_PULLUP, FIL_RUNOUT5_PULLDOWN)
    #error "You can't enable FIL_RUNOUT5_PULLUP and FIL_RUNOUT5_PULLDOWN at the same time."
  #elif BOTH(FIL_RUNOUT6_PULLUP, FIL_RUNOUT6_PULLDOWN)
    #error "You can't enable FIL_RUNOUT6_PULLUP and FIL_RUNOUT6_PULLDOWN at the same time."
  #elif BOTH(FIL_RUNOUT7_PULLUP, FIL_RUNOUT7_PULLDOWN)
    #error "You can't enable FIL_RUNOUT7_PULLUP and FIL_RUNOUT7_PULLDOWN at the same time."
  #elif BOTH(FIL_RUNOUT8_PULLUP, FIL_RUNOUT8_PULLDOWN)
    #error "You can't enable FIL_RUNOUT8_PULLUP and FIL_RUNOUT8_PULLDOWN at the same time."
  #elif FILAMENT_RUNOUT_DISTANCE_MM < 0
    #error "FILAMENT_RUNOUT_DISTANCE_MM must be greater than or equal to zero."
  #endif
#endif

/**
 * Options only for NUM_TEMP_SENSORS > 1
 */
#if HAS_MULTI_TEMP_SENSORS
  #define MAX_TEMP_SENSORS  8
  static_assert(NUM_TEMP_SENSORS <= MAX_TEMP_SENSORS, "Marlin supports a maximum of " STRINGIFY(MAX_TEMP_SENSORS) " temp sensors.");
  #undef MAX_TEMP_SENSORS
#endif

/**
 * Filament Width Sensor
 */
#if ENABLED(FILAMENT_WIDTH_SENSOR)
  #if !HAS_FILAMENT_WIDTH_SENSOR
    #error "FILAMENT_WIDTH_SENSOR requires a FILWIDTH_PIN to be defined."
  #endif
#endif

/**
 * System Power Sensor
 */
#if ENABLED(POWER_MONITOR_CURRENT) && !PIN_EXISTS(POWER_MONITOR_CURRENT)
  #error "POWER_MONITOR_CURRENT requires a valid POWER_MONITOR_CURRENT_PIN."
#elif ENABLED(POWER_MONITOR_VOLTAGE) && !PIN_EXISTS(POWER_MONITOR_VOLTAGE)
  #error "POWER_MONITOR_VOLTAGE requires POWER_MONITOR_VOLTAGE_PIN to be defined."
#elif BOTH(POWER_MONITOR_CURRENT, POWER_MONITOR_VOLTAGE) && POWER_MONITOR_CURRENT_PIN == POWER_MONITOR_VOLTAGE_PIN
  #error "POWER_MONITOR_CURRENT_PIN and POWER_MONITOR_VOLTAGE_PIN must be different."
#endif

/**
 * Case Light requirements
 */
#if NEED_CASE_LIGHT_PIN
  #if !PIN_EXISTS(CASE_LIGHT)
    #error "CASE_LIGHT_ENABLE requires CASE_LIGHT_PIN, CASE_LIGHT_USE_NEOPIXEL, or CASE_LIGHT_USE_RGB_LED."
  #elif CASE_LIGHT_PIN == FAN_PIN
    #error "CASE_LIGHT_PIN conflicts with FAN_PIN. Resolve before continuing."
  #endif
#endif

/**
 * Required custom thermistor settings
 */
#if   TEMP_SENSOR_0_IS_CUSTOM && !(defined(HOTEND0_PULLUP_RESISTOR_OHMS) && defined(HOTEND0_RESISTANCE_25C_OHMS) && defined(HOTEND0_BETA))
  #error "TEMP_SENSOR_0 1000 requires HOTEND0_PULLUP_RESISTOR_OHMS, HOTEND0_RESISTANCE_25C_OHMS and HOTEND0_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_1_IS_CUSTOM && !(defined(HOTEND1_PULLUP_RESISTOR_OHMS) && defined(HOTEND1_RESISTANCE_25C_OHMS) && defined(HOTEND1_BETA))
  #error "TEMP_SENSOR_1 1000 requires HOTEND1_PULLUP_RESISTOR_OHMS, HOTEND1_RESISTANCE_25C_OHMS and HOTEND1_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_2_IS_CUSTOM && !(defined(HOTEND2_PULLUP_RESISTOR_OHMS) && defined(HOTEND2_RESISTANCE_25C_OHMS) && defined(HOTEND2_BETA))
  #error "TEMP_SENSOR_2 1000 requires HOTEND2_PULLUP_RESISTOR_OHMS, HOTEND2_RESISTANCE_25C_OHMS and HOTEND2_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_3_IS_CUSTOM && !(defined(HOTEND3_PULLUP_RESISTOR_OHMS) && defined(HOTEND3_RESISTANCE_25C_OHMS) && defined(HOTEND3_BETA))
  #error "TEMP_SENSOR_3 1000 requires HOTEND3_PULLUP_RESISTOR_OHMS, HOTEND3_RESISTANCE_25C_OHMS and HOTEND3_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_4_IS_CUSTOM && !(defined(HOTEND4_PULLUP_RESISTOR_OHMS) && defined(HOTEND4_RESISTANCE_25C_OHMS) && defined(HOTEND4_BETA))
  #error "TEMP_SENSOR_4 1000 requires HOTEND4_PULLUP_RESISTOR_OHMS, HOTEND4_RESISTANCE_25C_OHMS and HOTEND4_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_5_IS_CUSTOM && !(defined(HOTEND5_PULLUP_RESISTOR_OHMS) && defined(HOTEND5_RESISTANCE_25C_OHMS) && defined(HOTEND5_BETA))
  #error "TEMP_SENSOR_5 1000 requires HOTEND5_PULLUP_RESISTOR_OHMS, HOTEND5_RESISTANCE_25C_OHMS and HOTEND5_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_6_IS_CUSTOM && !(defined(HOTEND6_PULLUP_RESISTOR_OHMS) && defined(HOTEND6_RESISTANCE_25C_OHMS) && defined(HOTEND6_BETA))
  #error "TEMP_SENSOR_6 1000 requires HOTEND6_PULLUP_RESISTOR_OHMS, HOTEND6_RESISTANCE_25C_OHMS and HOTEND6_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_7_IS_CUSTOM && !(defined(HOTEND7_PULLUP_RESISTOR_OHMS) && defined(HOTEND7_RESISTANCE_25C_OHMS) && defined(HOTEND7_BETA))
  #error "TEMP_SENSOR_7 1000 requires HOTEND7_PULLUP_RESISTOR_OHMS, HOTEND7_RESISTANCE_25C_OHMS and HOTEND7_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_BED_IS_CUSTOM && !(defined(BED_PULLUP_RESISTOR_OHMS) && defined(BED_RESISTANCE_25C_OHMS) && defined(BED_BETA))
  #error "TEMP_SENSOR_BED 1000 requires BED_PULLUP_RESISTOR_OHMS, BED_RESISTANCE_25C_OHMS and BED_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_CHAMBER_IS_CUSTOM && !(defined(CHAMBER_PULLUP_RESISTOR_OHMS) && defined(CHAMBER_RESISTANCE_25C_OHMS) && defined(CHAMBER_BETA))
  #error "TEMP_SENSOR_CHAMBER 1000 requires CHAMBER_PULLUP_RESISTOR_OHMS, CHAMBER_RESISTANCE_25C_OHMS and CHAMBER_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_PROBE_IS_CUSTOM && !(defined(PROBE_PULLUP_RESISTOR_OHMS) && defined(PROBE_RESISTANCE_25C_OHMS) && defined(PROBE_BETA))
  #error "TEMP_SENSOR_PROBE 1000 requires PROBE_PULLUP_RESISTOR_OHMS, PROBE_RESISTANCE_25C_OHMS and PROBE_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_BOARD_IS_CUSTOM && !(defined(BOARD_PULLUP_RESISTOR_OHMS) && defined(BOARD_RESISTANCE_25C_OHMS) && defined(BOARD_BETA))
  #error "TEMP_SENSOR_BOARD 1000 requires BOARD_PULLUP_RESISTOR_OHMS, BOARD_RESISTANCE_25C_OHMS and BOARD_BETA in Configuration_adv.h."
#elif TEMP_SENSOR_REDUNDANT_IS_CUSTOM && !(defined(REDUNDANT_PULLUP_RESISTOR_OHMS) && defined(REDUNDANT_RESISTANCE_25C_OHMS) && defined(REDUNDANT_BETA))
  #error "TEMP_SENSOR_REDUNDANT 1000 requires REDUNDANT_PULLUP_RESISTOR_OHMS, REDUNDANT_RESISTANCE_25C_OHMS and REDUNDANT_BETA in Configuration_adv.h."
#endif

/**
 * Test Sensor & Heater pin combos.
 * Pins and Sensor IDs must be set for each heater
 */
// #if HAS_TEMP_SENSORS && !ANY_PIN(TEMP_0, TEMP_0_CS)
//   #error "TEMP_0_PIN or TEMP_0_CS_PIN not defined for this board."
// #elif HAS_TEMP_SENSORS && !HAS_TEMP_SENSORS && !TEMP_SENSOR_0_IS_DUMMY
//   #error "TEMP_0_PIN (required for TEMP_SENSOR_0) not defined for this board."
// #endif

#if HAS_MULTI_TEMP_SENSORS
  #if TEMP_SENSOR_1 == 0
    #error "TEMP_SENSOR_1 is required with 2 or more NUM_TEMP_SENSORS."
  #elif !ANY_PIN(TEMP_1)
    #error "TEMP_1_PIN not defined for this board."
  #endif
  #if NUM_TEMP_SENSORS > 2
    #if TEMP_SENSOR_2 == 0
      #error "TEMP_SENSOR_2 is required with 3 or more NUM_TEMP_SENSORS."
    #elif !PIN_EXISTS(TEMP_2)
      #error "TEMP_2_PIN not defined for this board."
    #endif
    #if NUM_TEMP_SENSORS > 3
      #if TEMP_SENSOR_3 == 0
        #error "TEMP_SENSOR_3 is required with 4 or more NUM_TEMP_SENSORS."
      #elif !PIN_EXISTS(TEMP_3)
        #error "TEMP_3_PIN not defined for this board."
      #endif
      #if NUM_TEMP_SENSORS > 4
        #if TEMP_SENSOR_4 == 0
          #error "TEMP_SENSOR_4 is required with 5 or more NUM_TEMP_SENSORS."
        #elif !PIN_EXISTS(TEMP_4)
          #error "TEMP_4_PIN not defined for this board."
        #endif
        #if NUM_TEMP_SENSORS > 5
          #if TEMP_SENSOR_5 == 0
            #error "TEMP_SENSOR_5 is required with 6 NUM_TEMP_SENSORS."
          #elif !PIN_EXISTS(TEMP_5)
            #error "TEMP_5_PIN not defined for this board."
          #endif
          #if NUM_TEMP_SENSORS > 6
            #if TEMP_SENSOR_6 == 0
              #error "TEMP_SENSOR_6 is required with 6 NUM_TEMP_SENSORS."
            #elif !PIN_EXISTS(TEMP_6)
              #error "TEMP_6_PIN not defined for this board."
            #endif
            #if NUM_TEMP_SENSORS > 7
              #if TEMP_SENSOR_7 == 0
                #error "TEMP_SENSOR_7 is required with 7 NUM_TEMP_SENSORS."
              #elif !PIN_EXISTS(TEMP_7)
                #error "TEMP_7_PIN not defined for this board."
              #endif
            #endif // NUM_TEMP_SENSORS > 7
          #endif // NUM_TEMP_SENSORS > 6
        #endif // NUM_TEMP_SENSORS > 5
      #endif // NUM_TEMP_SENSORS > 4
    #endif // NUM_TEMP_SENSORS > 3
  #endif // NUM_TEMP_SENSORS > 2
#endif // HAS_MULTI_TEMP_SENSORS

/**
 * Temperature status LEDs
 */
#if ENABLED(TEMP_STAT_LEDS) && !ANY_PIN(STAT_LED_RED, STAT_LED_BLUE)
  #error "TEMP_STAT_LEDS requires STAT_LED_RED_PIN or STAT_LED_BLUE_PIN, preferably both."
#endif

/**
 * Emergency Command Parser
 */
#if ENABLED(EMERGENCY_PARSER) && defined(__AVR__) && defined(USBCON)
  #error "EMERGENCY_PARSER does not work on boards with AT90USB processors (USBCON)."
#endif

/**
 * Software Reset options
 */
#if ENABLED(SOFT_RESET_VIA_SERIAL) && DISABLED(EMERGENCY_PARSER)
  #error "EMERGENCY_PARSER is required to activate SOFT_RESET_VIA_SERIAL."
#endif

// Reset reason for AVR
#if ENABLED(OPTIBOOT_RESET_REASON) && !defined(__AVR__)
  #error "OPTIBOOT_RESET_REASON only applies to AVR."
#endif

/**
 * RGB_LED Requirements
 */
#define _RGB_TEST (PINS_EXIST(RGB_LED_R, RGB_LED_G, RGB_LED_B))
#if ENABLED(PRINTER_EVENT_LEDS) && !HAS_COLOR_LEDS
  #error "PRINTER_EVENT_LEDS requires BLINKM, PCA9533, PCA9632, RGB_LED, RGBW_LED or NEOPIXEL_LED."
#elif ENABLED(RGB_LED)
  #if !_RGB_TEST
    #error "RGB_LED requires RGB_LED_R_PIN, RGB_LED_G_PIN, and RGB_LED_B_PIN."
  #elif ENABLED(RGBW_LED)
    #error "Please enable only one of RGB_LED and RGBW_LED."
  #endif
#elif ENABLED(RGBW_LED)
  #if !(_RGB_TEST && PIN_EXISTS(RGB_LED_W))
    #error "RGBW_LED requires RGB_LED_R_PIN, RGB_LED_G_PIN, RGB_LED_B_PIN, and RGB_LED_W_PIN."
  #endif
#endif
#undef _RGB_TEST

// NeoPixel requirements
#if ENABLED(NEOPIXEL_LED)
  #if !PIN_EXISTS(NEOPIXEL) || NEOPIXEL_PIXELS == 0
    #error "NEOPIXEL_LED requires NEOPIXEL_PIN and NEOPIXEL_PIXELS."
  #elif ENABLED(NEOPIXEL2_SEPARATE) && !(defined(NEOPIXEL2_TYPE) && PIN_EXISTS(NEOPIXEL2) && NEOPIXEL2_PIXELS > 0)
    #error "NEOPIXEL2_SEPARATE requires NEOPIXEL2_TYPE, NEOPIXEL2_PIN and NEOPIXEL2_PIXELS."
  #elif ENABLED(NEO2_COLOR_PRESETS) && DISABLED(NEOPIXEL2_SEPARATE)
    #error "NEO2_COLOR_PRESETS requires NEOPIXEL2_SEPARATE to be enabled."
  #endif
#endif

#if DISABLED(NO_COMPILE_TIME_PWM)
  #define _TEST_PWM(P) PWM_PIN(P)
#else
  #define _TEST_PWM(P) 1 // pass
#endif

/**
 * Make sure only one EEPROM type is enabled
 */
#if ENABLED(EEPROM_SETTINGS)
  #if 1 < 0 \
    + ENABLED(I2C_EEPROM) \
    + ENABLED(SPI_EEPROM) \
    + ENABLED(QSPI_EEPROM) \
    + ENABLED(SDCARD_EEPROM_EMULATION) \
    + ENABLED(FLASH_EEPROM_EMULATION) \
    + ENABLED(SRAM_EEPROM_EMULATION) \
    + ENABLED(IIC_BL24CXX_EEPROM)
    #error "Please select only one method of EEPROM Persistent Storage."
  #endif
#endif

/**
 * Some boards forbid the use of -1 Native USB
 */
#if ENABLED(BOARD_NO_NATIVE_USB)
  #undef BOARD_NO_NATIVE_USB
  #if SERIAL_PORT == -1
    #error "SERIAL_PORT is set to -1, but the MOTHERBOARD has no native USB support. Set SERIAL_PORT to a valid value for your board."
  #endif
#endif

#if ENABLED(CASE_LIGHT_USE_NEOPIXEL) && DISABLED(NEOPIXEL_LED)
  #error "CASE_LIGHT_USE_NEOPIXEL requires NEOPIXEL_LED."
#endif

/**
 * Sanity check for WIFI
 */
#if EITHER(ESP3D_WIFISUPPORT, WIFISUPPORT) && DISABLED(ARDUINO_ARCH_ESP32)
  #error "ESP3D_WIFISUPPORT or WIFISUPPORT requires an ESP32 MOTHERBOARD."
#endif

// Misc. Cleanup
#undef _TEST_PWM
#undef _NUM_AXES_STR
#undef _LOGICAL_AXES_STR

// JTAG support in the HAL
#if ENABLED(DISABLE_DEBUG) && !defined(JTAGSWD_DISABLE)
  #error "DISABLE_DEBUG is not supported for the selected MCU/Board."
#elif ENABLED(DISABLE_JTAG) && !defined(JTAG_DISABLE)
  #error "DISABLE_JTAG is not supported for the selected MCU/Board."
#endif

// Check requirements for upload.py
#if ENABLED(XFER_BUILD) && !BOTH(BINARY_FILE_TRANSFER, CUSTOM_FIRMWARE_UPLOAD)
  #error "BINARY_FILE_TRANSFER and CUSTOM_FIRMWARE_UPLOAD are required for custom upload."
#endif
