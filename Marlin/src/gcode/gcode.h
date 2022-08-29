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
 * -----------------
 * G-Codes in Marlin
 * -----------------
 *
 * Helpful G-code references:
 *  - https://marlinfw.org/meta/gcode
 *  - https://reprap.org/wiki/G-code
 *  - https://linuxcnc.org/docs/html/gcode.html
 *
 * Help to document Marlin's G-codes online:
 *  - https://github.com/MarlinFirmware/MarlinDocumentation
 *
 * -----------------
 *
 * "G" Codes
 *
 * G4   - Dwell S<seconds> or P<milliseconds>
 * G20  - Set input units to inches (Requires INCH_MODE_SUPPORT)
 * G21  - Set input units to millimeters (Requires INCH_MODE_SUPPORT)
 *
 * "M" Codes
 *
 * M42  - Change pin status via G-code: M42 P<pin> S<value>. LED pin assumed if P is omitted. (Requires DIRECT_PIN_CONTROL)
 * M43  - Display pin status, watch pins for changes, watch digitalInputs & toggle LED, toggle pins (Requires PINS_DEBUGGING)
 *
 * M100 - Watch Free Memory (for debugging) (Requires M100_FREE_MEMORY_WATCHER)
 *
 * M105 - Report current temperatures.
 *
 * M111 - Set debug flags: "M111 S<flagbits>". See flag bits defined in enum.h.
 * M112 - Full Shutdown.
 *
 * M113 - Get or set the timeout interval for Host Keepalive "busy" messages. (Requires HOST_KEEPALIVE_FEATURE)
 * M115 - Report capabilities. (Extended capabilities requires EXTENDED_CAPABILITIES_REPORT)
 * M118 - Display a message in the host console.
 *
 * M119 - Report digitalInputs status.
 *
 * M149 - Set temperature units. (Requires TEMPERATURE_UNITS_SUPPORT)
 * M150 - Set Status LED Color as R<red> U<green> B<blue> W<white> P<bright>. Values 0-255. (Requires BLINKM, RGB_LED, RGBW_LED, NEOPIXEL_LED, PCA9533, or PCA9632).
 * M155 - Auto-report temperatures with interval of S<seconds>. (Requires AUTO_REPORT_SENSORS)
 * M226 - Wait until a pin is in a given state: "M226 P<pin> S<state>" (Requires DIRECT_PIN_CONTROL)
 * M305 - Set user thermistor parameters R T and P. (Requires TEMP_SENSOR_x 1000)
 * M355 - Set Case Light on/off and set brightness. (Requires CASE_LIGHT_PIN)
 * M404 - Display or set the Nominal Filament Width: "W<diameter>". (Requires FILAMENT_WIDTH_SENSOR)
 * M405 - Enable Filament Sensor flow control. "M405 D<delay_cm>". (Requires FILAMENT_WIDTH_SENSOR)
 * M406 - Disable Filament Sensor flow control. (Requires FILAMENT_WIDTH_SENSOR)
 * M407 - Display measured filament diameter in millimeters. (Requires FILAMENT_WIDTH_SENSOR)
 * M412 - Enable / Disable Filament Runout Detection. (Requires FILAMENT_RUNOUT_SENSOR)
 * M430 - Read the system current, voltage, and power (Requires POWER_MONITOR_CURRENT, POWER_MONITOR_VOLTAGE, or POWER_MONITOR_FIXED_VOLTAGE)
 * M500 - Store parameters in EEPROM. (Requires EEPROM_SETTINGS)
 * M501 - Restore parameters from EEPROM. (Requires EEPROM_SETTINGS)
 * M502 - Revert to the default "factory settings". ** Does not write them to EEPROM! **
 * M503 - Print the current settings (in memory): "M503 S<verbose>". S0 specifies compact output.
 * M504 - Validate EEPROM contents. (Requires EEPROM_SETTINGS)
 * M552 - Get or set IP address. Enable/disable network interface. (Requires enabled Ethernet port)
 * M553 - Get or set IP netmask. (Requires enabled Ethernet port)
 * M554 - Get or set IP gateway. (Requires enabled Ethernet port)
 * M575 - Change the serial baud rate. (Requires BAUD_RATE_GCODE)
 *
 *** I2C_POSITION_ENCODERS ***
 * M860 - Report the position of position encoder modules.
 * M861 - Report the status of position encoder modules.
 * M862 - Perform an axis continuity test for position encoder modules.
 * M863 - Perform steps-per-mm calibration for position encoder modules.
 * M864 - Change position encoder module I2C address.
 * M865 - Check position encoder module firmware version.
 * M866 - Report or reset position encoder module error count.
 * M867 - Enable/disable or toggle error correction for position encoder modules.
 * M868 - Report or set position encoder module error correction threshold.
 * M869 - Report position encoder module error.
 *
 * M3426 - Read MCP3426 ADC over I2C. (Requires HAS_MCP3426_ADC)
 *
 *** Custom codes (can be changed to suit future G-code standards) ***
 * M999 - Restart after being stopped by error
 *
 * D... - Custom Development G-code. Add hooks to 'gcode_D.cpp' for developers to test features. (Requires MARLIN_DEV_MODE)
 *        D576 - Set buffer monitoring options. (Requires BUFFER_MONITORING)
 *
 */

#include "../inc/MarlinConfig.h"
#include "parser.h"

#if ENABLED(I2C_POSITION_ENCODERS)
  #include "../feature/encoder_i2c.h"
#endif

class GcodeSuite {
public:

  static void report_echo_start(const bool forReplay);
  static void report_heading(const bool forReplay, FSTR_P const fstr, const bool eol=true);
  static void report_heading_etc(const bool forReplay, FSTR_P const fstr, const bool eol=true) {
    report_heading(forReplay, fstr, eol);
    report_echo_start(forReplay);
  }
  static void say_units();

  static int8_t get_target_extruder_from_command();
  static int8_t get_target_e_stepper_from_command(const int8_t dval=-1);

  static void process_parsed_command(const bool no_ok=false);
  static void process_next_command();

  // Execute G-code in-place, preserving current G-code parameters
  static void process_subcommands_now(FSTR_P fgcode);
  static void process_subcommands_now(char * gcode);

  #if EITHER(AUTO_REPORT_SENSORS, HOST_KEEPALIVE_FEATURE)
    static bool autoreport_paused;
    static bool set_autoreport_paused(const bool p) {
      const bool was = autoreport_paused;
      autoreport_paused = p;
      return was;
    }
  #else
    static constexpr bool autoreport_paused = false;
    static bool set_autoreport_paused(const bool) { return false; }
  #endif

  #if ENABLED(HOST_KEEPALIVE_FEATURE)
    /**
     * States for managing Marlin and host communication
     * Marlin sends messages if blocked or busy
     */
    enum MarlinBusyState : char {
      NOT_BUSY,           // Not in a handler
      IN_HANDLER,         // Processing a GCode
      IN_PROCESS,         // Known to be blocking command input (as in G29)
      PAUSED_FOR_USER,    // Blocking pending any input
      PAUSED_FOR_INPUT    // Blocking pending text input (concept)
    };

    static MarlinBusyState busy_state;
    static uint8_t host_keepalive_interval;

    static void host_keepalive();
    static bool host_keepalive_is_paused() { return busy_state >= PAUSED_FOR_USER; }

    #define KEEPALIVE_STATE(N) REMEMBER(_KA_, gcode.busy_state, gcode.N)
  #else
    #define KEEPALIVE_STATE(N) NOOP
  #endif

  static void dwell(millis_t time);

private:

  friend class MarlinSettings;

  #if ENABLED(MARLIN_DEV_MODE)
    static void D(const int16_t dcode);
  #endif

  static void G4();

  #if ENABLED(INCH_MODE_SUPPORT)
    static void G20();
    static void G21();
  #endif

  #if ENABLED(DIRECT_PIN_CONTROL)
    static void M42();
  #endif
  #if ENABLED(PINS_DEBUGGING)
    static void M43();
  #endif

  #if ENABLED(M100_FREE_MEMORY_WATCHER)
    static void M100();
  #endif

  static void M105();

  #if DISABLED(EMERGENCY_PARSER)
    static void M112();
  #endif

  static void M111();

  #if ENABLED(HOST_KEEPALIVE_FEATURE)
    static void M113();
  #endif

  static void M115();

  static void M118();
  static void M119();

  #if ENABLED(TEMPERATURE_UNITS_SUPPORT)
    static void M149();
    static void M149_report(const bool forReplay=true);
  #endif

  #if HAS_COLOR_LEDS
    static void M150();
  #endif

  #if ENABLED(AUTO_REPORT_SENSORS)
    static void M155();
  #endif

  #if ENABLED(DIRECT_PIN_CONTROL)
    static void M226();
  #endif

  #if HAS_USER_THERMISTORS
    static void M305();
  #endif

  #if ENABLED(CASE_LIGHT_ENABLE)
    static void M355();
  #endif

  #if ENABLED(FILAMENT_WIDTH_SENSOR)
    static void M404();
    static void M405();
    static void M406();
    static void M407();
  #endif

  #if HAS_FILAMENT_SENSOR
    static void M412();
    static void M412_report(const bool forReplay=true);
  #endif

  #if HAS_POWER_MONITOR
    static void M430();
  #endif

  static void M500();
  static void M501();
  static void M502();
  #if DISABLED(DISABLE_M503)
    static void M503();
  #endif
  #if ENABLED(EEPROM_SETTINGS)
    static void M504();
  #endif

  #if HAS_ETHERNET
    static void M552();
    static void M552_report();
    static void M553();
    static void M553_report();
    static void M554();
    static void M554_report();
  #endif

  #if ENABLED(BAUD_RATE_GCODE)
    static void M575();
  #endif

  #if ENABLED(I2C_POSITION_ENCODERS)
    FORCE_INLINE static void M860() { I2CPEM.M860(); }
    FORCE_INLINE static void M861() { I2CPEM.M861(); }
    FORCE_INLINE static void M862() { I2CPEM.M862(); }
    FORCE_INLINE static void M863() { I2CPEM.M863(); }
    FORCE_INLINE static void M864() { I2CPEM.M864(); }
    FORCE_INLINE static void M865() { I2CPEM.M865(); }
    FORCE_INLINE static void M866() { I2CPEM.M866(); }
    FORCE_INLINE static void M867() { I2CPEM.M867(); }
    FORCE_INLINE static void M868() { I2CPEM.M868(); }
    FORCE_INLINE static void M869() { I2CPEM.M869(); }
  #endif

  static void M999();

  #if ENABLED(HAS_MCP3426_ADC)
    static void M3426();
  #endif

};

extern GcodeSuite gcode;
