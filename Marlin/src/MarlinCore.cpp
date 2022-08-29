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
 * About Marlin
 *
 * This firmware is a mashup between Sprinter and grbl.
 *  - https://github.com/kliment/Sprinter
 *  - https://github.com/grbl/grbl
 */

#include "MarlinCore.h"

#include "HAL/shared/Delay.h"
#include "HAL/shared/esp_wifi.h"
#include "HAL/shared/cpu_exception/exception_hook.h"

#ifdef ARDUINO
  #include <pins_arduino.h>
#endif
#include <math.h>

#include "module/digital_inputs.h"
#include "module/settings.h"
#include "module/temperature.h"

#include "gcode/gcode.h"
#include "gcode/parser.h"
#include "gcode/queue.h"

#if HAS_ETHERNET
  #include "feature/ethernet.h"
#endif

#if ENABLED(IIC_BL24CXX_EEPROM)
  #include "libs/BL24CXX.h"
#endif

#if ENABLED(HOST_ACTION_COMMANDS)
  #include "feature/host_actions.h"
#endif

#if HAS_COLOR_LEDS
  #include "feature/leds/leds.h"
#endif

#if ENABLED(I2C_POSITION_ENCODERS)
  #include "feature/encoder_i2c.h"
#endif

#if HAS_FILAMENT_SENSOR
  #include "feature/runout.h"
#endif

#if ENABLED(TEMP_STAT_LEDS)
  #include "feature/leds/tempstat.h"
#endif

#if ENABLED(CASE_LIGHT_ENABLE)
  #include "feature/caselight.h"
#endif

PGMSTR(M112_KILL_STR, "M112 Shutdown");

MarlinState marlin_state = MF_INITIALIZING;

/**
 * ***************************************************************************
 * ******************************** FUNCTIONS ********************************
 * ***************************************************************************
 */

/**
 * Sensitive pin test for M42, M226
 */

#include "pins/sensitive_pins.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

#ifndef RUNTIME_ONLY_ANALOG_TO_DIGITAL
  template <pin_t ...D>
  constexpr pin_t OnlyPins<_SP_END, D...>::table[sizeof...(D)];
#endif

bool pin_is_protected(const pin_t pin) {
  #ifdef RUNTIME_ONLY_ANALOG_TO_DIGITAL
    static const pin_t sensitive_pins[] PROGMEM = { SENSITIVE_PINS };
    const size_t pincount = COUNT(sensitive_pins);
  #else
    static constexpr size_t pincount = OnlyPins<SENSITIVE_PINS>::size;
    static const pin_t (&sensitive_pins)[pincount] PROGMEM = OnlyPins<SENSITIVE_PINS>::table;
  #endif
  LOOP_L_N(i, pincount) {
    const pin_t * const pptr = &sensitive_pins[i];
    if (pin == (sizeof(pin_t) == 2 ? (pin_t)pgm_read_word(pptr) : (pin_t)pgm_read_byte(pptr))) return true;
  }
  return false;
}

#pragma GCC diagnostic pop

/**
 * Standard idle routine keeps the machine alive:
 *  - Core Marlin activities
 *  - Manage heaters (and Watchdog)
 *
 *  Only after setup() is complete:
 *  - Handle filament runout sensors
 *  - Run HAL idle tasks
 *  - Announce Host Keepalive state (if any)
 *  - Run i2c Position Encoders
 *  - Auto-report
 */
void idle() {

  #if ENABLED(MARLIN_DEV_MODE)
    static uint16_t idle_depth = 0;
    if (++idle_depth > 5) SERIAL_ECHOLNPGM("idle() call depth: ", idle_depth);
  #endif

  // Keep the command buffer full
  queue.get_available_commands();

  // Manage Temperature Sensors (and Watchdog)
  tempSensors.task();

  // Return if setup() isn't completed
  if (marlin_state != MF_INITIALIZING) {
  
    // Manage Digital Inputs
    digitalInputs.event_handler();
    
    // Handle Power Monitoring
    TERN_(HAS_POWER_MONITOR,     power_monitor.capture_values());

    // Handle Status LEDs
    TERN_(TEMP_STAT_LEDS, handle_status_leds());

    // Handle filament sensors
    TERN_(HAS_FILAMENT_SENSOR, runout.run());
    TERN_(FILAMENT_WIDTH_SENSOR, filwidth.update_measured_mm());

    // Run HAL idle tasks
    hal.idletask();

    // Check network connection
    TERN_(HAS_ETHERNET, ethernet.check());

    // Announce Host Keepalive state (if any)
    TERN_(HOST_KEEPALIVE_FEATURE, gcode.host_keepalive());

    // Run i2c Position Encoders
    #if ENABLED(I2C_POSITION_ENCODERS)
    {
      static millis_t i2cpem_next_update_ms;
      if (planner.has_blocks_queued()) {
        const millis_t ms = millis();
        if (ELAPSED(ms, i2cpem_next_update_ms)) {
          I2CPEM.update();
          i2cpem_next_update_ms = ms + I2CPE_MIN_UPD_TIME_MS;
        }
      }
    }
    #endif

    // Auto-report
    #if EITHER(AUTO_REPORT_SENSORS, BUFFER_MONITORING)
      if (!gcode.autoreport_paused) {
        TERN_(AUTO_REPORT_SENSORS, tempSensors.auto_reporter.tick());
        TERN_(BUFFER_MONITORING, queue.auto_report_buffer_statistics());
      }
    #endif

  }
  TERN_(MARLIN_DEV_MODE, idle_depth--);
  return;
}

/**
 * Kill all activity and lock the machine.
 * After this the machine will need to be reset.
 */
void kill(FSTR_P const message/*=nullptr*/) {

  // Echo the message to serial for extra context
  if (message) { SERIAL_ECHO_START(); SERIAL_ECHOLNF(message); }

  // "Error:Printer halted. kill() called!"
  SERIAL_ERROR_MSG(STR_ERR_KILLED);

  #ifdef ACTION_ON_KILL
    hostui.kill();
  #endif

  minkill();
}

void minkill() {

  // Wait a short time (allows messages to get out before shutting down.
  for (int i = 1000; i--;) DELAY_US(600);

  cli(); // Stop interrupts

  // Wait to ensure all interrupts stopped
  for (int i = 1000; i--;) DELAY_US(250);

  for (;;) hal.watchdog_refresh();  // Wait for RESET button or power-cycle

}

/**
 * Marlin Firmware entry-point. Abandon Hope All Ye Who Enter Here.
 * Setup before the program loop:
 *
 *  - Call any special pre-init set for the board
 *  - Init the serial ports (so setup can be debugged)
 *  - Install Marlin custom Exception Handlers, if set.
 *  - Init Marlin's HAL interfaces (for SPI, i2c, etc.)
 *  - Init some optional hardware and features:
 *    • Filament Runout Sensor
 *    • Run hal.init_board() for additional pins setup
 *    • ESP WiFi
 *  - Get the Reset Reason and report it
 *  - Print startup messages and diagnostics
 *  - Calibrate the HAL DELAY for precise timing
 *  - Init more optional hardware:
 *    • Color LED illumination
 *    • Neopixel illumination
 *  - Load settings from EEPROM (or use defaults)
 *  - Init the Ethernet Port
 *  - Initialize more managers and peripherals:
 *    • Temperatures
 *    • Endstops and Endstop Interrupts
 *    • Red/Blue Status LEDs
 *    • Case Light
 *    • I2C Position Encoders
 *    • Custom I2C Bus handlers
 *    • Watchdog Timer - Also Kind of Important!
 *  - Tell host to close Host Prompts
 *  - Set Marlin to RUNNING State
 */
void setup() {
  #ifdef FASTIO_INIT
    FASTIO_INIT();
  #endif

  #ifdef BOARD_PREINIT
    BOARD_PREINIT(); // Low-level init (before serial init)
  #endif

  // Check startup - does nothing if bootloader sets MCUSR to 0
  const byte mcu = hal.get_reset_source();
  hal.clear_reset_source();

  #if ENABLED(MARLIN_DEV_MODE)
    auto log_current_ms = [&](PGM_P const msg) {
      SERIAL_ECHO_START();
      SERIAL_CHAR('['); SERIAL_ECHO(millis()); SERIAL_ECHOPGM("] ");
      SERIAL_ECHOLNPGM_P(msg);
    };
    #define SETUP_LOG(M) log_current_ms(PSTR(M))
  #else
    #define SETUP_LOG(...) NOOP
  #endif
  #define SETUP_RUN(C) do{ SETUP_LOG(STRINGIFY(C)); C; }while(0)

  MYSERIAL1.begin(BAUDRATE);
  millis_t serial_connect_timeout = millis() + 1000UL;
  while (!MYSERIAL1.connected() && PENDING(millis(), serial_connect_timeout)) { /*nada*/ }

  SERIAL_ECHOLNPGM("start");

  TERN_(DYNAMIC_VECTORTABLE, hook_cpu_exceptions()); // If supported, install Marlin exception handlers at runtime

  SETUP_RUN(hal.init());

  #if HAS_FILAMENT_SENSOR
    SETUP_RUN(runout.setup());
  #endif

  SETUP_RUN(hal.init_board());

  SETUP_RUN(esp_wifi_init());

  // Report Reset Reason
  if (mcu & RST_POWER_ON)  SERIAL_ECHOLNPGM(STR_POWERUP);
  if (mcu & RST_EXTERNAL)  SERIAL_ECHOLNPGM(STR_EXTERNAL_RESET);
  if (mcu & RST_BROWN_OUT) SERIAL_ECHOLNPGM(STR_BROWNOUT_RESET);
  if (mcu & RST_WATCHDOG)  SERIAL_ECHOLNPGM(STR_WATCHDOG_RESET);
  if (mcu & RST_SOFTWARE)  SERIAL_ECHOLNPGM(STR_SOFTWARE_RESET);

  // Identify myself as Marlin x.x.x
  SERIAL_ECHOLNPGM("Marlin " SHORT_BUILD_VERSION);
  #if defined(STRING_DISTRIBUTION_DATE) && defined(STRING_CONFIG_H_AUTHOR)
    SERIAL_ECHO_MSG(
      " Last Updated: " STRING_DISTRIBUTION_DATE
      " | Author: " STRING_CONFIG_H_AUTHOR
    );
  #endif
  SERIAL_ECHO_MSG(" Compiled: " __DATE__);
  SERIAL_ECHO_MSG(STR_FREE_MEMORY, hal.freeMemory());

  // Some HAL need precise delay adjustment
  calibrate_delay_loop();
  

  // Set up LEDs early
  #if HAS_COLOR_LEDS
    SETUP_RUN(leds.setup());
  #endif

  #if ENABLED(NEOPIXEL2_SEPARATE)
    SETUP_RUN(leds2.setup());
  #endif

  SETUP_RUN(settings.first_load());   // Load data from EEPROM if available (or use defaults)
                                      // This also updates variables in the planner, elsewhere

  #if HAS_ETHERNET
    SETUP_RUN(ethernet.init());
  #endif

  // Init HAL ADC
  hal.adc_init();

  SETUP_RUN(tempSensors.init());   // Initialize temperature sensors loop
  
  #if ENABLED(FILAMENT_WIDTH_SENSOR)
    SETUP_RUN(hal.adc_enable(FILWIDTH_PIN));
  #endif

  #if ENABLED(POWER_MONITOR_CURRENT)
    SETUP_RUN(hal.adc_enable(POWER_MONITOR_CURRENT_PIN));
  #endif
  #if ENABLED(POWER_MONITOR_VOLTAGE)
    SETUP_RUN(hal.adc_enable(POWER_MONITOR_VOLTAGE_PIN));
  #endif

  SETUP_RUN(digitalInputs.init()); // Initialize digital inputes and pullups/pulldowns

  #if PIN_EXISTS(STAT_LED_RED)
    OUT_WRITE(STAT_LED_RED_PIN, LOW); // OFF
  #endif
  #if PIN_EXISTS(STAT_LED_BLUE)
    OUT_WRITE(STAT_LED_BLUE_PIN, LOW); // OFF
  #endif

  #if ENABLED(CASE_LIGHT_ENABLE)
    SETUP_RUN(caselight.init());
  #endif

  #if ENABLED(I2C_POSITION_ENCODERS)
    SETUP_RUN(I2CPEM.init());
  #endif

  #if ENABLED(USE_WATCHDOG)
    SETUP_RUN(hal.watchdog_init());   // Reinit watchdog after hal.get_reset_source call
  #endif

  #if ENABLED(IIC_BL24CXX_EEPROM)
    BL24CXX::init();
    const uint8_t err = BL24CXX::check();
    SERIAL_ECHO_TERNARY(err, "BL24CXX Check ", "failed", "succeeded", "!\n");
  #endif

  marlin_state = MF_RUNNING;

  SETUP_LOG("setup() completed.");

  TERN_(MARLIN_TEST_BUILD, runStartupTests());
}

/**
 * The main Marlin program loop
 *
 *  - Call idle() to handle all tasks between G-code commands
 *      Note that no G-codes from the queue can be executed during idle()
 *      but many G-codes can be called directly anytime like macros.
 *  - Check whether SD card auto-start is needed now.
 *  - Check whether SD print finishing is needed now.
 *  - Run one G-code command from the immediate or main command queue
 *    and open up one space. Commands in the main queue may come from sd
 *    card, host, or by direct injection. The queue will continue to fill
 *    as long as idle() is being called.
 */
void loop() {
  do {
    idle();

    queue.advance();

    TERN_(MARLIN_TEST_BUILD, runPeriodicTests());

  } while (ENABLED(__AVR__)); // Loop forever on slower (AVR) boards
}
