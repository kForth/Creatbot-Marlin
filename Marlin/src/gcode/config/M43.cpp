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

#include "../../inc/MarlinConfig.h"

#if ENABLED(PINS_DEBUGGING)

#include "../gcode.h"
#include "../../MarlinCore.h" // for pin_is_protected
#include "../../pins/pinsDebug.h"
#include "../../module/digital_inputs.h"

#ifndef GET_PIN_MAP_PIN_M43
  #define GET_PIN_MAP_PIN_M43(Q) GET_PIN_MAP_PIN(Q)
#endif

inline void toggle_pins() {
  const bool ignore_protection = parser.boolval('I');
  const int repeat = parser.intval('R', 1),
            start = PARSED_PIN_INDEX('S', 0),
            end = PARSED_PIN_INDEX('L', NUM_DIGITAL_PINS - 1),
            wait = parser.intval('W', 500);

  LOOP_S_LE_N(i, start, end) {
    pin_t pin = GET_PIN_MAP_PIN_M43(i);
    if (!VALID_PIN(pin)) continue;
    if (M43_NEVER_TOUCH(i) || (!ignore_protection && pin_is_protected(pin))) {
      report_pin_state_extended(pin, ignore_protection, true, F("Untouched "));
      SERIAL_EOL();
    }
    else {
      hal.watchdog_refresh();
      report_pin_state_extended(pin, ignore_protection, true, F("Pulsing   "));
      #ifdef __STM32F1__
        const auto prior_mode = _GET_MODE(i);
      #else
        const bool prior_mode = GET_PINMODE(pin);
      #endif
      #if AVR_AT90USB1286_FAMILY // Teensy IDEs don't know about these pins so must use FASTIO
        if (pin == TEENSY_E2) {
          SET_OUTPUT(TEENSY_E2);
          for (int16_t j = 0; j < repeat; j++) {
            WRITE(TEENSY_E2, LOW);  safe_delay(wait);
            WRITE(TEENSY_E2, HIGH); safe_delay(wait);
            WRITE(TEENSY_E2, LOW);  safe_delay(wait);
          }
        }
        else if (pin == TEENSY_E3) {
          SET_OUTPUT(TEENSY_E3);
          for (int16_t j = 0; j < repeat; j++) {
            WRITE(TEENSY_E3, LOW);  safe_delay(wait);
            WRITE(TEENSY_E3, HIGH); safe_delay(wait);
            WRITE(TEENSY_E3, LOW);  safe_delay(wait);
          }
        }
        else
      #endif
      {
        pinMode(pin, OUTPUT);
        for (int16_t j = 0; j < repeat; j++) {
          hal.watchdog_refresh(); extDigitalWrite(pin, 0); safe_delay(wait);
          hal.watchdog_refresh(); extDigitalWrite(pin, 1); safe_delay(wait);
          hal.watchdog_refresh(); extDigitalWrite(pin, 0); safe_delay(wait);
          hal.watchdog_refresh();
        }
      }
      #ifdef __STM32F1__
        _SET_MODE(i, prior_mode);
      #else
        pinMode(pin, prior_mode);
      #endif
    }
    SERIAL_EOL();
  }
  SERIAL_ECHOLNPGM(STR_DONE);

} // toggle_pins

/**
 * M43: Pin debug - report pin state, watch pins, toggle pins and servo probe test/report
 *
 *  M43         - report name and state of pin(s)
 *                  P<pin>  Pin to read or watch. If omitted, reads all pins.
 *                  I       Flag to ignore Marlin's pin protection.
 *
 *  M43 W       - Watch pins -reporting changes- until reset, click, or M108.
 *                  P<pin>  Pin to read or watch. If omitted, read/watch all pins.
 *                  I       Flag to ignore Marlin's pin protection.
 *
 *  M43 E<bool> - Enable / disable background endstop monitoring
 *                  - Machine continues to operate
 *                  - Reports changes to endstops
 *                  - Toggles LED_PIN when an endstop changes
 *                  - Cannot reliably catch the 5mS pulse from BLTouch type probes
 *
 *  M43 T       - Toggle pin(s) and report which pin is being toggled
 *                  S<pin>  - Start Pin number.   If not given, will default to 0
 *                  L<pin>  - End Pin number.   If not given, will default to last pin defined for this board
 *                  I<bool> - Flag to ignore Marlin's pin protection.   Use with caution!!!!
 *                  R       - Repeat pulses on each pin this number of times before continuing to next pin
 *                  W       - Wait time (in milliseconds) between pulses.  If not given will default to 500
 *
 */
void GcodeSuite::M43() {

  // 'T' must be first. It uses 'S' and 'E' differently.
  if (parser.seen('T')) return toggle_pins();

  // 'E' Enable or disable endstop monitoring and return
  if (parser.seen('E')) {
    digitalInputs.monitor_flag = parser.value_bool();
    SERIAL_ECHOPGM("endstop monitor ");
    SERIAL_ECHOF(digitalInputs.monitor_flag ? F("en") : F("dis"));
    SERIAL_ECHOLNPGM("abled");
    return;
  }

  // 'P' Get the range of pins to test or watch
  uint8_t first_pin = PARSED_PIN_INDEX('P', 0),
          last_pin = parser.seenval('P') ? first_pin : TERN(HAS_HIGH_ANALOG_PINS, NUM_DIGITAL_PINS, NUMBER_PINS_TOTAL) - 1;

  if (first_pin > last_pin) return;

  // 'I' to ignore protected pins
  const bool ignore_protection = parser.boolval('I');

  // 'W' Watch until click, M108, or reset
  if (parser.boolval('W')) {
    SERIAL_ECHOLNPGM("Watching pins");
    #ifdef ARDUINO_ARCH_SAM
      NOLESS(first_pin, 2); // Don't hijack the UART pins
    #endif
    uint8_t pin_state[last_pin - first_pin + 1];
    LOOP_S_LE_N(i, first_pin, last_pin) {
      pin_t pin = GET_PIN_MAP_PIN_M43(i);
      if (!VALID_PIN(pin)) continue;
      if (M43_NEVER_TOUCH(i) || (!ignore_protection && pin_is_protected(pin))) continue;
      pinMode(pin, INPUT_PULLUP);
      delay(1);
      /*
      if (IS_ANALOG(pin))
        pin_state[pin - first_pin] = analogRead(DIGITAL_PIN_TO_ANALOG_PIN(pin)); // int16_t pin_state[...]
      else
      //*/
        pin_state[i - first_pin] = extDigitalRead(pin);
    }

    for (;;) {
      LOOP_S_LE_N(i, first_pin, last_pin) {
        pin_t pin = GET_PIN_MAP_PIN_M43(i);
        if (!VALID_PIN(pin)) continue;
        if (M43_NEVER_TOUCH(i) || (!ignore_protection && pin_is_protected(pin))) continue;
        const byte val =
          /*
          IS_ANALOG(pin)
            ? analogRead(DIGITAL_PIN_TO_ANALOG_PIN(pin)) : // int16_t val
            :
          //*/
            extDigitalRead(pin);
        if (val != pin_state[i - first_pin]) {
          report_pin_state_extended(pin, ignore_protection, false);
          pin_state[i - first_pin] = val;
        }
      }

      safe_delay(200);
    }
  }
  else {
    // Report current state of selected pin(s)
    LOOP_S_LE_N(i, first_pin, last_pin) {
      pin_t pin = GET_PIN_MAP_PIN_M43(i);
      if (VALID_PIN(pin)) report_pin_state_extended(pin, ignore_protection, true);
    }
  }
}

#endif // PINS_DEBUGGING
