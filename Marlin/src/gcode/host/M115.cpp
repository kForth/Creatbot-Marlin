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

#include "../gcode.h"
#include "../../inc/MarlinConfig.h"
#include "../queue.h"           // for getting the command port

#if ENABLED(CASE_LIGHT_ENABLE)
  #include "../../feature/caselight.h"
#endif

//#define MINIMAL_CAP_LINES // Don't even mention the disabled capabilities

#if ENABLED(EXTENDED_CAPABILITIES_REPORT)
  #if ENABLED(MINIMAL_CAP_LINES)
    #define cap_line(S,C) if (C) _cap_line(S)
    static void _cap_line(FSTR_P const name) {
      SERIAL_ECHOPGM("Cap:");
      SERIAL_ECHOF(name);
      SERIAL_ECHOLNPGM(":1");
    }
  #else
    #define cap_line(V...) _cap_line(V)
    static void _cap_line(FSTR_P const name, bool ena=false) {
      SERIAL_ECHOPGM("Cap:");
      SERIAL_ECHOF(name);
      SERIAL_CHAR(':', '0' + ena);
      SERIAL_EOL();
    }
  #endif
#endif

/**
 * M115: Capabilities string and extended capabilities report
 *       If a capability is not reported, hosts should assume
 *       the capability is not present.
 */
void GcodeSuite::M115() {
  SERIAL_ECHOLNPGM(
    "FIRMWARE_NAME:Marlin " DETAILED_BUILD_VERSION " (" __DATE__ " " __TIME__ ") "
    "SOURCE_CODE_URL:" SOURCE_CODE_URL " "
    "PROTOCOL_VERSION:" PROTOCOL_VERSION " "
    "MACHINE_TYPE:" MACHINE_NAME " "
    #if NUM_TEMP_SENSORS
      "TEMP_SENSORS:" STRINGIFY(NUM_TEMP_SENSORS) " "
    #endif
    #ifdef MACHINE_UUID
      "UUID:" MACHINE_UUID
    #endif
  );

  #if ENABLED(EXTENDED_CAPABILITIES_REPORT)

    // The port that sent M115
    // serial_index_t port = queue.ring_buffer.command_port();

    // PAREN_COMMENTS
    TERN_(PAREN_COMMENTS, cap_line(F("PAREN_COMMENTS"), true));

    // QUOTED_STRINGS
    TERN_(GCODE_QUOTED_STRINGS, cap_line(F("QUOTED_STRINGS"), true));

    // SERIAL_XON_XOFF
    cap_line(F("SERIAL_XON_XOFF"), ENABLED(SERIAL_XON_XOFF));

    // EEPROM (M500, M501)
    cap_line(F("EEPROM"), ENABLED(EEPROM_SETTINGS));

    // AUTOREPORT_TEMP (M155)
    cap_line(F("AUTOREPORT_TEMP"), ENABLED(AUTO_REPORT_SENSORS));

    // RUNOUT (M412)
    cap_line(F("RUNOUT"), ENABLED(FILAMENT_RUNOUT_SENSOR));

    // TOGGLE_LIGHTS (M355)
    cap_line(F("TOGGLE_LIGHTS"), ENABLED(CASE_LIGHT_ENABLE));
    cap_line(F("CASE_LIGHT_BRIGHTNESS"), TERN0(CASE_LIGHT_ENABLE, caselight.has_brightness()));

    // EMERGENCY_PARSER (M112)
    cap_line(F("EMERGENCY_PARSER"), ENABLED(EMERGENCY_PARSER));

    // HOST ACTION COMMANDS (paused, resume, resumed, cancel, etc.)
    cap_line(F("HOST_ACTION_COMMANDS"), ENABLED(HOST_ACTION_COMMANDS));

  #endif // EXTENDED_CAPABILITIES_REPORT
}
