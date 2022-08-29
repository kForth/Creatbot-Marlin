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
 * temperature.cpp - temperature control
 */

// Useful when debugging thermocouples
//#define IGNORE_THERMOCOUPLE_ERRORS

#include "../MarlinCore.h"
#include "../HAL/shared/Delay.h"

#include "temperature.h"
#include "digital_inputs.h"

#if ENABLED(EMERGENCY_PARSER)
  #include "../feature/e_parser.h"
#endif

#if ENABLED(PRINTER_EVENT_LEDS)
  #include "../feature/leds/printer_event_leds.h"
#endif

#if ANY_TEMP_SENSOR_IS_THERMISTOR
  #define HAS_HOTEND_THERMISTOR 1
#endif

#if HAS_HOTEND_THERMISTOR
  #define NEXT_TEMPTABLE(N) ,TEMPTABLE(N)
  #define NEXT_TEMPTABLE_LEN(N) ,TEMPTABLE_LEN(N)
  static const temp_entry_t* heater_ttbl_map[NUM_TEMP_SENSORS] = ARRAY_BY_TEMP_SENSOR(TEMPTABLE(0) REPEAT_S(1, NUM_TEMP_SENSORS, NEXT_TEMPTABLE));
  static constexpr uint8_t heater_ttbllen_map[NUM_TEMP_SENSORS] = ARRAY_BY_TEMP_SENSOR(TEMPTABLE_LEN(0) REPEAT_S(1, NUM_TEMP_SENSORS, NEXT_TEMPTABLE_LEN));
#endif

Temperature tempSensors;

/**
 * public:
 */

#if HAS_TEMP_SENSORS
  TempInfo Temperature::temp_hotend[NUM_TEMP_SENSORS];
#endif

/**
 * private:
 */

volatile bool Temperature::raw_temps_ready = false;

#define TEMPDIR(N) (TEMP_SENSOR_RAW_LO_TEMP(N) < TEMP_SENSOR_RAW_HI_TEMP(N) ? 1 : -1)

#if HAS_TEMP_SENSORS
  // Init mintemp and maxtemp with extreme values to prevent false errors during startup
  constexpr temp_range_t temp_sensor_0 { TEMP_SENSOR_RAW_LO_TEMP(0), TEMP_SENSOR_RAW_HI_TEMP(0), 0, 16383 },
                         temp_sensor_1 { TEMP_SENSOR_RAW_LO_TEMP(1), TEMP_SENSOR_RAW_HI_TEMP(1), 0, 16383 },
                         temp_sensor_2 { TEMP_SENSOR_RAW_LO_TEMP(2), TEMP_SENSOR_RAW_HI_TEMP(2), 0, 16383 },
                         temp_sensor_3 { TEMP_SENSOR_RAW_LO_TEMP(3), TEMP_SENSOR_RAW_HI_TEMP(3), 0, 16383 },
                         temp_sensor_4 { TEMP_SENSOR_RAW_LO_TEMP(4), TEMP_SENSOR_RAW_HI_TEMP(4), 0, 16383 },
                         temp_sensor_5 { TEMP_SENSOR_RAW_LO_TEMP(5), TEMP_SENSOR_RAW_HI_TEMP(5), 0, 16383 },
                         temp_sensor_6 { TEMP_SENSOR_RAW_LO_TEMP(6), TEMP_SENSOR_RAW_HI_TEMP(6), 0, 16383 },
                         temp_sensor_7 { TEMP_SENSOR_RAW_LO_TEMP(7), TEMP_SENSOR_RAW_HI_TEMP(7), 0, 16383 };

  temp_range_t Temperature::temp_range[NUM_TEMP_SENSORS] = ARRAY_BY_TEMP_SENSOR(temp_sensor_0, temp_sensor_1, temp_sensor_2, temp_sensor_3, temp_sensor_4, temp_sensor_5, temp_sensor_6, temp_sensor_7);
#endif

/**
 * public:
 * Class and Instance Methods
 */

#define _EFANOVERLAP(A,B) _FANOVERLAP(E##A,B)

/**
 * Manage heating activities for extruder hot-ends and a heated bed
 *  - Acquire updated temperature readings
 *    - Also resets the watchdog timer
 *  - Invoke thermal runaway protection
 *  - Manage extruder auto-fan
 *  - Apply filament width to the extrusion rate (may move)
 *  - Update the heated bed PID output value
 */
void Temperature::task() {
  if (marlin_state == MF_INITIALIZING) return hal.watchdog_refresh(); // If Marlin isn't started, at least reset the watchdog!

  static bool no_reentry = false;  // Prevent recursion
  if (no_reentry) return;
  REMEMBER(mh, no_reentry, true);

  #if ENABLED(EMERGENCY_PARSER)
    if (emergency_parser.killed_by_M112) kill(FPSTR(M112_KILL_STR));
  #endif

  if (!updateTemperaturesIfReady()) return; // Will also reset the watchdog if temperatures are ready

  const millis_t ms = millis();

  UNUSED(ms);
}

#define TEMP_AD595(RAW)  ((RAW) * 5.0 * 100.0 / float(HAL_ADC_RANGE) / (OVERSAMPLENR) * (TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET)
#define TEMP_AD8495(RAW) ((RAW) * 6.6 * 100.0 / float(HAL_ADC_RANGE) / (OVERSAMPLENR) * (TEMP_SENSOR_AD8495_GAIN) + TEMP_SENSOR_AD8495_OFFSET)

/**
 * Bisect search for the range of the 'raw' value, then interpolate
 * proportionally between the under and over values.
 */
#define SCAN_THERMISTOR_TABLE(TBL,LEN) do{                                \
  uint8_t l = 0, r = LEN, m;                                              \
  for (;;) {                                                              \
    m = (l + r) >> 1;                                                     \
    if (!m) return celsius_t(pgm_read_word(&TBL[0].celsius));             \
    if (m == l || m == r) return celsius_t(pgm_read_word(&TBL[LEN-1].celsius)); \
    raw_adc_t v00 = pgm_read_word(&TBL[m-1].value),                       \
              v10 = pgm_read_word(&TBL[m-0].value);                       \
         if (raw < v00) r = m;                                            \
    else if (raw > v10) l = m;                                            \
    else {                                                                \
      const celsius_t v01 = celsius_t(pgm_read_word(&TBL[m-1].celsius)),  \
                      v11 = celsius_t(pgm_read_word(&TBL[m-0].celsius));  \
      return v01 + (raw - v00) * float(v11 - v01) / float(v10 - v00);     \
    }                                                                     \
  }                                                                       \
}while(0)

#if HAS_USER_THERMISTORS

  user_thermistor_t Temperature::user_thermistor[USER_THERMISTORS]; // Initialized by settings.load()

  void Temperature::reset_user_thermistors() {
    user_thermistor_t default_user_thermistor[USER_THERMISTORS] = {
      #if TEMP_SENSOR_IS_CUSTOM(0)
        { true, HOTEND0_SH_C_COEFF, 0, HOTEND0_PULLUP_RESISTOR_OHMS, HOTEND0_RESISTANCE_25C_OHMS, 0, 0, HOTEND0_BETA, 0 },
      #endif
      #if TEMP_SENSOR_IS_CUSTOM(1)
        { true, HOTEND1_SH_C_COEFF, 0, HOTEND1_PULLUP_RESISTOR_OHMS, HOTEND1_RESISTANCE_25C_OHMS, 0, 0, HOTEND1_BETA, 0 },
      #endif
      #if TEMP_SENSOR_IS_CUSTOM(2)
        { true, HOTEND2_SH_C_COEFF, 0, HOTEND2_PULLUP_RESISTOR_OHMS, HOTEND2_RESISTANCE_25C_OHMS, 0, 0, HOTEND2_BETA, 0 },
      #endif
      #if TEMP_SENSOR_IS_CUSTOM(3)
        { true, HOTEND3_SH_C_COEFF, 0, HOTEND3_PULLUP_RESISTOR_OHMS, HOTEND3_RESISTANCE_25C_OHMS, 0, 0, HOTEND3_BETA, 0 },
      #endif
      #if TEMP_SENSOR_IS_CUSTOM(4)
        { true, HOTEND4_SH_C_COEFF, 0, HOTEND4_PULLUP_RESISTOR_OHMS, HOTEND4_RESISTANCE_25C_OHMS, 0, 0, HOTEND4_BETA, 0 },
      #endif
      #if TEMP_SENSOR_IS_CUSTOM(5)
        { true, HOTEND5_SH_C_COEFF, 0, HOTEND5_PULLUP_RESISTOR_OHMS, HOTEND5_RESISTANCE_25C_OHMS, 0, 0, HOTEND5_BETA, 0 },
      #endif
      #if TEMP_SENSOR_IS_CUSTOM(6)
        { true, HOTEND6_SH_C_COEFF, 0, HOTEND6_PULLUP_RESISTOR_OHMS, HOTEND6_RESISTANCE_25C_OHMS, 0, 0, HOTEND6_BETA, 0 },
      #endif
      #if TEMP_SENSOR_IS_CUSTOM(7)
        { true, HOTEND7_SH_C_COEFF, 0, HOTEND7_PULLUP_RESISTOR_OHMS, HOTEND7_RESISTANCE_25C_OHMS, 0, 0, HOTEND7_BETA, 0 },
      #endif
    };
    COPY(user_thermistor, default_user_thermistor);
  }

  void Temperature::M305_report(const uint8_t t_index, const bool forReplay/*=true*/) {
    gcode.report_heading_etc(forReplay, F(STR_USER_THERMISTORS));
    SERIAL_ECHOPGM("  M305 P", AS_DIGIT(t_index));

    const user_thermistor_t &t = user_thermistor[t_index];

    SERIAL_ECHOPAIR_F(" R", t.series_res, 1);
    SERIAL_ECHOPAIR_F_P(SP_T_STR, t.res_25, 1);
    SERIAL_ECHOPAIR_F_P(SP_B_STR, t.beta, 1);
    SERIAL_ECHOPAIR_F_P(SP_C_STR, t.sh_c_coeff, 9);
    SERIAL_ECHOPGM(" ; ");
    SERIAL_ECHOF(
      TERN_(TEMP_SENSOR_IS_CUSTOM(0), t_index == CUSTOM_TI_0 ? F("TEMP SENSOR 0") :)
      TERN_(TEMP_SENSOR_IS_CUSTOM(1), t_index == CUSTOM_TI_1 ? F("TEMP SENSOR 1") :)
      TERN_(TEMP_SENSOR_IS_CUSTOM(2), t_index == CUSTOM_TI_2 ? F("TEMP SENSOR 2") :)
      TERN_(TEMP_SENSOR_IS_CUSTOM(3), t_index == CUSTOM_TI_3 ? F("TEMP SENSOR 3") :)
      TERN_(TEMP_SENSOR_IS_CUSTOM(4), t_index == CUSTOM_TI_4 ? F("TEMP SENSOR 4") :)
      TERN_(TEMP_SENSOR_IS_CUSTOM(5), t_index == CUSTOM_TI_5 ? F("TEMP SENSOR 5") :)
      TERN_(TEMP_SENSOR_IS_CUSTOM(6), t_index == CUSTOM_TI_6 ? F("TEMP SENSOR 6") :)
      TERN_(TEMP_SENSOR_IS_CUSTOM(7), t_index == CUSTOM_TI_7 ? F("TEMP SENSOR 7") :)
      nullptr
    );
    SERIAL_EOL();
  }

  celsius_float_t Temperature::user_thermistor_to_deg_c(const uint8_t t_index, const raw_adc_t raw) {

    if (!WITHIN(t_index, 0, COUNT(user_thermistor) - 1)) return 25;

    user_thermistor_t &t = user_thermistor[t_index];
    if (t.pre_calc) { // pre-calculate some variables
      t.pre_calc     = false;
      t.res_25_recip = 1.0f / t.res_25;
      t.res_25_log   = logf(t.res_25);
      t.beta_recip   = 1.0f / t.beta;
      t.sh_alpha     = RECIPROCAL(THERMISTOR_RESISTANCE_NOMINAL_C - (THERMISTOR_ABS_ZERO_C))
                        - (t.beta_recip * t.res_25_log) - (t.sh_c_coeff * cu(t.res_25_log));
    }

    // Maximum ADC value .. take into account the over sampling
    constexpr raw_adc_t adc_max = MAX_RAW_THERMISTOR_VALUE;
    const raw_adc_t adc_raw = constrain(raw, 1, adc_max - 1); // constrain to prevent divide-by-zero

    const float adc_inverse = (adc_max - adc_raw) - 0.5f,
                resistance = t.series_res * (adc_raw + 0.5f) / adc_inverse,
                log_resistance = logf(resistance);

    float value = t.sh_alpha;
    value += log_resistance * t.beta_recip;
    if (t.sh_c_coeff != 0)
      value += t.sh_c_coeff * cu(log_resistance);
    value = 1.0f / value;

    return value + THERMISTOR_ABS_ZERO_C;
  }
#endif

#if HAS_TEMP_SENSORS
  // Derived from RepRap FiveD extruder::getTemperature()
  // For hot end temperature measurement.
  celsius_float_t Temperature::analog_to_celsius_hotend(const raw_adc_t raw, const uint8_t e) {
    if (e >= NUM_TEMP_SENSORS) {
      SERIAL_ERROR_START();
      SERIAL_ECHO(e);
      // SERIAL_ECHOLNPGM(STR_INVALID_EXTRUDER_NUM); // TODO
      kill();
      return 0;
    }

    #if ANY_TEMP_SENSOR_IS_CUSTOM
      switch (e) {
        #define A_TO_C_CASE(N) \
          case N: \
            return TERN_IF(TEMP_SENSOR_IS_CUSTOM(N), \
              user_thermistor_to_deg_c(CUSTOM_TI_##N, raw), \
              TERN_IF(TEMP_SENSOR_IS_AD595(N), \
                TEMP_AD8495(raw##N), \
                TERN(TEMP_SENSOR_IS_AD8495(N), TEMP_AD8495(raw), 0 ) \
              ) \
            );
        A_TO_C_CASE(0)
        A_TO_C_CASE(1)
        A_TO_C_CASE(2)
        A_TO_C_CASE(3)
        A_TO_C_CASE(4)
        A_TO_C_CASE(5)
        A_TO_C_CASE(6)
        A_TO_C_CASE(7)
        default: break;
      }
      #undef A_TO_C_CASE;
    #endif

    #if HAS_HOTEND_THERMISTOR
      // Thermistor with conversion table?
      const temp_entry_t(*tt)[] = (temp_entry_t(*)[])(heater_ttbl_map[e]);
      SCAN_THERMISTOR_TABLE((*tt), heater_ttbllen_map[e]);
    #endif

    return 0;
  }
#endif // HAS_TEMP_SENSORS

/**
 * Convert the raw sensor readings into actual Celsius temperatures and
 * validate raw temperatures. Bad readings generate min/maxtemp errors.
 *
 * The raw values are generated entirely in interrupt context, and this
 * method is called from normal context once 'raw_temps_ready' has been
 * set by update_raw_temperatures().
 *
 * The watchdog is dependent on this method. If 'raw_temps_ready' stops
 * being set by the interrupt so that this method is not called for over
 * 4 seconds then something has gone afoul and the machine will be reset.
 */
void Temperature::updateTemperaturesFromRawValues() {

  hal.watchdog_refresh(); // Reset because raw_temps_ready was set by the interrupt

  #define A_TO_C(N) temp_hotend[N].celsius = analog_to_celsius_hotend(temp_hotend[N].getraw(), N);
  A_TO_C(0)
  A_TO_C(1)
  A_TO_C(2)
  A_TO_C(3)
  A_TO_C(4)
  A_TO_C(5)
  A_TO_C(6)
  A_TO_C(7)
  #undef A_TO_C

} // Temperature::updateTemperaturesFromRawValues

/**
 * Initialize the temperature manager
 *
 * The manager is implemented by periodic calls to task()
 *
 *  - Disable RUMBA JTAG to accommodate a thermocouple extension
 *  - Read-enable thermistors with a read-enable pin
 *  - Init the SPI interface for SPI thermocouples
 *  - Init ADC according to the HAL
 *  - Set thermistor pins to analog inputs according to the HAL
 *  - Start the Temperature ISR timer
 *  - Wait 250ms for temperatures to settle
 *  - Init temp_range[], used for catching min/maxtemp
 */
void Temperature::init() {

  #define ADC_ENABLE(N) if(HAS_ADC_TEST(N)) hal.adc_enable(TEMP_SENSOR_PIN(N));
  ADC_ENABLE(0)
  ADC_ENABLE(1)
  ADC_ENABLE(2)
  ADC_ENABLE(3)
  ADC_ENABLE(4)
  ADC_ENABLE(5)
  ADC_ENABLE(6)
  ADC_ENABLE(7)
  #undef ADC_ENABLE
  #undef _ADC_ENABLE

  HAL_timer_start(MF_TIMER_TEMP, TEMP_TIMER_FREQUENCY);
  ENABLE_TEMPERATURE_INTERRUPT();

  #if HAS_TEMP_SENSORS
    #define _TEMP_MIN_E(NR) do{ \
      const celsius_t tmin = TERN(TEMP_SENSOR_IS_CUSTOM(NR), 0, int16_t(pgm_read_word(&TEMPTABLE(NR) [TEMP_SENSOR_MINTEMP_IND(NR)].celsius))); \
      temp_range[NR].mintemp = tmin; \
      while (analog_to_celsius_hotend(temp_range[NR].raw_min, NR) < tmin) \
        temp_range[NR].raw_min += TEMPDIR(NR) * (OVERSAMPLENR); \
    }while(0)
    #define _TEMP_MAX_E(NR) do{ \
      const celsius_t tmax = TERN(TEMP_SENSOR_IS_CUSTOM(NR), 2000, int16_t(pgm_read_word(&TEMPTABLE(NR) [TEMP_SENSOR_MAXTEMP_IND(NR)].celsius)) - 1); \
      temp_range[NR].maxtemp = tmax; \
      while (analog_to_celsius_hotend(temp_range[NR].raw_max, NR) > tmax) \
        temp_range[NR].raw_max -= TEMPDIR(NR) * (OVERSAMPLENR); \
    }while(0)

  #endif // HAS_TEMP_SENSORS
}

/**
 * Update raw temperatures
 *
 * Called by ISR => readings_ready when new temperatures have been set by updateTemperaturesFromRawValues.
 * Applies all the accumulators to the current raw temperatures.
 */
void Temperature::update_raw_temperatures() {
  #define UPDATE_ADC(N) temp_hotend[N].update();
  #if HAS_ADC_TEST(0)
    UPDATE_ADC(0)
  #endif
  #if HAS_ADC_TEST(1)
    UPDATE_ADC(1)
  #endif
  #if HAS_ADC_TEST(2)
    UPDATE_ADC(2)
  #endif
  #if HAS_ADC_TEST(3)
    UPDATE_ADC(3)
  #endif
  #if HAS_ADC_TEST(4)
    UPDATE_ADC(4)
  #endif
  #if HAS_ADC_TEST(5)
    UPDATE_ADC(5)
  #endif
  #if HAS_ADC_TEST(6)
    UPDATE_ADC(6)
  #endif
  #if HAS_ADC_TEST(7)
    UPDATE_ADC(7)
  #endif
  #undef UPDATE_ADC
}

/**
 * Called by the Temperature ISR when all the ADCs have been processed.
 * Reset all the ADC accumulators for another round of updates.
 */
void Temperature::readings_ready() {

  // Update raw values only if they're not already set.
  if (!raw_temps_ready) {
    update_raw_temperatures();
    raw_temps_ready = true;
  }

  // Filament Sensor - can be read any time since IIR filtering is used
  TERN_(FILAMENT_WIDTH_SENSOR, filwidth.reading_ready());

  #define RESET_HOTEND(N) temp_hotend[N].reset();
  RESET_HOTEND(0);
  RESET_HOTEND(1);
  RESET_HOTEND(2);
  RESET_HOTEND(3);
  RESET_HOTEND(4);
  RESET_HOTEND(5);
  RESET_HOTEND(6);
  RESET_HOTEND(7);
  #undef RESET_HOTEND
}

/**
 * Timer 0 is shared with millies so don't change the prescaler.
 *
 * On AVR this ISR uses the compare method so it runs at the base
 * frequency (16 MHz / 64 / 256 = 976.5625 Hz), but at the TCNT0 set
 * in OCR0B above (128 or halfway between OVFs).
 *
 *  - Prepare or Measure one of the raw ADC sensor values
 *  - Check new temperature values for MIN/MAX errors (kill on error)
 *  - Step the babysteps value for each axis towards 0
 *  - For PINS_DEBUGGING, monitor and report endstop pins
 *  - Call planner.isr to count down its "ignore" time
 */
HAL_TEMP_TIMER_ISR() {
  HAL_timer_isr_prologue(MF_TIMER_TEMP);

  Temperature::isr();

  HAL_timer_isr_epilogue(MF_TIMER_TEMP);
}

/**
 * Handle various ~1kHz tasks associated with temperature
 *  - Start / Read one ADC sensor
 *  - Endstop polling
 */
void Temperature::isr() {

  static int8_t temp_count = -1;
  static ADCSensorState adc_sensor_state = StartupDelay;

  /**
   * One sensor is sampled on every other call of the ISR.
   * Each sensor is read 16 (OVERSAMPLENR) times, taking the average.
   *
   * On each Prepare pass, ADC is started for a sensor pin.
   * On the next pass, the ADC value is read and accumulated.
   *
   * This gives each ADC 0.9765ms to charge up.
   */
  #define ACCUMULATE_ADC(obj) do{ \
    if (!hal.adc_ready()) next_sensor_state = adc_sensor_state; \
    else obj.sample(hal.adc_value()); \
  }while(0)

  ADCSensorState next_sensor_state = adc_sensor_state < SensorsReady ? (ADCSensorState)(int(adc_sensor_state) + 1) : StartSampling;

  switch (adc_sensor_state) {

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

    case SensorsReady: {
      // All sensors have been read. Stay in this state for a few
      // ISRs to save on calls to temp update/checking code below.
      constexpr int8_t extra_loops = MIN_ADC_ISR_LOOPS - (int8_t)SensorsReady;
      static uint8_t delay_count = 0;
      if (extra_loops > 0) {
        if (delay_count == 0) delay_count = extra_loops;  // Init this delay
        if (--delay_count)                                // While delaying...
          next_sensor_state = SensorsReady;               // retain this state (else, next state will be 0)
        break;
      }
      else {
        adc_sensor_state = StartSampling;                 // Fall-through to start sampling
        next_sensor_state = (ADCSensorState)(int(StartSampling) + 1);
      }
    }

    #pragma GCC diagnostic pop

    case StartSampling:                                   // Start of sampling loops. Do updates/checks.
      if (++temp_count >= OVERSAMPLENR) {                 // 10 * 16 * 1/(16000000/64/256)  = 164ms.
        temp_count = 0;
        readings_ready();
      }
      break;

    #define ADC_CASE(N) \
      case PrepareTemp_##N: hal.adc_start(TEMP_SENSOR_PIN(N)); break; \
      case MeasureTemp_##N: ACCUMULATE_ADC(temp_hotend[N]); break; 
    #if HAS_TEMP_SENSOR(0) && HAS_ADC_TEST(0)
      ADC_CASE(0)
    #endif
    #if HAS_TEMP_SENSOR(1) && HAS_ADC_TEST(1)
      ADC_CASE(1)
    #endif
    #if HAS_TEMP_SENSOR(2) && HAS_ADC_TEST(2)
      ADC_CASE(2)
    #endif
    #if HAS_TEMP_SENSOR(3) && HAS_ADC_TEST(3)
      ADC_CASE(3)
    #endif
    #if HAS_TEMP_SENSOR(4) && HAS_ADC_TEST(4)
      ADC_CASE(4)
    #endif
    #if HAS_TEMP_SENSOR(5) && HAS_ADC_TEST(5)
      ADC_CASE(5)
    #endif
    #if HAS_TEMP_SENSOR(6) && HAS_ADC_TEST(6)
      ADC_CASE(6)
    #endif
    #if HAS_TEMP_SENSOR(7) && HAS_ADC_TEST(7)
      ADC_CASE(7)
    #endif
    #undef ADC_CASE
    
    // TODO: Move these to main loop() ?
    #if ENABLED(FILAMENT_WIDTH_SENSOR)
      case Prepare_FILWIDTH: hal.adc_start(FILWIDTH_PIN); break;
      case Measure_FILWIDTH:
        if (!hal.adc_ready()) next_sensor_state = adc_sensor_state; // Redo this state
        else filwidth.accumulate(hal.adc_value());
      break;
    #endif

    #if ENABLED(POWER_MONITOR_CURRENT)
      case Prepare_POWER_MONITOR_CURRENT:
        hal.adc_start(POWER_MONITOR_CURRENT_PIN);
        break;
      case Measure_POWER_MONITOR_CURRENT:
        if (!hal.adc_ready()) next_sensor_state = adc_sensor_state; // Redo this state
        else power_monitor.add_current_sample(hal.adc_value());
        break;
    #endif

    #if ENABLED(POWER_MONITOR_VOLTAGE)
      case Prepare_POWER_MONITOR_VOLTAGE:
        hal.adc_start(POWER_MONITOR_VOLTAGE_PIN);
        break;
      case Measure_POWER_MONITOR_VOLTAGE:
        if (!hal.adc_ready()) next_sensor_state = adc_sensor_state; // Redo this state
        else power_monitor.add_voltage_sample(hal.adc_value());
        break;
    #endif

    case StartupDelay: break;

  } // switch(adc_sensor_state)

  // Go to the next state
  adc_sensor_state = next_sensor_state;

  //
  // Additional ~1kHz Tasks
  //

  // TODO: Move this to main loop() ?
  // Poll endstops state, if required
  digitalInputs.poll();
}

#if HAS_TEMP_SENSORS
  /**
   * Print a single heater state in the form:
   *   Sensor:   " T0:nnn.nn"
   *   With ADC: " T0:nnn.nn (nnn.nn)"
   */
  static void print_heater_state(const heater_id_t e, const_celsius_float_t c
    OPTARG(SHOW_TEMP_ADC_VALUES, const float r)
  ) {
    char k;
    switch (e) {
      default:
        #if HAS_TEMP_SENSORS
          k = 'T'; break;
        #endif
    }
    SERIAL_CHAR(' ', k);
    #if HAS_MULTI_TEMP_SENSORS
      if (e >= 0) SERIAL_CHAR('0' + e);
    #endif
    #ifdef SERIAL_FLOAT_PRECISION
      #define SFP _MIN(SERIAL_FLOAT_PRECISION, 2)
    #else
      #define SFP 2
    #endif
    SERIAL_CHAR(':');
    SERIAL_PRINT(c, SFP);
    #if ENABLED(SHOW_TEMP_ADC_VALUES)
      SERIAL_ECHOPGM(" (", k == 'T' ? r : r * RECIPROCAL(OVERSAMPLENR));
      SERIAL_CHAR(')');
    #endif
    delay(2);
  }

  void Temperature::print_heater_states(const int8_t target_extruder
    OPTARG(HAS_TEMP_REDUNDANT, const bool include_r/*=false*/)
  ) {
    #if HAS_TEMP_SENSORS
      print_heater_state(H_NONE, degHotend(target_extruder) OPTARG(SHOW_TEMP_ADC_VALUES, rawHotendTemp(target_extruder)));
    #endif
    #if HAS_MULTI_TEMP_SENSORS
      TEMP_SENSOR_LOOP() print_heater_state((heater_id_t)e, degHotend(e) OPTARG(SHOW_TEMP_ADC_VALUES, rawHotendTemp(e)));
    #endif
  }

  #if ENABLED(AUTO_REPORT_SENSORS)
    AutoReporter<Temperature::AutoReportTemp> Temperature::auto_reporter;
    void Temperature::AutoReportTemp::report() {
      // TODO: Loop over NUM_TEMP_SENSORS
      print_heater_states(0);
      SERIAL_EOL();
    }
  #endif

#endif // HAS_TEMP_SENSORS
