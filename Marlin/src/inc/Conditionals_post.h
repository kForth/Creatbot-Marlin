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
 * Conditionals_post.h
 * Internal defines that depend on Configurations and Pins but are not user-editable.
 */

#ifdef GITHUB_ACTIONS
  // Extras for CI testing
#endif

// ADC
#ifdef BOARD_ADC_VREF
  #define ADC_VREF BOARD_ADC_VREF
#else
  #define ADC_VREF HAL_ADC_VREF
#endif

// Determine which type of 'EEPROM' is in use
#if ENABLED(EEPROM_SETTINGS)
  // EEPROM type may be defined by compile flags, configs, HALs, or pins
  // Set additional flags to let HALs choose in their Conditionals_post.h
  #if ANY(FLASH_EEPROM_EMULATION, SRAM_EEPROM_EMULATION, SDCARD_EEPROM_EMULATION, QSPI_EEPROM)
    #define USE_EMULATED_EEPROM 1
  #elif EITHER(I2C_EEPROM, SPI_EEPROM)
    #define USE_WIRED_EEPROM    1
  #elif ENABLED(IIC_BL24CXX_EEPROM)
    // nothing
  #else
    #define USE_FALLBACK_EEPROM 1
  #endif
#else
  #undef I2C_EEPROM
  #undef SPI_EEPROM
  #undef QSPI_EEPROM
  #undef SDCARD_EEPROM_EMULATION
  #undef SRAM_EEPROM_EMULATION
  #undef FLASH_EEPROM_EMULATION
  #undef IIC_BL24CXX_EEPROM
#endif

#ifdef TEENSYDUINO
  #undef max
  #define max(a,b) ((a)>(b)?(a):(b))
  #undef min
  #define min(a,b) ((a)<(b)?(a):(b))

  #undef NOT_A_PIN    // Override Teensyduino legacy CapSense define work-around
  #define NOT_A_PIN 0 // For PINS_DEBUGGING
#endif

/**
 * Host keep alive
 */
#ifndef DEFAULT_KEEPALIVE_INTERVAL
  #define DEFAULT_KEEPALIVE_INTERVAL 2
#endif

//
// Set USING_HW_SERIALn flags for used Serial Ports
//

// Flag the indexed hardware serial ports in use
#define CONF_SERIAL_IS(N) (defined(SERIAL_PORT) && SERIAL_PORT == N)

// Flag the named hardware serial ports in use
#define ANY_SERIAL_IS(N) CONF_SERIAL_IS(N)

#define HW_Serial    501
#define HW_Serial0   502
#define HW_Serial1   503
#define HW_Serial2   504
#define HW_Serial3   505
#define HW_Serial4   506
#define HW_Serial5   507
#define HW_Serial6   508
#define HW_MSerial0  509
#define HW_MSerial1  510
#define HW_MSerial2  511
#define HW_MSerial3  512
#define HW_MSerial4  513
#define HW_MSerial5  514
#define HW_MSerial6  515
#define HW_MSerial7  516
#define HW_MSerial8  517
#define HW_MSerial9  518
#define HW_MSerial10 519

#if CONF_SERIAL_IS(-1)
  #define USING_HW_SERIALUSB 1
#endif
#if ANY_SERIAL_IS(0)
  #define USING_HW_SERIAL0 1
#endif
#if ANY_SERIAL_IS(1)
  #define USING_HW_SERIAL1 1
#endif
#if ANY_SERIAL_IS(2)
  #define USING_HW_SERIAL2 1
#endif
#if ANY_SERIAL_IS(3)
  #define USING_HW_SERIAL3 1
#endif
#if ANY_SERIAL_IS(4)
  #define USING_HW_SERIAL4 1
#endif
#if ANY_SERIAL_IS(5)
  #define USING_HW_SERIAL5 1
#endif
#if ANY_SERIAL_IS(6)
  #define USING_HW_SERIAL6 1
#endif
#if ANY_SERIAL_IS(7)
  #define USING_HW_SERIAL7 1
#endif
#if ANY_SERIAL_IS(8)
  #define USING_HW_SERIAL8 1
#endif
#if ANY_SERIAL_IS(9)
  #define USING_HW_SERIAL9 1
#endif
#if ANY_SERIAL_IS(10)
  #define USING_HW_SERIAL10 1
#endif

#undef HW_Serial
#undef HW_Serial0
#undef HW_Serial1
#undef HW_Serial2
#undef HW_Serial3
#undef HW_Serial4
#undef HW_Serial5
#undef HW_Serial6
#undef HW_MSerial0
#undef HW_MSerial1
#undef HW_MSerial2
#undef HW_MSerial3
#undef HW_MSerial4
#undef HW_MSerial5
#undef HW_MSerial6
#undef HW_MSerial7
#undef HW_MSerial8
#undef HW_MSerial9
#undef HW_MSerial10

#undef _SERIAL_ID
#undef ANY_SERIAL_IS

//
// ADC Temp Sensors (Thermistor or Thermocouple with amplifier ADC interface)
//
#define HAS_ADC_TEST(P) (TEMP_SENSOR(P) != 0) //PIN_EXISTS(TEMP_##P) && 

// Sensors
#if PIN_EXISTS(FILWIDTH)
  #define HAS_FILAMENT_WIDTH_SENSOR 1
#endif

/**
 * MIN/MAX case light PWM scaling
 */
#if ENABLED(CASE_LIGHT_ENABLE)
  #ifndef CASE_LIGHT_MAX_PWM
    #define CASE_LIGHT_MAX_PWM 255
  #elif !WITHIN(CASE_LIGHT_MAX_PWM, 1, 255)
    #error "CASE_LIGHT_MAX_PWM must be a value from 1 to 255."
  #endif
#endif

#if ANY(DIGITAL_INPUT_PULLDOWNS, FIL_RUNOUT_PULLDOWN, \
    FIL_RUNOUT1_PULLDOWN, FIL_RUNOUT2_PULLDOWN, FIL_RUNOUT3_PULLDOWN, FIL_RUNOUT4_PULLDOWN, \
    FIL_RUNOUT5_PULLDOWN, FIL_RUNOUT6_PULLDOWN, FIL_RUNOUT7_PULLDOWN, FIL_RUNOUT8_PULLDOWN)
  #define USING_PULLDOWNS 1
#endif
