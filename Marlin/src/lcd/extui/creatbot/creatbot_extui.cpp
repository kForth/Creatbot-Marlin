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

namespace ExtUI {

  void onStartup() {
    // TODO
  }

  void onIdle() {
    // TODO
  }

  void onPrinterKilled(FSTR_P const error, FSTR_P const component) {
    // TODO
  }
  void onMediaInserted() { 
    // TODO
  }
  void onMediaError() { 
    // TODO
  }
  void onMediaRemoved() { 
    // TODO
  }
  void onPlayTone(const uint16_t frequency, const uint16_t duration) { 
    // TODO
  }
  void onPrintTimerStarted() { 
    // TODO
  }
  void onPrintTimerPaused() { 
    // TODO
  }
  void onPrintTimerStopped() { 
    // TODO
  }
  void onFilamentRunout(const extruder_t) { 
    // TODO
  }
  void onUserConfirmRequired(const char * const msg) { 
    // TODO
  }
  void onStatusChanged(const char * const msg) { 
    // TODO
  }

  void onHomingStart() {
    // TODO
  }
  void onHomingDone() {
    // TODO
  }
  void onPrintDone() { 
    // TODO
  }

  void onFactoryReset() {
    // TODO
  }

  void onStoreSettings(char *buff) {
    // Called when saving to EEPROM (i.e. M500). If the ExtUI needs
    // permanent data to be stored, it can write up to eeprom_data_size bytes
    // into buff.

    // Example:
    //  static_assert(sizeof(myDataStruct) <= eeprom_data_size);
    //  memcpy(buff, &myDataStruct, sizeof(myDataStruct));

    // TODO
  }

  void onLoadSettings(const char *buff) {
    // Called while loading settings from EEPROM. If the ExtUI
    // needs to retrieve data, it should copy up to eeprom_data_size bytes
    // from buff

    // Example:
    //  static_assert(sizeof(myDataStruct) <= eeprom_data_size);
    //  memcpy(&myDataStruct, buff, sizeof(myDataStruct));
    
    // TODO
  }

  void onPostprocessSettings() {
    // Called after loading or resetting stored settings
    
    // TODO
  }

  void onSettingsStored(bool success) {
    // Called after the entire EEPROM has been written,
    // whether successful or not.
    
    // TODO
  }

  void onSettingsLoaded(bool success) {
    // Called after the entire EEPROM has been read,
    // whether successful or not.
    
    // TODO
  }

  #if HAS_MESH
    void onLevelingStart() {
      // TODO
    }
    void onLevelingDone() {
      // TODO
    }

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const float zval) {
      // Called when any mesh points are updated

      // TODO
    }

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const probe_state_t state) {
      // Called to indicate a special condition

      // TODO
    }

    void onMeshUpdate(signed char xpos, signed char ypos, float const &zval) {
      // Called from bbl.cpp > line 105 for some reason

      // TODO
    }
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    void onPowerLossResume() {
      // Called on resume from power-loss

      // TODO
    }
  #endif

  #if HAS_PID_HEATING
    void onPidTuning(const result_t rst) {
      // Called for temperature PID tuning result

      // TODO
    }
  #endif

  void onSteppersDisabled() {}
  void onSteppersEnabled()  {}

}

#endif // CREATBOT_LCD
