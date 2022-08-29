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
 * digital_inputs.h - manages endstops
 */

#include "../inc/MarlinConfig.h"
#include <stdint.h>

class DigitalInputs {
  public:

    typedef IF<(NUM_DIGITAL_INPUTS > 8), uint16_t, uint8_t>::type endstop_mask_t;

  private:
    static bool enabled, enabled_globally;
    static endstop_mask_t live_state;
    static volatile endstop_mask_t hit_state; // Use X_MIN, Y_MIN, Z_MIN and Z_MIN_PROBE as BIT index

    #if DIGITAL_INPUT_NOISE_THRESHOLD
      static endstop_mask_t validated_live_state;
      static uint8_t endstop_poll_count;    // Countdown from threshold for polling
    #endif

  public:
    DigitalInputs() {};

    /**
     * Initialize the endstop pins
     */
    static void init();

    /**
     * Are endstops or the probe set to abort the move?
     */
    FORCE_INLINE static bool abort_enabled() {
      return enabled;
    }

    static bool global_enabled() { return enabled_globally; }

    /**
     * Periodic call to poll endstops if required. Called from temperature ISR
     */
    static void poll();

    /**
     * Update endstops bits from the pins. Apply filtering to get a verified state.
     * If abort_enabled() and moving towards a triggered switch, abort the current move.
     * Called from ISR contexts.
     */
    static void update();

    /**
     * Get Endstop hit state.
     */
    FORCE_INLINE static endstop_mask_t trigger_state() { return hit_state; }

    /**
     * Get current endstops state
     */
    FORCE_INLINE static endstop_mask_t state() {
      return
        #if DIGITAL_INPUT_NOISE_THRESHOLD
          validated_live_state
        #else
          live_state
        #endif
      ;
    }

    /**
     * Report endstop hits to serial. Called from loop().
     */
    static void event_handler();

    /**
     * Report endstop states in response to M119
     */
    static void report_states();

    // Enable / disable endstop checking globally
    static void enable_globally(const bool onoff=true);

    // Enable / disable endstop checking
    static void enable(const bool onoff=true);

    // Disable / Enable endstops based on ENSTOPS_ONLY_FOR_HOMING and global enable
    static void not_homing();

    FORCE_INLINE static void validate_homing_move() { hit_on_purpose(); }

    // Clear endstops (i.e., they were hit intentionally) to suppress the report
    FORCE_INLINE static void hit_on_purpose() { hit_state = 0; }

    // Enable / disable endstop z-probe checking
    #if HAS_BED_PROBE
      static volatile bool z_probe_enabled;
      static void enable_z_probe(const bool onoff=true);
    #endif

    static void resync();

    // Debugging of endstops
    #if ENABLED(PINS_DEBUGGING)
      static bool monitor_flag;
      static void monitor();
      static void run_monitor();
    #endif

  public:
    // Basic functions for Sensorless Homing
    #if USE_SENSORLESS
      static void set_homing_current(const bool onoff);
    #endif
};

extern DigitalInputs digitalInputs;

/**
 * A class to save and change the endstop state,
 * then restore it when it goes out of scope.
 */
class TemporaryGlobalEndstopsState {
  bool saved;

  public:
    TemporaryGlobalEndstopsState(const bool enable) : saved(digitalInputs.global_enabled()) {
      digitalInputs.enable_globally(enable);
    }
    ~TemporaryGlobalEndstopsState() { digitalInputs.enable_globally(saved); }
};
