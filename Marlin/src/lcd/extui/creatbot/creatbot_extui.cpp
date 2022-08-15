/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2021 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
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
 * lcd/extui/creatbot/creatbot_extui.cpp
 */

#include "../../../inc/MarlinConfigPre.h"

#if ENABLED(CREATBOT_LCD)

#include "../ui_api.h"
#include "dwin_lcd.h"

namespace ExtUI {

  void onStartup() {
    dwin_init();
  }

  void onIdle() {
    dwin_loop();
  }

  void onPrinterKilled(FSTR_P const error, FSTR_P const component) {
    return_default_button_action();
  }

  void onMediaInserted() { 
    return_default_button_action();
  }
  void onMediaError() { 
    return_default_button_action();
  }
  void onMediaRemoved() { 
    return_default_button_action();
  }

  void onPlayTone(const uint16_t frequency, const uint16_t duration) { 
    return_default_button_action();
  }
  
  void onPrintTimerStarted() { 
    return_default_button_action();
  }
  void onPrintTimerPaused() { 
    return_default_button_action();
  }
  void onPrintTimerStopped() { 
    return_default_button_action();
  }
  
  void onFilamentRunout(const extruder_t) { 
    return_default_button_action();
  }
  void onUserConfirmRequired(const char * const msg) { 
    return_default_button_action();
  }
  void onStatusChanged(const char * const msg) { 
    return_default_button_action();
  }

  void onHomingStart() {
    return_default_button_action();
  }
  void onHomingDone() {
    return_default_button_action();
  }
  void onPrintDone() { 
    return_default_button_action();
  }

  void onFactoryReset() {
    return_default_button_action();
  }
  void onStoreSettings(char *buff) { }
  void onLoadSettings(const char *buff) { }
  void onPostprocessSettings() { }
  void onSettingsStored(bool success) { }
  void onSettingsLoaded(bool success) { }

  #if HAS_MESH
    void onLevelingStart() { }
    void onLevelingDone() { }
    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const float zval) { }
    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const probe_state_t state) { }
    void onMeshUpdate(signed char xpos, signed char ypos, float const &zval) { }
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    void onPowerLossResume() {
      // Called on resume from power-loss

      return_default_button_action();
    }
  #endif

  #if HAS_PID_HEATING
    void onPidTuning(const result_t rst) { }
  #endif

  void onSteppersDisabled() {}
  void onSteppersEnabled()  {}

}

#endif // CREATBOT_LCD
