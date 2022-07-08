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

#include "MarlinConfig.h"

#if ENABLED(HOST_ACTION_COMMANDS)

//#define DEBUG_HOST_ACTIONS

#include "host_actions.h"

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  #include "gcode.h"
#endif

HostUI hostui;

void HostUI::action(const char* fstr, const bool eol) {
  SERIAL_ECHOPGM("//action:");
  SERIAL_ECHO(fstr);
  if (eol) SERIAL_EOL();
}

#ifdef ACTION_ON_KILL
  void HostUI::kill() { action(ACTION_ON_KILL); }
#endif
#ifdef ACTION_ON_PAUSE
  void HostUI::pause(const bool eol/*=true*/) { action(ACTION_ON_PAUSE, eol); }
#endif
#ifdef ACTION_ON_PAUSED
  void HostUI::paused(const bool eol/*=true*/) { action(ACTION_ON_PAUSED, eol); }
#endif
#ifdef ACTION_ON_RESUME
  void HostUI::resume() { action(ACTION_ON_RESUME); }
#endif
#ifdef ACTION_ON_RESUMED
  void HostUI::resumed() { action(ACTION_ON_RESUMED); }
#endif
#ifdef ACTION_ON_CANCEL
  void HostUI::cancel() { action(ACTION_ON_CANCEL); }
#endif
#ifdef ACTION_ON_START
  void HostUI::start() { action(ACTION_ON_START); }
#endif

#if ENABLED(G29_RETRY_AND_RECOVER)
  #ifdef ACTION_ON_G29_RECOVER
    void HostUI::g29_recover() { action(ACTION_ON_G29_RECOVER); }
  #endif
  #ifdef ACTION_ON_G29_FAILURE
    void HostUI::g29_failure() { action(ACTION_ON_G29_FAILURE); }
  #endif
#endif

#ifdef SHUTDOWN_ACTION
  void HostUI::shutdown() { action(SHUTDOWN_ACTION); }
#endif

#if ENABLED(HOST_PROMPT_SUPPORT)

  PromptReason HostUI::host_prompt_reason = PROMPT_NOT_DEFINED;

  #if HAS_RESUME_CONTINUE
    extern bool wait_for_user;
  #endif

  void HostUI::notify(const char * const cstr) {
    action("notification ", false);
    SERIAL_ECHOLN(cstr);
  }

  void HostUI::prompt(const char* ptype, const bool eol/*=true*/) {
    action("prompt_", false);
    SERIAL_ECHO(ptype);
    if (eol) SERIAL_EOL();
  }

  void HostUI::prompt_plus(const char* ptype, const char* fstr, const char extra_char/*='\0'*/) {
    prompt(ptype, false);
    SERIAL_CHAR(' ');
    SERIAL_ECHO(fstr);
    if (extra_char != '\0') SERIAL_CHAR(extra_char);
    SERIAL_EOL();
  }
  void HostUI::prompt_begin(const PromptReason reason, const char* fstr, const char extra_char/*='\0'*/) {
    prompt_end();
    host_prompt_reason = reason;
    prompt_plus("begin", fstr, extra_char);
  }
  void HostUI::prompt_button(const char* fstr) { prompt_plus("button", fstr); }
  void HostUI::prompt_end() { prompt("end"); }
  void HostUI::prompt_show() { prompt("show"); }

  void HostUI::_prompt_show(const char* btn1, const char* btn2) {
    if (btn1) prompt_button(btn1);
    if (btn2) prompt_button(btn2);
    prompt_show();
  }
  void HostUI::prompt_do(const PromptReason reason, const char* fstr, const char* btn1/*=nullptr*/, const char* btn2/*=nullptr*/) {
    prompt_begin(reason, fstr);
    _prompt_show(btn1, btn2);
  }
  void HostUI::prompt_do(const PromptReason reason, const char* fstr, const char extra_char, const char* btn1/*=nullptr*/, const char* btn2/*=nullptr*/) {
    prompt_begin(reason, fstr, extra_char);
    _prompt_show(btn1, btn2);
  }

  //
  // Handle responses from the host, such as:
  //  - Filament runout responses: Purge More, Continue
  //  - General "Continue" response
  //  - Resume Print response
  //  - Dismissal of info
  //
  void HostUI::handle_response(const uint8_t response) {
    const PromptReason hpr = host_prompt_reason;
    host_prompt_reason = PROMPT_NOT_DEFINED;  // Reset now ahead of logic
    switch (hpr) {
      case PROMPT_FILAMENT_RUNOUT:
        switch (response) {

          case 0: // "Purge More" button
            #if BOTH(M600_PURGE_MORE_RESUMABLE, ADVANCED_PAUSE_FEATURE)
              pause_menu_response = PAUSE_RESPONSE_EXTRUDE_MORE;  // Simulate menu selection (menu exits, doesn't extrude more)
            #endif
            break;

          case 1: // "Continue" / "Disable Runout" button
            #if BOTH(M600_PURGE_MORE_RESUMABLE, ADVANCED_PAUSE_FEATURE)
              pause_menu_response = PAUSE_RESPONSE_RESUME_PRINT;  // Simulate menu selection
            #endif
            #if HAS_FILAMENT_SENSOR
              if (runout.filament_ran_out) {                      // Disable a triggered sensor
                runout.enabled = false;
                runout.reset();
              }
            #endif
            break;
        }
        break;
      case PROMPT_USER_CONTINUE:
        #if HAS_RESUME_CONTINUE
          wait_for_user = false;
        #endif
        break;
      case PROMPT_PAUSE_RESUME:
        #if BOTH(ADVANCED_PAUSE_FEATURE, SDSUPPORT)
          extern const char M24_STR[];
          enqueue_and_echo_commands(M24_STR); // queue.inject_P(M24_STR);
        #endif
        break;
      case PROMPT_INFO:
        break;
      default: break;
    }
  }

#endif // HOST_PROMPT_SUPPORT

#endif // HOST_ACTION_COMMANDS
