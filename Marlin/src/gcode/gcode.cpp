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

/**
 * gcode.cpp - Temporary container for all G-code handlers
 *             Most will migrate to classes, by feature.
 */

#include "gcode.h"
GcodeSuite gcode;

#if ENABLED(WIFI_CUSTOM_COMMAND)
  extern bool wifi_custom_command(char * const command_ptr);
#endif

#include "parser.h"
#include "queue.h"

#if ENABLED(HOST_ACTION_COMMANDS)
  #include "../feature/host_actions.h"
#endif

#include "../MarlinCore.h" // for idle, kill

#if EITHER(AUTO_REPORT_SENSORS, HOST_KEEPALIVE_FEATURE)
  bool GcodeSuite::autoreport_paused; // = false
#endif

#if ENABLED(HOST_KEEPALIVE_FEATURE)
  GcodeSuite::MarlinBusyState GcodeSuite::busy_state = NOT_BUSY;
  uint8_t GcodeSuite::host_keepalive_interval = DEFAULT_KEEPALIVE_INTERVAL;
#endif

void GcodeSuite::report_echo_start(const bool forReplay) { if (!forReplay) SERIAL_ECHO_START(); }
void GcodeSuite::report_heading(const bool forReplay, FSTR_P const fstr, const bool eol/*=true*/) {
  if (forReplay) return;
  if (fstr) {
    SERIAL_ECHO_START();
    SERIAL_ECHOPGM("; ");
    SERIAL_ECHOF(fstr);
  }
  if (eol) { SERIAL_CHAR(':'); SERIAL_EOL(); }
}

void GcodeSuite::say_units() {
  SERIAL_ECHOLNPGM_P(
    TERN_(INCH_MODE_SUPPORT, parser.linear_unit_factor != 1.0 ? PSTR(" (in)") :)
    PSTR(" (mm)")
  );
}

/**
 * Get the target extruder from the T parameter or the active_extruder
 * Return -1 if the T parameter is out of range
 */
int8_t GcodeSuite::get_target_extruder_from_command() {
  if (parser.seenval('T')) {
    const int8_t e = parser.value_byte();
    if (e < NUM_TEMP_SENSORS) return e;
    SERIAL_ECHO_START();
    SERIAL_CHAR('M'); SERIAL_ECHO(parser.codenum);
    SERIAL_ECHOLNPGM(" " STR_INVALID_EXTRUDER " ", e);
    return -1;
  }
  return -1;
}

/**
 * Get the target E stepper from the 'T' parameter.
 * If there is no 'T' parameter then dval will be substituted.
 * Returns -1 if the resulting E stepper index is out of range.
 */
int8_t GcodeSuite::get_target_e_stepper_from_command(const int8_t dval/*=-1*/) {
  const int8_t e = parser.intval('T', dval);
  if (WITHIN(e, 0, NUM_TEMP_SENSORS - 1)) return e;
  if (dval == -2) return dval;

  SERIAL_ECHO_START();
  SERIAL_CHAR('M'); SERIAL_ECHO(parser.codenum);
  // if (e == -1)
  //   SERIAL_ECHOLNPGM(" " STR_E_STEPPER_NOT_SPECIFIED);
  // else
  //   SERIAL_ECHOLNPGM(" " STR_INVALID_E_STEPPER " ", e);
  return -1;
}

/**
 * Dwell waits immediately. It does not synchronize. Use M400 instead of G4
 */
void GcodeSuite::dwell(millis_t time) {
  time += millis();
  while (PENDING(millis(), time)) idle();
}

/**
 * Process the parsed command and dispatch it to its handler
 */
void GcodeSuite::process_parsed_command(const bool no_ok/*=false*/) {

  KEEPALIVE_STATE(IN_HANDLER);

  // Handle a known command or reply "unknown command"

  switch (parser.command_letter) {

    case 'G': switch (parser.codenum) {

      case 4: G4(); break;                                        // G4: Dwell

      #if ENABLED(INCH_MODE_SUPPORT)
        case 20: G20(); break;                                    // G20: Inch Mode
        case 21: G21(); break;                                    // G21: MM Mode
      #else
        case 21: NOOP; break;                                     // No error on unknown G21
      #endif

      #if ENABLED(DEBUG_GCODE_PARSER)
        case 800: parser.debug(); break;                          // G800: GCode Parser Test for G
      #endif

      default: parser.unknown_command_warning(); break;
    }
    break;

    case 'M': switch (parser.codenum) {

      #if ENABLED(DIRECT_PIN_CONTROL)
        case 42: M42(); break;                                    // M42: Change pin state
      #endif

      #if ENABLED(PINS_DEBUGGING)
        case 43: M43(); break;                                    // M43: Read pin state
      #endif

      #if ENABLED(M100_FREE_MEMORY_WATCHER)
        case 100: M100(); break;                                  // M100: Free Memory Report
      #endif

      case 105: M105(); return;                                   // M105: Report Temperatures (and say "ok")

      case 111: M111(); break;                                    // M111: Set debug level

      #if DISABLED(EMERGENCY_PARSER)
        case 112: M112(); break;                                  // M112: Full Shutdown
      #else
        case 112: break;
      #endif

      #if ENABLED(HOST_KEEPALIVE_FEATURE)
        case 113: M113(); break;                                  // M113: Set Host Keepalive interval
      #endif

      #if ENABLED(AUTO_REPORT_SENSORS)
        case 155: M155(); break;                                  // M155: Set temperature auto-report interval
      #endif

      case 115: M115(); break;                                    // M115: Report capabilities

      case 118: M118(); break;                                    // M118: Display a message in the host console
      case 119: M119(); break;                                    // M119: Report endstop states

      #if ENABLED(TEMPERATURE_UNITS_SUPPORT)
        case 149: M149(); break;                                  // M149: Set temperature units
      #endif

      #if HAS_COLOR_LEDS
        case 150: M150(); break;                                  // M150: Set Status LED Color
      #endif

      #if ENABLED(DIRECT_PIN_CONTROL)
        case 226: M226(); break;                                  // M226: Wait until a pin reaches a state
      #endif

      #if HAS_USER_THERMISTORS
        case 305: M305(); break;                                  // M305: Set user thermistor parameters
      #endif

      #if ENABLED(FILAMENT_WIDTH_SENSOR)
        case 404: M404(); break;                                  // M404: Enter the nominal filament width (3mm, 1.75mm ) N<3.0> or display nominal filament width
        case 405: M405(); break;                                  // M405: Turn on filament sensor for control
        case 406: M406(); break;                                  // M406: Turn off filament sensor for control
        case 407: M407(); break;                                  // M407: Display measured filament diameter
      #endif

      #if HAS_FILAMENT_SENSOR
        case 412: M412(); break;                                  // M412: Enable/Disable filament runout detection
      #endif

      #if HAS_POWER_MONITOR
        case 430: M430(); break;                                  // M430: Read the system current (A), voltage (V), and power (W)
      #endif

      case 500: M500(); break;                                    // M500: Store settings in EEPROM
      case 501: M501(); break;                                    // M501: Read settings from EEPROM
      case 502: M502(); break;                                    // M502: Revert to default settings
      #if DISABLED(DISABLE_M503)
        case 503: M503(); break;                                  // M503: print settings currently in memory
      #endif
      #if ENABLED(EEPROM_SETTINGS)
        case 504: M504(); break;                                  // M504: Validate EEPROM contents
      #endif

      #if HAS_ETHERNET
        case 552: M552(); break;                                  // M552: Set IP address
        case 553: M553(); break;                                  // M553: Set gateway
        case 554: M554(); break;                                  // M554: Set netmask
      #endif

      #if ENABLED(BAUD_RATE_GCODE)
        case 575: M575(); break;                                  // M575: Set serial baudrate
      #endif

      #if ENABLED(CASE_LIGHT_ENABLE)
        case 355: M355(); break;                                  // M355: Set case light brightness
      #endif

      #if ENABLED(DEBUG_GCODE_PARSER)
        case 800: parser.debug(); break;                          // M800: GCode Parser Test for M
      #endif

      #if ENABLED(I2C_POSITION_ENCODERS)
        case 860: M860(); break;                                  // M860: Report encoder module position
        case 861: M861(); break;                                  // M861: Report encoder module status
        case 862: M862(); break;                                  // M862: Perform axis test
        case 863: M863(); break;                                  // M863: Calibrate steps/mm
        case 864: M864(); break;                                  // M864: Change module address
        case 865: M865(); break;                                  // M865: Check module firmware version
        case 866: M866(); break;                                  // M866: Report axis error count
        case 867: M867(); break;                                  // M867: Toggle error correction
        case 868: M868(); break;                                  // M868: Set error correction threshold
        case 869: M869(); break;                                  // M869: Report axis error
      #endif

      case 999: M999(); break;                                    // M999: Restart after being Stopped

      #if ENABLED(HAS_MCP3426_ADC)
        case 3426: M3426(); break;                                // M3426: Read MCP3426 ADC (over i2c)
      #endif

      default: parser.unknown_command_warning(); break;
    }
    break;

    #if ENABLED(MARLIN_DEV_MODE)
      case 'D': D(parser.codenum); break;                         // Dn: Debug codes
    #endif

    #if ENABLED(REALTIME_REPORTING_COMMANDS)
      case 'S': case 'P': case 'R': break;                        // Invalid S, P, R commands already filtered
    #endif

    default:
      #if ENABLED(WIFI_CUSTOM_COMMAND)
        if (wifi_custom_command(parser.command_ptr)) break;
      #endif
      parser.unknown_command_warning();
  }

  if (!no_ok) queue.ok_to_send();

  SERIAL_OUT(msgDone); // Call the msgDone serial hook to signal command processing done
}

#if ENABLED(M100_FREE_MEMORY_DUMPER)
  void M100_dump_routine(FSTR_P const title, const char * const start, const uintptr_t size);
#endif

/**
 * Process a single command and dispatch it to its handler
 * This is called from the main loop()
 */
void GcodeSuite::process_next_command() {
  GCodeQueue::CommandLine &command = queue.ring_buffer.peek_next_command();

  PORT_REDIRECT(SERIAL_PORTMASK(command.port));

  TERN_(POWER_LOSS_RECOVERY, recovery.queue_index_r = queue.ring_buffer.index_r);

  if (DEBUGGING(ECHO)) {
    SERIAL_ECHO_START();
    SERIAL_ECHOLN(command.buffer);
    #if ENABLED(M100_FREE_MEMORY_DUMPER)
      SERIAL_ECHOPGM("slot:", queue.ring_buffer.index_r);
      M100_dump_routine(F("   Command Queue:"), (const char*)&queue.ring_buffer, sizeof(queue.ring_buffer));
    #endif
  }

  // Parse the next command in the queue
  parser.parse(command.buffer);
  process_parsed_command();
}

#pragma GCC diagnostic push
#if GCC_VERSION >= 80000
  #pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif

/**
 * Run a series of commands, bypassing the command queue to allow
 * G-code "macros" to be called from within other G-code handlers.
 */
void GcodeSuite::process_subcommands_now(FSTR_P fgcode) {
  PGM_P pgcode = FTOP(fgcode);
  char * const saved_cmd = parser.command_ptr;        // Save the parser state
  for (;;) {
    PGM_P const delim = strchr_P(pgcode, '\n');       // Get address of next newline
    const size_t len = delim ? delim - pgcode : strlen_P(pgcode); // Get the command length
    char cmd[len + 1];                                // Allocate a stack buffer
    strncpy_P(cmd, pgcode, len);                      // Copy the command to the stack
    cmd[len] = '\0';                                  // End with a nul
    parser.parse(cmd);                                // Parse the command
    process_parsed_command(true);                     // Process it (no "ok")
    if (!delim) break;                                // Last command?
    pgcode = delim + 1;                               // Get the next command
  }
  parser.parse(saved_cmd);                            // Restore the parser state
}

#pragma GCC diagnostic pop

void GcodeSuite::process_subcommands_now(char * gcode) {
  char * const saved_cmd = parser.command_ptr;        // Save the parser state
  for (;;) {
    char * const delim = strchr(gcode, '\n');         // Get address of next newline
    if (delim) *delim = '\0';                         // Replace with nul
    parser.parse(gcode);                              // Parse the current command
    process_parsed_command(true);                     // Process it (no "ok")
    if (!delim) break;                                // Last command?
    *delim = '\n';                                    // Put back the newline
    gcode = delim + 1;                                // Get the next command
  }
  parser.parse(saved_cmd);                            // Restore the parser state
}

#if ENABLED(HOST_KEEPALIVE_FEATURE)

  /**
   * Output a "busy" message at regular intervals
   * while the machine is not accepting commands.
   */
  void GcodeSuite::host_keepalive() {
    const millis_t ms = millis();
    static millis_t next_busy_signal_ms = 0;
    if (!autoreport_paused && host_keepalive_interval && busy_state != NOT_BUSY) {
      if (PENDING(ms, next_busy_signal_ms)) return;
      PORT_REDIRECT(SerialMask::All);
      switch (busy_state) {
        case IN_HANDLER:
        case IN_PROCESS:
          SERIAL_ECHO_MSG(STR_BUSY_PROCESSING);
          TERN_(FULL_REPORT_TO_HOST_FEATURE, report_current_position_moving());
          break;
        case PAUSED_FOR_USER:
          SERIAL_ECHO_MSG(STR_BUSY_PAUSED_FOR_USER);
          TERN_(FULL_REPORT_TO_HOST_FEATURE, set_and_report_grblstate(M_HOLD));
          break;
        case PAUSED_FOR_INPUT:
          SERIAL_ECHO_MSG(STR_BUSY_PAUSED_FOR_INPUT);
          TERN_(FULL_REPORT_TO_HOST_FEATURE, set_and_report_grblstate(M_HOLD));
          break;
        default:
          break;
      }
    }
    next_busy_signal_ms = ms + SEC_TO_MS(host_keepalive_interval);
  }

#endif // HOST_KEEPALIVE_FEATURE
