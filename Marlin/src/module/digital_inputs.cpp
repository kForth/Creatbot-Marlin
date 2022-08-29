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
 * digital_inputs.cpp - A singleton object to manage endstops
 */

#include "digital_inputs.h"

#include "temperature.h"

#include "../core/debug_out.h"

DigitalInputs digitalInputs;

// private:

bool DigitalInputs::enabled, DigitalInputs::enabled_globally; // Initialized by settings.load()

volatile DigitalInputs::endstop_mask_t DigitalInputs::hit_state;
DigitalInputs::endstop_mask_t DigitalInputs::live_state = 0;

#define DI_PIN(N)      DIGITAL_INPUT_##N##_PIN
#define DI_PULLUP(N)   ENABLED(DIGITAL_INPUT_PULLUP_##N)
#define DI_PULLDOWN(N) ENABLED(DIGITAL_INPUT_PULLDOWN_##N)
#define _IF_DI(N, S)   TERN_(NUM_DIGITAL_INPUTS >= INCREMENT(N), S(N))
#define CALL_PER_DI(S)      _IF_DI(0, S); _IF_DI(1, S); _IF_DI(2, S); _IF_DI(3, S); \
                       _IF_DI(4, S); _IF_DI(5, S); _IF_DI(6, S); _IF_DI(7, S);

#if DIGITAL_INPUT_NOISE_THRESHOLD
  Endstops::endstop_mask_t Endstops::validated_live_state;
  uint8_t Endstops::endstop_poll_count;
#endif

/**
 * Class and Instance Methods
 */

void DigitalInputs::init() {

  #define DI_SETUP(N) IF(DI_PULLUP(N), SET_INPUT_PULLUP(N), IF(DI_PULLDOWN(N), SET_INPUT_PULLDOWN(N), SET_INPUT(N)))

  CALL_PER_DI(DI_SETUP);

  // Enable endstops
  enable_globally(true);

} // Endstops::init

// Called at ~1kHz from Temperature ISR: Poll endstop state if required
void DigitalInputs::poll() {

  TERN_(PINS_DEBUGGING, run_monitor()); // Report changes in endstop status

  update();
}

void DigitalInputs::enable_globally(const bool onoff) {
  enabled_globally = enabled = onoff;
  resync();
}

// Enable / disable endstop checking
void DigitalInputs::enable(const bool onoff) {
  enabled = onoff;
  resync();
}

// Disable / Enable endstops based on ENSTOPS_ONLY_FOR_HOMING and global enable
void DigitalInputs::not_homing() {
  enabled = enabled_globally;
}

// Get the stable endstop states when enabled
void DigitalInputs::resync() {
  if (!abort_enabled()) return;     // If endstops/probes are disabled the loop below can hang

  // Wait for Temperature ISR to run at least once (runs at 1kHz)
  while (TERN0(DIGITAL_INPUT_NOISE_THRESHOLD, endstop_poll_count)) safe_delay(1);
}

#if ENABLED(PINS_DEBUGGING)
  void DigitalInputs::run_monitor() {
    if (!monitor_flag) return;
    static uint8_t monitor_count = 16;  // offset this check from the others
    monitor_count += _BV(1);            //  15 Hz
    monitor_count &= 0x7F;
    if (!monitor_count) monitor();      // report changes in endstop status
  }
#endif

void DigitalInputs::event_handler() {
  static endstop_mask_t prev_hit_state; // = 0
  if (hit_state == prev_hit_state) return;
  prev_hit_state = hit_state;
  if (hit_state) {

    #define DI_HIT_ECHO(N) do { SERIAL_ECHO(" " STRINGIFY(N)); } while(0)
    #define DI_HIT_TEST(N) if (TEST(hit_state, N)) { DI_HIT_ECHO(N); }

    SERIAL_ECHO_START();
    SERIAL_ECHOPGM(STR_DI_HIT_LIST);
    CALL_PER_DI(DI_HIT_TEST);
    SERIAL_EOL();

  }
}

#pragma GCC diagnostic push
#if GCC_VERSION <= 50000
  #pragma GCC diagnostic ignored "-Wunused-function"
#endif

static void print_es_state(const bool is_hit, FSTR_P const flabel=nullptr) {
  if (flabel) SERIAL_ECHOF(flabel);
  SERIAL_ECHOPGM(": ");
  SERIAL_ECHOLNF(is_hit ? F(STR_DI_HIT) : F(STR_DI_OPEN));
}

#pragma GCC diagnostic pop

void __O2 DigitalInputs::report_states() {
  SERIAL_ECHOLNPGM(STR_M119_REPORT);

  #define DI_REPORT(N) print_es_state(READ(DI_PIN(N)), F(STRINGIFY(N)))

  CALL_PER_DI(DI_REPORT)

  #if MULTI_FILAMENT_SENSOR
    #define _CASE_RUNOUT(N) case N: pin = FIL_RUNOUT##N##_PIN; state = FIL_RUNOUT##N##_STATE; break;
    LOOP_S_LE_N(i, 1, NUM_RUNOUT_SENSORS) {
      pin_t pin;
      uint8_t state;
      switch (i) {
        default: continue;
        REPEAT_1(NUM_RUNOUT_SENSORS, _CASE_RUNOUT)
      }
      SERIAL_ECHOPGM(STR_FILAMENT);
      if (i > 1) SERIAL_CHAR(' ', '0' + i);
      print_es_state(extDigitalRead(pin) != state);
    }
    #undef _CASE_RUNOUT
  #elif HAS_FILAMENT_SENSOR
    print_es_state(READ(FIL_RUNOUT1_PIN) != FIL_RUNOUT1_STATE, F(STR_FILAMENT));
  #endif

} // Endstops::report_states


/**
 * Called from interrupt context by the Endstop ISR or Stepper ISR!
 * Read endstops to get their current states, register hits for all
 * axes moving in the direction of their endstops, and abort moves.
 */
void DigitalInputs::update() {

  #if !DIGITAL_INPUT_NOISE_THRESHOLD      // If not debouncing...
    if (!abort_enabled()) return;   // ...and not enabled, exit.
  #endif

  // Macros to update the live_state
  #define _UPDATE_BIT(N) SET_BIT_TO(live_state, N, (READ(DI_PIN(N))))
  #define UPDATE_BIT(N) TERN_(NUM_DIGITAL_INPUTS >= (N + 1), _UPDATE_BIT(N))

  /**
   * Check and update endstops
   */
  CALL_PER_DI(UPDATE_BIT);

  #if DIGITAL_INPUT_NOISE_THRESHOLD

    /**
     * Filtering out noise on endstops requires a delayed decision. Let's assume, due to noise,
     * that 50% of endstop signal samples are good and 50% are bad (assuming normal distribution
     * of random noise). Then the first sample has a 50% chance to be good or bad. The 2nd sample
     * also has a 50% chance to be good or bad. The chances of 2 samples both being bad becomes
     * 50% of 50%, or 25%. That was the previous implementation of Marlin endstop handling. It
     * reduces chances of bad readings in half, at the cost of 1 extra sample period, but chances
     * still exist. The only way to reduce them further is to increase the number of samples.
     * To reduce the chance to 1% (1/128th) requires 7 samples (adding 7ms of delay).
     */
    static endstop_mask_t old_live_state;
    if (old_live_state != live_state) {
      endstop_poll_count = DIGITAL_INPUT_NOISE_THRESHOLD;
      old_live_state = live_state;
    }
    else if (endstop_poll_count && !--endstop_poll_count)
      validated_live_state = live_state;

    if (!abort_enabled()) return;

  #endif

  // Test the current status of an endstop
  #define TEST_DI(N) (TEST(state(), N))

  // Record endstop was hit
  #define _DI_HIT(N) SBI(hit_state, N)

  // Call the endstop triggered routine for single endstops
  #define _PROCESS_DI(N) do { \
    if (TEST_DI(N)) { \
      _DI_HIT(N); \
    } \
  }while(0)
  #define PROCESS_DI(N) TERN_(NUM_DIGITAL_INPUTS >= (N + 1), _PROCESS_DI(N))

  // Signal, after validation, if an endstop limit is pressed or not

  CALL_PER_DI(PROCESS_DI);
  
} // Endstops::update()

#if ENABLED(PINS_DEBUGGING)

  bool DigitalInputs::monitor_flag = false;

  /**
   * Monitor Endstops and Z Probe for changes
   *
   * If a change is detected then the LED is toggled and
   * a message is sent out the serial port.
   *
   * Yes, we could miss a rapid back & forth change but
   * that won't matter because this is all manual.
   */
  void DigitalInputs::monitor() {

    static uint16_t old_live_state_local = 0;
    static uint8_t local_LED_status = 0;
    uint16_t live_state_local = 0;

    #define GET_DI_STATE(N) if (READ(DI_PIN(N))) { SBI(live_state_local, N); }

    CALL_PER_DI(GET_DI_STATE);

    uint16_t endstop_change = live_state_local ^ old_live_state_local;

    if (endstop_change) {
      #define DI_REPORT_CHANGE(N) if (TEST(endstop_change, N)) SERIAL_ECHOPGM("  " STRINGIFY(N) ":", TEST(live_state_local, N))

      CALL_PER_DI(DI_REPORT_CHANGE)

      SERIAL_ECHOLNPGM("\n");
      hal.set_pwm_duty(pin_t(LED_PIN), local_LED_status);
      local_LED_status ^= 255;
      old_live_state_local = live_state_local;
    }
  }

#endif // PINS_DEBUGGING
