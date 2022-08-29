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

#include "../inc/MarlinConfigPre.h"
#include "../HAL/shared/Marduino.h"

class HostUI {
  public:

  static void action(FSTR_P const fstr, const bool eol=true);

  #ifdef ACTION_ON_KILL
    static void kill();
  #endif
  #ifdef ACTION_ON_PAUSE
    static void pause(const bool eol=true);
  #endif
  #ifdef ACTION_ON_PAUSED
    static void paused(const bool eol=true);
  #endif
  #ifdef ACTION_ON_RESUME
    static void resume();
  #endif
  #ifdef ACTION_ON_RESUMED
    static void resumed();
  #endif
  #ifdef ACTION_ON_CANCEL
    static void cancel();
  #endif
  #ifdef ACTION_ON_START
    static void start();
  #endif
  #ifdef SHUTDOWN_ACTION
    static void shutdown();
  #endif

  #if ENABLED(G29_RETRY_AND_RECOVER)
    #ifdef ACTION_ON_G29_RECOVER
      static void g29_recover();
    #endif
    #ifdef ACTION_ON_G29_FAILURE
      static void g29_failure();
    #endif
  #endif
};

extern HostUI hostui;

extern const char CONTINUE_STR[], DISMISS_STR[];
