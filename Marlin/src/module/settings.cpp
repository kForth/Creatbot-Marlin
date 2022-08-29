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
 * settings.cpp
 *
 * Settings and EEPROM storage
 *
 * IMPORTANT:  Whenever there are changes made to the variables stored in EEPROM
 * in the functions below, also increment the version number. This makes sure that
 * the default values are used whenever there is a change to the data, to prevent
 * wrong data being written to the variables.
 *
 * ALSO: Variables in the Store and Retrieve sections must be in the same order.
 *       If a feature is disabled, some data must still be written that, when read,
 *       either sets a Sane Default, or results in No Change to the existing value.
 */

// Change EEPROM version if the structure changes
#define EEPROM_VERSION "V86"
#define EEPROM_OFFSET 100

// Check the integrity of data offsets.
// Can be disabled for production build.
//#define DEBUG_EEPROM_READWRITE

#include "settings.h"

#include "digital_inputs.h"
#include "temperature.h"

#include "../gcode/gcode.h"
#include "../MarlinCore.h"

#if EITHER(EEPROM_SETTINGS, SD_FIRMWARE_UPDATE)
  #include "../HAL/shared/eeprom_api.h"
#endif

#if HAS_POWER_MONITOR
  #include "../feature/power_monitor.h"
#endif

#if HAS_FILAMENT_SENSOR
  #include "../feature/runout.h"
  #ifndef FIL_RUNOUT_ENABLED_DEFAULT
    #define FIL_RUNOUT_ENABLED_DEFAULT true
  #endif
#endif

#if ENABLED(CASE_LIGHT_ENABLE)
  #include "../feature/caselight.h"
#endif

#if HAS_ETHERNET
  #include "../feature/ethernet.h"
#endif

#pragma pack(push, 1) // No padding between variables

#if HAS_ETHERNET
  void ETH0_report();
  void MAC_report();
#endif

/**
 * Current EEPROM Layout
 *
 * Keep this data structure up to date so
 * EEPROM size is known at compile time!
 */
typedef struct SettingsDataStruct {
  char      version[4];                                 // Vnn\0
  #if ENABLED(EEPROM_INIT_NOW)
    uint32_t build_hash;                                // Unique build hash
  #endif
  uint16_t  crc;                                        // Data Checksum

  //
  // FILAMENT_RUNOUT_SENSOR
  //
  bool runout_sensor_enabled;                           // M412 S
  float runout_distance_mm;                             // M412 D

  //
  // User-defined Thermistors
  //
  #if HAS_USER_THERMISTORS
    user_thermistor_t user_thermistor[USER_THERMISTORS]; // M305 P0 R4700 T100000 B3950
  #endif

  //
  // Power monitor
  //
  uint8_t power_monitor_flags;                          // M430 I V W

  //
  // CASELIGHT_USES_BRIGHTNESS
  //
  #if CASELIGHT_USES_BRIGHTNESS
    uint8_t caselight_brightness;                        // M355 P
  #endif

  // Ethernet settings
  #if HAS_ETHERNET
    bool ethernet_hardware_enabled;                     // M552 S
    uint32_t ethernet_ip,                               // M552 P
             ethernet_dns,
             ethernet_gateway,                          // M553 P
             ethernet_subnet;                           // M554 P
  #endif

} SettingsData;

//static_assert(sizeof(SettingsData) <= MARLIN_EEPROM_SIZE, "EEPROM too small to contain SettingsData!");

MarlinSettings settings;

uint16_t MarlinSettings::datasize() { return sizeof(SettingsData); }

/**
 * Post-process after Retrieve or Reset
 */

#if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
  float new_z_fade_height;
#endif

void MarlinSettings::postprocess() {

  TERN_(CASELIGHT_USES_BRIGHTNESS, caselight.update_brightness());

}

#ifdef ARCHIM2_SPI_FLASH_EEPROM_BACKUP_SIZE
  static_assert(EEPROM_OFFSET + sizeof(SettingsData) < ARCHIM2_SPI_FLASH_EEPROM_BACKUP_SIZE,
                "ARCHIM2_SPI_FLASH_EEPROM_BACKUP_SIZE is insufficient to capture all EEPROM data.");
#endif

//
// This file simply uses the DEBUG_ECHO macros to implement EEPROM_CHITCHAT.
// For deeper debugging of EEPROM issues enable DEBUG_EEPROM_READWRITE.
//
#define DEBUG_OUT EITHER(EEPROM_CHITCHAT, DEBUG_LEVELING_FEATURE)
#include "../core/debug_out.h"

#if ENABLED(EEPROM_SETTINGS)

  #define EEPROM_ASSERT(TST,ERR)  do{ if (!(TST)) { SERIAL_ERROR_MSG(ERR); eeprom_error = true; } }while(0)

  #if ENABLED(DEBUG_EEPROM_READWRITE)
    #define _FIELD_TEST(FIELD) \
      EEPROM_ASSERT( \
        eeprom_error || eeprom_index == offsetof(SettingsData, FIELD) + EEPROM_OFFSET, \
        "Field " STRINGIFY(FIELD) " mismatch." \
      )
  #else
    #define _FIELD_TEST(FIELD) NOOP
  #endif

  const char version[4] = EEPROM_VERSION;

  #if ENABLED(EEPROM_INIT_NOW)
    constexpr uint32_t strhash32(const char *s, const uint32_t h=0) {
      return *s ? strhash32(s + 1, ((h + *s) << (*s & 3)) ^ *s) : h;
    }
    constexpr uint32_t build_hash = strhash32(__DATE__ __TIME__);
  #endif

  bool MarlinSettings::eeprom_error, MarlinSettings::validating;
  int MarlinSettings::eeprom_index;
  uint16_t MarlinSettings::working_crc;

  bool MarlinSettings::size_error(const uint16_t size) {
    if (size != datasize()) {
      DEBUG_ERROR_MSG("EEPROM datasize error."
        #if ENABLED(MARLIN_DEV_MODE)
          " (Actual:", size, " Expected:", datasize(), ")"
        #endif
      );
      return true;
    }
    return false;
  }

  /**
   * M500 - Store Configuration
   */
  bool MarlinSettings::save() {
    char ver[4] = "ERR";

    if (!EEPROM_START(EEPROM_OFFSET)) return false;

    eeprom_error = false;

    // Write or Skip version. (Flash doesn't allow rewrite without erase.)
    TERN(FLASH_EEPROM_EMULATION, EEPROM_SKIP, EEPROM_WRITE)(ver);

    #if ENABLED(EEPROM_INIT_NOW)
      EEPROM_SKIP(build_hash);  // Skip the hash slot
    #endif

    EEPROM_SKIP(working_crc);   // Skip the checksum slot

    working_crc = 0; // clear before first "real data"

    //
    // Filament Runout Sensor
    //
    {
      #if HAS_FILAMENT_SENSOR
        const bool &runout_sensor_enabled = runout.enabled;
      #else
        constexpr int8_t runout_sensor_enabled = -1;
      #endif
      _FIELD_TEST(runout_sensor_enabled);
      EEPROM_WRITE(runout_sensor_enabled);

      #if HAS_FILAMENT_RUNOUT_DISTANCE
        const float &runout_distance_mm = runout.runout_distance();
      #else
        constexpr float runout_distance_mm = 0;
      #endif
      EEPROM_WRITE(runout_distance_mm);
    }

    //
    // User-defined Thermistors
    //
    #if HAS_USER_THERMISTORS
    {
      _FIELD_TEST(user_thermistor);
      EEPROM_WRITE(tempSensors.user_thermistor);
    }
    #endif

    //
    // Power monitor
    //
    {
      #if HAS_POWER_MONITOR
        const uint8_t &power_monitor_flags = power_monitor.flags;
      #else
        constexpr uint8_t power_monitor_flags = 0x00;
      #endif
      _FIELD_TEST(power_monitor_flags);
      EEPROM_WRITE(power_monitor_flags);
    }

    //
    // Case Light Brightness
    //
    #if CASELIGHT_USES_BRIGHTNESS
      EEPROM_WRITE(caselight.brightness);
    #endif

    //
    // Ethernet network info
    //
    #if HAS_ETHERNET
    {
      _FIELD_TEST(ethernet_hardware_enabled);
      const bool ethernet_hardware_enabled = ethernet.hardware_enabled;
      const uint32_t ethernet_ip      = ethernet.ip,
                     ethernet_dns     = ethernet.myDns,
                     ethernet_gateway = ethernet.gateway,
                     ethernet_subnet  = ethernet.subnet;
      EEPROM_WRITE(ethernet_hardware_enabled);
      EEPROM_WRITE(ethernet_ip);
      EEPROM_WRITE(ethernet_dns);
      EEPROM_WRITE(ethernet_gateway);
      EEPROM_WRITE(ethernet_subnet);
    }
    #endif

    //
    // Report final CRC and Data Size
    //
    if (!eeprom_error) {
      const uint16_t eeprom_size = eeprom_index - (EEPROM_OFFSET),
                     final_crc = working_crc;

      // Write the EEPROM header
      eeprom_index = EEPROM_OFFSET;

      EEPROM_WRITE(version);
      #if ENABLED(EEPROM_INIT_NOW)
        EEPROM_WRITE(build_hash);
      #endif
      EEPROM_WRITE(final_crc);

      // Report storage size
      DEBUG_ECHO_MSG("Settings Stored (", eeprom_size, " bytes; crc ", (uint32_t)final_crc, ")");

      eeprom_error |= size_error(eeprom_size);
    }
    EEPROM_FINISH();

    TERN_(EXTENSIBLE_UI, ExtUI::onSettingsStored(!eeprom_error));

    return !eeprom_error;
  }

  /**
   * M501 - Retrieve Configuration
   */
  bool MarlinSettings::_load() {
    if (!EEPROM_START(EEPROM_OFFSET)) return false;

    char stored_ver[4];
    EEPROM_READ_ALWAYS(stored_ver);

    // Version has to match or defaults are used
    if (strncmp(version, stored_ver, 3) != 0) {
      if (stored_ver[3] != '\0') {
        stored_ver[0] = '?';
        stored_ver[1] = '\0';
      }
      DEBUG_ECHO_MSG("EEPROM version mismatch (EEPROM=", stored_ver, " Marlin=" EEPROM_VERSION ")");

      eeprom_error = true;
    }
    else {

      // Optionally reset on the first boot after flashing
      #if ENABLED(EEPROM_INIT_NOW)
        uint32_t stored_hash;
        EEPROM_READ_ALWAYS(stored_hash);
        if (stored_hash != build_hash) { EEPROM_FINISH(); return false; }
      #endif

      uint16_t stored_crc;
      EEPROM_READ_ALWAYS(stored_crc);

      working_crc = 0;  // Init to 0. Accumulated by EEPROM_READ

      _FIELD_TEST(e_factors);

      //
      // Filament Runout Sensor
      //
      {
        int8_t runout_sensor_enabled;
        _FIELD_TEST(runout_sensor_enabled);
        EEPROM_READ(runout_sensor_enabled);
        #if HAS_FILAMENT_SENSOR
          runout.enabled = runout_sensor_enabled < 0 ? FIL_RUNOUT_ENABLED_DEFAULT : runout_sensor_enabled;
        #endif

        TERN_(HAS_FILAMENT_SENSOR, if (runout.enabled) runout.reset());

        float runout_distance_mm;
        EEPROM_READ(runout_distance_mm);
        #if HAS_FILAMENT_RUNOUT_DISTANCE
          if (!validating) runout.set_runout_distance(runout_distance_mm);
        #endif
      }

      //
      // User-defined Thermistors
      //
      #if HAS_USER_THERMISTORS
      {
        user_thermistor_t user_thermistor[USER_THERMISTORS];
        _FIELD_TEST(user_thermistor);
        EEPROM_READ(user_thermistor);
        if (!validating) COPY(tempSensors.user_thermistor, user_thermistor);
      }
      #endif

      //
      // Power monitor
      //
      {
        uint8_t power_monitor_flags;
        _FIELD_TEST(power_monitor_flags);
        EEPROM_READ(power_monitor_flags);
        TERN_(HAS_POWER_MONITOR, if (!validating) power_monitor.flags = power_monitor_flags);
      }

      //
      // Case Light Brightness
      //
      #if CASELIGHT_USES_BRIGHTNESS
        _FIELD_TEST(caselight_brightness);
        EEPROM_READ(caselight.brightness);
      #endif

      //
      // Ethernet network info
      //
      #if HAS_ETHERNET
        _FIELD_TEST(ethernet_hardware_enabled);
        uint32_t ethernet_ip, ethernet_dns, ethernet_gateway, ethernet_subnet;
        EEPROM_READ(ethernet.hardware_enabled);
        EEPROM_READ(ethernet_ip);      ethernet.ip      = ethernet_ip;
        EEPROM_READ(ethernet_dns);     ethernet.myDns   = ethernet_dns;
        EEPROM_READ(ethernet_gateway); ethernet.gateway = ethernet_gateway;
        EEPROM_READ(ethernet_subnet);  ethernet.subnet  = ethernet_subnet;
      #endif

      //
      // Validate Final Size and CRC
      //
      eeprom_error = size_error(eeprom_index - (EEPROM_OFFSET));
      if (eeprom_error) {
        DEBUG_ECHO_MSG("Index: ", eeprom_index - (EEPROM_OFFSET), " Size: ", datasize());
      }
      else if (working_crc != stored_crc) {
        eeprom_error = true;
        DEBUG_ERROR_MSG("EEPROM CRC mismatch - (stored) ", stored_crc, " != ", working_crc, " (calculated)!");
        TERN_(HOST_EEPROM_CHITCHAT, hostui.notify(GET_TEXT_F(MSG_ERR_EEPROM_CRC)));
      }
      else if (!validating) {
        DEBUG_ECHO_START();
        DEBUG_ECHO(version);
        DEBUG_ECHOLNPGM(" stored settings retrieved (", eeprom_index - (EEPROM_OFFSET), " bytes; crc ", (uint32_t)working_crc, ")");
        TERN_(HOST_EEPROM_CHITCHAT, hostui.notify(F("Stored settings retrieved")));
      }

      if (!validating && !eeprom_error) postprocess();
    }

    #if ENABLED(EEPROM_CHITCHAT) && DISABLED(DISABLE_M503)
      // Report the EEPROM settings
      if (!validating && TERN1(EEPROM_BOOT_SILENT, IsRunning())) report();
    #endif

    EEPROM_FINISH();

    return !eeprom_error;
  }

  #ifdef ARCHIM2_SPI_FLASH_EEPROM_BACKUP_SIZE
    extern bool restoreEEPROM();
  #endif

  bool MarlinSettings::validate() {
    validating = true;
    #ifdef ARCHIM2_SPI_FLASH_EEPROM_BACKUP_SIZE
      bool success = _load();
      if (!success && restoreEEPROM()) {
        SERIAL_ECHOLNPGM("Recovered backup EEPROM settings from SPI Flash");
        success = _load();
      }
    #else
      const bool success = _load();
    #endif
    validating = false;
    return success;
  }

  bool MarlinSettings::load() {
    if (validate()) {
      const bool success = _load();
      return success;
    }
    reset();
    #if EITHER(EEPROM_AUTO_INIT, EEPROM_INIT_NOW)
      (void)save();
      SERIAL_ECHO_MSG("EEPROM Initialized");
    #endif
    return false;
  }

#else // !EEPROM_SETTINGS

  bool MarlinSettings::save() {
    DEBUG_ERROR_MSG("EEPROM disabled");
    return false;
  }

#endif // !EEPROM_SETTINGS

/**
 * M502 - Reset Configuration
 */
void MarlinSettings::reset() {

  //
  // Filament Runout Sensor
  //

  #if HAS_FILAMENT_SENSOR
    runout.enabled = FIL_RUNOUT_ENABLED_DEFAULT;
    runout.reset();
    TERN_(HAS_FILAMENT_RUNOUT_DISTANCE, runout.set_runout_distance(FILAMENT_RUNOUT_DISTANCE_MM));
  #endif

  //
  // Case Light Brightness
  //
  TERN_(CASELIGHT_USES_BRIGHTNESS, caselight.brightness = CASE_LIGHT_DEFAULT_BRIGHTNESS);

  //
  // User-Defined Thermistors
  //
  TERN_(HAS_USER_THERMISTORS, tempSensors.reset_user_thermistors());

  //
  // Power Monitor
  //
  TERN_(POWER_MONITOR, power_monitor.reset());

  postprocess();

  #if EITHER(EEPROM_CHITCHAT, DEBUG_LEVELING_FEATURE)
    FSTR_P const hdsl = F("Hardcoded Default Settings Loaded");
    TERN_(HOST_EEPROM_CHITCHAT, hostui.notify(hdsl));
    DEBUG_ECHO_START(); DEBUG_ECHOLNF(hdsl);
  #endif
}

#if DISABLED(DISABLE_M503)

  #define CONFIG_ECHO_START()       gcode.report_echo_start(forReplay)
  #define CONFIG_ECHO_MSG(V...)     do{ CONFIG_ECHO_START(); SERIAL_ECHOLNPGM(V); }while(0)
  #define CONFIG_ECHO_MSG_P(V...)   do{ CONFIG_ECHO_START(); SERIAL_ECHOLNPGM_P(V); }while(0)
  #define CONFIG_ECHO_HEADING(STR)  gcode.report_heading(forReplay, F(STR))

  void M92_report(const bool echo=true, const int8_t e=-1);

  /**
   * M503 - Report current settings in RAM
   *
   * Unless specifically disabled, M503 is available even without EEPROM
   */
  void MarlinSettings::report(const bool forReplay) {
    //
    // Announce current units, in case inches are being displayed
    //
    CONFIG_ECHO_HEADING("Linear Units");
    CONFIG_ECHO_START();
    #if ENABLED(INCH_MODE_SUPPORT)
      SERIAL_ECHOPGM("  G2", AS_DIGIT(parser.linear_unit_factor == 1.0), " ;");
    #else
      SERIAL_ECHOPGM("  G21 ;");
    #endif
    gcode.say_units(); // " (in/mm)"

    //
    // M149 Temperature units
    //
    #if ENABLED(TEMPERATURE_UNITS_SUPPORT)
      gcode.M149_report(forReplay);
    #else
      CONFIG_ECHO_HEADING(STR_TEMPERATURE_UNITS);
      CONFIG_ECHO_MSG("  M149 C ; Units in Celsius");
    #endif

    //
    // Filament Runout Sensor
    //
    TERN_(HAS_FILAMENT_SENSOR, gcode.M412_report(forReplay));

    #if HAS_ETHERNET
      CONFIG_ECHO_HEADING("Ethernet");
      if (!forReplay) ETH0_report();
      CONFIG_ECHO_START(); SERIAL_ECHO_SP(2); MAC_report();
      CONFIG_ECHO_START(); SERIAL_ECHO_SP(2); gcode.M552_report();
      CONFIG_ECHO_START(); SERIAL_ECHO_SP(2); gcode.M553_report();
      CONFIG_ECHO_START(); SERIAL_ECHO_SP(2); gcode.M554_report();
    #endif
  }

#endif // !DISABLE_M503

#pragma pack(pop)
