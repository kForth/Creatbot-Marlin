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
 * temperature.h - temperature controller
 */

#include "thermistor/thermistors.h"

#include "../inc/MarlinConfig.h"

#if ENABLED(AUTO_REPORT_SENSORS)
  #include "../libs/autoreport.h"
#endif

#define HOTEND_INDEX TERN(HAS_MULTI_TEMP_SENSORS, e, 0)
#define E_NAME TERN_(HAS_MULTI_TEMP_SENSORS, e)

// Element identifiers. Positive values are hotends. Negative values are other heaters or coolers.
typedef enum : int8_t {
  H_E0 = HID_E0, H_E1, H_E2, H_E3, H_E4, H_E5, H_E6, H_E7,
  H_NONE = -128
} heater_id_t;

/**
 * States for ADC reading in the ISR
 */

enum ADCSensorState : char {
  StartSampling,

  #define ADC_STATE(N) PrepareTemp_##N, MeasureTemp_##N,
  #if HAS_TEMP_SENSOR(0) && HAS_ADC_TEST(0)
    ADC_STATE(0)
  #endif
  #if HAS_TEMP_SENSOR(1) && HAS_ADC_TEST(1)
    ADC_STATE(1)
  #endif
  #if HAS_TEMP_SENSOR(2) && HAS_ADC_TEST(2)
    ADC_STATE(2)
  #endif
  #if HAS_TEMP_SENSOR(3) && HAS_ADC_TEST(3)
    ADC_STATE(3)
  #endif
  #if HAS_TEMP_SENSOR(4) && HAS_ADC_TEST(4)
    ADC_STATE(4)
  #endif
  #if HAS_TEMP_SENSOR(5) && HAS_ADC_TEST(5)
    ADC_STATE(5)
  #endif
  #if HAS_TEMP_SENSOR(6) && HAS_ADC_TEST(6)
    ADC_STATE(6)
  #endif
  #if HAS_TEMP_SENSOR(7) && HAS_ADC_TEST(7)
    ADC_STATE(7)
  #endif
  #undef ADC_STATE
  
  #if ENABLED(FILAMENT_WIDTH_SENSOR)
    Prepare_FILWIDTH, Measure_FILWIDTH,
  #endif
  #if ENABLED(POWER_MONITOR_CURRENT)
    Prepare_POWER_MONITOR_CURRENT,
    Measure_POWER_MONITOR_CURRENT,
  #endif
  #if ENABLED(POWER_MONITOR_VOLTAGE)
    Prepare_POWER_MONITOR_VOLTAGE,
    Measure_POWER_MONITOR_VOLTAGE,
  #endif
  SensorsReady, // Temperatures ready. Delay the next round of readings to let ADC pins settle.
  StartupDelay  // Startup, delay initial temp reading a tiny bit so the hardware can settle
};
#undef _ADC_STATE
#undef ADC_STATE

// Minimum number of Temperature::ISR loops between sensor readings.
// Multiplied by 16 (OVERSAMPLENR) to obtain the total time to
// get all oversampled sensor readings
#define MIN_ADC_ISR_LOOPS 10

#define ACTUAL_ADC_SAMPLES _MAX(int(MIN_ADC_ISR_LOOPS), int(SensorsReady))

// A temperature sensor
typedef struct TempInfo {
private:
  raw_adc_t acc;
  raw_adc_t raw;
public:
  celsius_float_t celsius;
  inline void reset() { acc = 0; }
  inline void sample(const raw_adc_t s) { acc += s; }
  inline void update() { raw = acc; }
  void setraw(const raw_adc_t r) { raw = r; }
  raw_adc_t getraw() { return raw; }
} temp_info_t;


// Temperature sensor read value ranges
typedef struct { raw_adc_t raw_min, raw_max; celsius_t mintemp, maxtemp; } temp_range_t;

#define THERMISTOR_ABS_ZERO_C           -273.15f  // bbbbrrrrr cold !
#define THERMISTOR_RESISTANCE_NOMINAL_C 25.0f     // mmmmm comfortable

#if HAS_USER_THERMISTORS
  enum CustomThermistorIndex : uint8_t {
    #define CUSTOM_TI(N) CUSTOM_TI_##N,
    #if TEMP_SENSOR_IS_CUSTOM(0)
      CUSTOM_TI(0)
    #endif
    #if TEMP_SENSOR_IS_CUSTOM(1)
      CUSTOM_TI(1)
    #endif
    #if TEMP_SENSOR_IS_CUSTOM(2)
      CUSTOM_TI(2)
    #endif
    #if TEMP_SENSOR_IS_CUSTOM(3)
      CUSTOM_TI(3)
    #endif
    #if TEMP_SENSOR_IS_CUSTOM(4)
      CUSTOM_TI(4)
    #endif
    #if TEMP_SENSOR_IS_CUSTOM(5)
      CUSTOM_TI(5)
    #endif
    #if TEMP_SENSOR_IS_CUSTOM(6)
      CUSTOM_TI(6)
    #endif
    #if TEMP_SENSOR_IS_CUSTOM(7)
      CUSTOM_TI(7)
    #endif
    #undef CUSTOM_TI
    USER_THERMISTORS
  };

  // User-defined thermistor
  typedef struct {
    bool pre_calc;     // true if pre-calculations update needed
    float sh_c_coeff,  // Steinhart-Hart C coefficient .. defaults to '0.0'
          sh_alpha,
          series_res,
          res_25, res_25_recip,
          res_25_log,
          beta, beta_recip;
  } user_thermistor_t;

#endif

class Temperature {

  public:

    #if HAS_TEMP_SENSORS
      static TempInfo temp_hotend[NUM_TEMP_SENSORS];
      static temp_range_t temp_range[NUM_TEMP_SENSORS];
    #endif

  public:
    /**
     * Instance Methods
     */

    void init();

    /**
     * Static (class) methods
     */

    #if HAS_USER_THERMISTORS
      static user_thermistor_t user_thermistor[USER_THERMISTORS];
      static void M305_report(const uint8_t t_index, const bool forReplay=true);
      static void reset_user_thermistors();
      static celsius_float_t user_thermistor_to_deg_c(const uint8_t t_index, const raw_adc_t raw);
      static bool set_pull_up_res(int8_t t_index, float value) {
        //if (!WITHIN(t_index, 0, USER_THERMISTORS - 1)) return false;
        if (!WITHIN(value, 1, 1000000)) return false;
        user_thermistor[t_index].series_res = value;
        return true;
      }
      static bool set_res25(int8_t t_index, float value) {
        if (!WITHIN(value, 1, 10000000)) return false;
        user_thermistor[t_index].res_25 = value;
        user_thermistor[t_index].pre_calc = true;
        return true;
      }
      static bool set_beta(int8_t t_index, float value) {
        if (!WITHIN(value, 1, 1000000)) return false;
        user_thermistor[t_index].beta = value;
        user_thermistor[t_index].pre_calc = true;
        return true;
      }
      static bool set_sh_coeff(int8_t t_index, float value) {
        if (!WITHIN(value, -0.01f, 0.01f)) return false;
        user_thermistor[t_index].sh_c_coeff = value;
        user_thermistor[t_index].pre_calc = true;
        return true;
      }
    #endif

    #if HAS_TEMP_SENSORS
      static celsius_float_t analog_to_celsius_hotend(const raw_adc_t raw, const uint8_t e);
    #endif

    /**
     * Called from the Temperature ISR
     */
    static void isr();
    static void readings_ready();

    /**
     * Call periodically to manage heaters and keep the watchdog fed
     */
    static void task();

    //high level conversion routines, for use outside of temperature.cpp
    //inline so that there is no performance decrease.
    //deg=degreeCelsius

    static celsius_float_t degHotend(const uint8_t E_NAME) {
      return TERN0(HAS_TEMP_SENSORS, temp_hotend[HOTEND_INDEX].celsius);
    }

    static celsius_t wholeDegHotend(const uint8_t E_NAME) {
      return TERN0(HAS_TEMP_SENSORS, static_cast<celsius_t>(temp_hotend[HOTEND_INDEX].celsius + 0.5f));
    }

    #if ENABLED(SHOW_TEMP_ADC_VALUES)
      static raw_adc_t rawHotendTemp(const uint8_t E_NAME) {
        return TERN0(HAS_TEMP_SENSORS, temp_hotend[HOTEND_INDEX].getraw());
      }
    #endif

    #if HAS_TEMP_SENSORS

      static bool degHotendNear(const uint8_t e, const celsius_t temp, const celsius_t hysteresis) {
        return ABS(wholeDegHotend(e) - temp) < hysteresis;
      }
      
      static void print_heater_states(const int8_t target_extruder
        OPTARG(HAS_TEMP_REDUNDANT, const bool include_r=false)
      );
      #if ENABLED(AUTO_REPORT_SENSORS)
        struct AutoReportTemp { static void report(); };
        static AutoReporter<AutoReportTemp> auto_reporter;
      #endif
    #endif

  private:

    // Reading raw temperatures and converting to Celsius when ready
    static volatile bool raw_temps_ready;
    static void update_raw_temperatures();
    static void updateTemperaturesFromRawValues();
    static bool updateTemperaturesIfReady() {
      if (!raw_temps_ready) return false;
      updateTemperaturesFromRawValues();
      raw_temps_ready = false;
      return true;
    }
};

extern Temperature tempSensors;
