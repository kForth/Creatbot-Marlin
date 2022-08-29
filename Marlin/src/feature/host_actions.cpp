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

#include "../inc/MarlinConfig.h"

#if ENABLED(HOST_ACTION_COMMANDS)

//#define DEBUG_HOST_ACTIONS

#include "host_actions.h"

#if HAS_FILAMENT_SENSOR
  #include "runout.h"
#endif

HostUI hostui;

void HostUI::action(FSTR_P const fstr, const bool eol) {
  PORT_REDIRECT(SerialMask::All);
  SERIAL_ECHOPGM("//action:");
  SERIAL_ECHOF(fstr);
  if (eol) SERIAL_EOL();
}

#ifdef ACTION_ON_KILL
  void HostUI::kill() { action(F(ACTION_ON_KILL)); }
#endif
#ifdef ACTION_ON_PAUSE
  void HostUI::pause(const bool eol/*=true*/) { action(F(ACTION_ON_PAUSE), eol); }
#endif
#ifdef ACTION_ON_PAUSED
  void HostUI::paused(const bool eol/*=true*/) { action(F(ACTION_ON_PAUSED), eol); }
#endif
#ifdef ACTION_ON_RESUME
  void HostUI::resume() { action(F(ACTION_ON_RESUME)); }
#endif
#ifdef ACTION_ON_RESUMED
  void HostUI::resumed() { action(F(ACTION_ON_RESUMED)); }
#endif
#ifdef ACTION_ON_CANCEL
  void HostUI::cancel() { action(F(ACTION_ON_CANCEL)); }
#endif
#ifdef ACTION_ON_START
  void HostUI::start() { action(F(ACTION_ON_START)); }
#endif
#ifdef SHUTDOWN_ACTION
  void HostUI::shutdown() { action(F(SHUTDOWN_ACTION)); }
#endif

#endif // HOST_ACTION_COMMANDS
