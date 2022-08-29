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

#include <math.h>
#include <stddef.h>

#include "../inc/MarlinConfigPre.h"

//
// Conditional type assignment magic. For example...
//
// typename IF<(MYOPT==12), int, float>::type myvar;
//
template <bool, class L, class R>
struct IF { typedef R type; };
template <class L, class R>
struct IF<true, L, R> { typedef L type; };

#define NUM_TEMP_SENSORS_GANG(V...) GANG_N(NUM_TEMP_SENSORS, V)
#define NUM_TEMP_SENSORS_CODE(V...) CODE_N(NUM_TEMP_SENSORS, V)
#define NUM_TEMP_SENSORS_LIST(V...) LIST_N(NUM_TEMP_SENSORS, V)

#if HAS_TEMP_SENSORS
  #define LIST_ITEM_E(N) , N
  #define GANG_ITEM_E(N) N
#else
  #define LIST_ITEM_E(N)
  #define GANG_ITEM_E(N)
#endif

// General Flags for some number of states
template<size_t N>
struct Flags {
  typedef typename IF<(N>8), uint16_t, uint8_t>::type bits_t;
  typedef struct { bool b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; } N8;
  typedef struct { bool b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1, b8:1, b9:1, b10:1, b11:1, b12:1, b13:1, b14:1, b15:1; } N16;
  union {
    bits_t b;
    typename IF<(N>8), N16, N8>::type flag;
  };
  void reset()                             { b = 0; }
  void set(const int n, const bool onoff)  { onoff ? set(n) : clear(n); }
  void set(const int n)                    { b |=  (bits_t)_BV(n); }
  void clear(const int n)                  { b &= ~(bits_t)_BV(n); }
  bool test(const int n) const             { return TEST(b, n); }
  const bool operator[](const int n)       { return test(n); }
  const bool operator[](const int n) const { return test(n); }
  int size() const                         { return sizeof(b); }
};

// Specialization for a single bool flag
template<>
struct Flags<1> {
  bool b;
  void reset()                            { b = false; }
  void set(const int n, const bool onoff) { onoff ? set(n) : clear(n); }
  void set(const int)                     { b = true; }
  void clear(const int)                   { b = false; }
  bool test(const int) const              { return b; }
  bool& operator[](const int)             { return b; }
  bool  operator[](const int) const       { return b; }
  int size() const                        { return sizeof(b); }
};

typedef IF<(NUM_TEMP_SENSORS > 8), uint16_t, uint8_t>::type axis_bits_t;

//
// Loop over axes
//
#define LOOP_ABC(VAR) LOOP_S_LE_N(VAR, A_AXIS, C_AXIS)
#define LOOP_NUM_AXES(VAR) LOOP_S_L_N(VAR, X_AXIS, NUM_AXES)
#define LOOP_LOGICAL_AXES(VAR) LOOP_S_L_N(VAR, X_AXIS, LOGICAL_AXES)
#define LOOP_DISTINCT_AXES(VAR) LOOP_S_L_N(VAR, X_AXIS, DISTINCT_AXES)
#define LOOP_DISTINCT_E(VAR) LOOP_L_N(VAR, DISTINCT_E)

//
// feedRate_t is just a humble float
//
typedef float feedRate_t;

//
// celsius_t is the native unit of temperature. Signed to handle a disconnected thermistor value (-14).
// For more resolition (e.g., for a chocolate printer) this may later be changed to Celsius x 100
//
typedef uint16_t raw_adc_t;
typedef int16_t celsius_t;
typedef float celsius_float_t;

//
// On AVR pointers are only 2 bytes so use 'const float &' for 'const float'
//
#ifdef __AVR__
  typedef const float & const_float_t;
#else
  typedef const float const_float_t;
#endif
typedef const_float_t const_celsius_float_t;

// Conversion macros
#define MMM_TO_MMS(MM_M) feedRate_t(static_cast<float>(MM_M) / 60.0f)
#define MMS_TO_MMM(MM_S) (static_cast<float>(MM_S) * 60.0f)
