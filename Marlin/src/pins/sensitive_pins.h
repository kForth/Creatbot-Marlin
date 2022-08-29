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

//
// Prepare a list of protected pins for M42/M43
//

//
// Temp Sensors
//

#define _T0_PINS
#define _T1_PINS
#define _T2_PINS
#define _T3_PINS
#define _T4_PINS
#define _T5_PINS
#define _T6_PINS
#define _T7_PINS

#define DIO_PIN(P) TERN(TARGET_LPC1768, P, analogInputToDigitalPin(P))

#if HAS_TEMP_SENSORS
  #undef _T0_PINS
  #define _T0_PINS DIO_PIN(TEMP_0_PIN),
  #if HAS_MULTI_TEMP_SENSORS
    #undef _T1_PINS
    #define _T1_PINS DIO_PIN(TEMP_1_PIN),
    #if NUM_TEMP_SENSORS > 2
      #undef _T2_PINS
      #define _T2_PINS DIO_PIN(TEMP_2_PIN),
      #if NUM_TEMP_SENSORS > 3
        #undef _T3_PINS
        #define _T3_PINS DIO_PIN(TEMP_3_PIN),
        #if NUM_TEMP_SENSORS > 4
          #undef _T4_PINS
          #define _T4_PINS DIO_PIN(TEMP_4_PIN),
          #if NUM_TEMP_SENSORS > 5
            #undef _T5_PINS
            #define _T5_PINS DIO_PIN(TEMP_5_PIN),
            #if NUM_TEMP_SENSORS > 6
              #undef _T6_PINS
              #define _T6_PINS DIO_PIN(TEMP_6_PIN),
              #if NUM_TEMP_SENSORS > 7
                #undef _T7_PINS
                #define _T7_PINS DIO_PIN(TEMP_7_PIN),
              #endif // NUM_TEMP_SENSORS > 7
            #endif // NUM_TEMP_SENSORS > 6
          #endif // NUM_TEMP_SENSORS > 5
        #endif // NUM_TEMP_SENSORS > 4
      #endif // NUM_TEMP_SENSORS > 3
    #endif // NUM_TEMP_SENSORS > 2
  #endif // HAS_MULTI_TEMP_SENSORS
#endif // NUM_TEMP_SENSORS

//
// Generate the final Sensitive Pins array,
// keeping the array as small as possible.
//

#ifndef HAL_SENSITIVE_PINS
  #define HAL_SENSITIVE_PINS
#endif

#ifdef RUNTIME_ONLY_ANALOG_TO_DIGITAL
  #define _SP_END
#else
  #define _SP_END -2

  // Move a regular pin in front to the end
  template<pin_t F, pin_t ...D>
  struct OnlyPins : OnlyPins<D..., F> { };

  // Remove a -1 from the front
  template<pin_t ...D>
  struct OnlyPins<-1, D...> : OnlyPins<D...> { };

  // Remove -2 from the front, emit the rest, cease propagation
  template<pin_t ...D>
  struct OnlyPins<_SP_END, D...> { static constexpr size_t size = sizeof...(D); static constexpr pin_t table[sizeof...(D)] PROGMEM = { D... }; };
#endif

#define SENSITIVE_PINS \
  _T0_PINS _T1_PINS _T2_PINS _T3_PINS _T4_PINS _T5_PINS _T6_PINS _T7_PINS \
  HAL_SENSITIVE_PINS _SP_END
