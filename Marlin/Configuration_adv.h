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
 * Configuration_adv.h
 *
 * Advanced settings.
 * Only change these if you know exactly what you're doing.
 * Some of these settings can damage your printer if improperly set!
 *
 * Basic settings can be found in Configuration.h
 */
#define CONFIGURATION_ADV_H_VERSION 02010101

// @section develop

/**
 * Configuration Export
 *
 * Export the configuration as part of the build. (See signature.py)
 * Output files are saved with the build (e.g., .pio/build/mega2560).
 *
 * See `build_all_examples --ini` as an example of config.ini archiving.
 *
 *  1 = marlin_config.json - Dictionary containing the configuration.
 *      This file is also generated for CONFIGURATION_EMBEDDING.
 *  2 = config.ini - File format for PlatformIO preprocessing.
 *  3 = schema.json - The entire configuration schema. (13 = pattern groups)
 *  4 = schema.yml - The entire configuration schema.
 */
//#define CONFIG_EXPORT 2 // :[1:'JSON', 2:'config.ini', 3:'schema.json', 4:'schema.yml']

//===========================================================================
//============================ Temperature Sensors ==========================
//===========================================================================
// @section temperature

/**
 * Thermocouple sensors are quite sensitive to noise.  Any noise induced in
 * the sensor wires, such as by stepper motor wires run in parallel to them,
 * may result in the thermocouple sensor reporting spurious errors.  This
 * value is the number of errors which can occur in a row before the error
 * is reported.  This allows us to ignore intermittent error conditions while
 * still detecting an actual failure, which should result in a continuous
 * stream of errors from the sensor.
 *
 * Set this value to 0 to fail on the first error to occur.
 */
#define THERMOCOUPLE_MAX_ERRORS 15

//
// Custom Thermistor 1000 parameters
//
#if TEMP_SENSOR_0 == 1000
  #define HOTEND0_PULLUP_RESISTOR_OHMS    4700 // Pullup resistor
  #define HOTEND0_RESISTANCE_25C_OHMS   100000 // Resistance at 25C
  #define HOTEND0_BETA                    3950 // Beta value
  #define HOTEND0_SH_C_COEFF                 0 // Steinhart-Hart C coefficient
#endif

#if TEMP_SENSOR_1 == 1000
  #define HOTEND1_PULLUP_RESISTOR_OHMS    4700 // Pullup resistor
  #define HOTEND1_RESISTANCE_25C_OHMS   100000 // Resistance at 25C
  #define HOTEND1_BETA                    3950 // Beta value
  #define HOTEND1_SH_C_COEFF                 0 // Steinhart-Hart C coefficient
#endif

#if TEMP_SENSOR_2 == 1000
  #define HOTEND2_PULLUP_RESISTOR_OHMS    4700 // Pullup resistor
  #define HOTEND2_RESISTANCE_25C_OHMS   100000 // Resistance at 25C
  #define HOTEND2_BETA                    3950 // Beta value
  #define HOTEND2_SH_C_COEFF                 0 // Steinhart-Hart C coefficient
#endif

#if TEMP_SENSOR_3 == 1000
  #define HOTEND3_PULLUP_RESISTOR_OHMS    4700 // Pullup resistor
  #define HOTEND3_RESISTANCE_25C_OHMS   100000 // Resistance at 25C
  #define HOTEND3_BETA                    3950 // Beta value
  #define HOTEND3_SH_C_COEFF                 0 // Steinhart-Hart C coefficient
#endif

#if TEMP_SENSOR_4 == 1000
  #define HOTEND4_PULLUP_RESISTOR_OHMS    4700 // Pullup resistor
  #define HOTEND4_RESISTANCE_25C_OHMS   100000 // Resistance at 25C
  #define HOTEND4_BETA                    3950 // Beta value
  #define HOTEND4_SH_C_COEFF                 0 // Steinhart-Hart C coefficient
#endif

#if TEMP_SENSOR_5 == 1000
  #define HOTEND5_PULLUP_RESISTOR_OHMS    4700 // Pullup resistor
  #define HOTEND5_RESISTANCE_25C_OHMS   100000 // Resistance at 25C
  #define HOTEND5_BETA                    3950 // Beta value
  #define HOTEND5_SH_C_COEFF                 0 // Steinhart-Hart C coefficient
#endif

#if TEMP_SENSOR_6 == 1000
  #define HOTEND6_PULLUP_RESISTOR_OHMS    4700 // Pullup resistor
  #define HOTEND6_RESISTANCE_25C_OHMS   100000 // Resistance at 25C
  #define HOTEND6_BETA                    3950 // Beta value
  #define HOTEND6_SH_C_COEFF                 0 // Steinhart-Hart C coefficient
#endif

#if TEMP_SENSOR_7 == 1000
  #define HOTEND7_PULLUP_RESISTOR_OHMS    4700 // Pullup resistor
  #define HOTEND7_RESISTANCE_25C_OHMS   100000 // Resistance at 25C
  #define HOTEND7_BETA                    3950 // Beta value
  #define HOTEND7_SH_C_COEFF                 0 // Steinhart-Hart C coefficient
#endif

/**
 * Thermocouple Options — for MAX6675 (-2), MAX31855 (-3), and MAX31865 (-5).
 */
//#define TEMP_SENSOR_FORCE_HW_SPI                // Ignore SCK/MOSI/MISO pins; use CS and the default SPI bus.
//#define MAX31865_SENSOR_WIRES_0 2               // (2-4) Number of wires for the probe connected to a MAX31865 board.
//#define MAX31865_SENSOR_WIRES_1 2

//#define MAX31865_50HZ_FILTER                    // Use a 50Hz filter instead of the default 60Hz.
//#define MAX31865_USE_READ_ERROR_DETECTION       // Treat value spikes (20°C delta in under 1s) as read errors.

//#define MAX31865_USE_AUTO_MODE                  // Read faster and more often than 1-shot; bias voltage always on; slight effect on RTD temperature.
//#define MAX31865_MIN_SAMPLING_TIME_MSEC     100 // (ms) 1-shot: minimum read interval. Reduces bias voltage effects by leaving sensor unpowered for longer intervals.
//#define MAX31865_IGNORE_INITIAL_FAULTY_READS 10 // Ignore some read faults (keeping the temperature reading) to work around a possible issue (#23439).

//#define MAX31865_WIRE_OHMS_0              0.95f // For 2-wire, set the wire resistances for more accurate readings.
//#define MAX31865_WIRE_OHMS_1              0.0f

// Show Temperature ADC value
// Enable for M105 to include ADC values read from temperature sensors.
//#define SHOW_TEMP_ADC_VALUES

// Calibration for AD595 / AD8495 sensor to adjust temperature measurements.
// The final temperature is calculated as (measuredTemp * GAIN) + OFFSET.
#define TEMP_SENSOR_AD595_OFFSET  0.0
#define TEMP_SENSOR_AD595_GAIN    1.0
#define TEMP_SENSOR_AD8495_OFFSET 0.0
#define TEMP_SENSOR_AD8495_GAIN   1.0

//===========================================================================
//================================== Extras =================================
//===========================================================================

// @section case lighting

/**
 * M355 Case Light on-off / brightness
 */
//#define CASE_LIGHT_ENABLE
#if ENABLED(CASE_LIGHT_ENABLE)
  //#define CASE_LIGHT_PIN 4                  // Override the default pin if needed
  #define INVERT_CASE_LIGHT false             // Set true if Case Light is ON when pin is LOW
  #define CASE_LIGHT_DEFAULT_ON true          // Set default power-up state on
  #define CASE_LIGHT_DEFAULT_BRIGHTNESS 105   // Set default power-up brightness (0-255, requires PWM pin)
  //#define CASE_LIGHT_NO_BRIGHTNESS          // Disable brightness control. Enable for non-PWM lighting.
  //#define CASE_LIGHT_MAX_PWM 128            // Limit PWM duty cycle (0-255)
  //#define CASE_LIGHT_MENU                   // Add Case Light options to the LCD menu
  #if ENABLED(NEOPIXEL_LED)
    //#define CASE_LIGHT_USE_NEOPIXEL         // Use NeoPixel LED as case light
  #endif
  #if EITHER(RGB_LED, RGBW_LED)
    //#define CASE_LIGHT_USE_RGB_LED          // Use RGB / RGBW LED as case light
  #endif
  #if EITHER(CASE_LIGHT_USE_NEOPIXEL, CASE_LIGHT_USE_RGB_LED)
    #define CASE_LIGHT_DEFAULT_COLOR { 255, 255, 255, 255 } // { Red, Green, Blue, White }
  #endif
#endif

// @section power

/**
 * Power Monitor
 * Monitor voltage (V) and/or current (A), and -when possible- power (W)
 *
 * Read and configure with M430
 *
 * The current sensor feeds DC voltage (relative to the measured current) to an analog pin
 * The voltage sensor feeds DC voltage (relative to the measured voltage) to an analog pin
 */
//#define POWER_MONITOR_CURRENT   // Monitor the system current
//#define POWER_MONITOR_VOLTAGE   // Monitor the system voltage

#if ENABLED(POWER_MONITOR_CURRENT)
  #define POWER_MONITOR_VOLTS_PER_AMP    0.05000  // Input voltage to the MCU analog pin per amp  - DO NOT apply more than ADC_VREF!
  #define POWER_MONITOR_CURRENT_OFFSET   0        // Offset (in amps) applied to the calculated current
  #define POWER_MONITOR_FIXED_VOLTAGE   13.6      // Voltage for a current sensor with no voltage sensor (for power display)
#endif

#if ENABLED(POWER_MONITOR_VOLTAGE)
  #define POWER_MONITOR_VOLTS_PER_VOLT  0.077933  // Input voltage to the MCU analog pin per volt - DO NOT apply more than ADC_VREF!
  #define POWER_MONITOR_VOLTAGE_OFFSET  0         // Offset (in volts) applied to the calculated voltage
#endif

// @section i2cbus

//
// I2C Master ID for LPC176x LCD and Digital Current control
// Does not apply to other peripherals based on the Wire library.
//
//#define I2C_MASTER_ID  1  // Set a value from 0 to 2

/**
 * TWI/I2C BUS
 *
 * This feature is an EXPERIMENTAL feature so it shall not be used on production
 * machines. Enabling this will allow you to send and receive I2C data from slave
 * devices on the bus.
 *
 * ; Example #1
 * ; This macro send the string "Marlin" to the slave device with address 0x63 (99)
 * ; It uses multiple M260 commands with one B<base 10> arg
 * M260 A99  ; Target slave address
 * M260 B77  ; M
 * M260 B97  ; a
 * M260 B114 ; r
 * M260 B108 ; l
 * M260 B105 ; i
 * M260 B110 ; n
 * M260 S1   ; Send the current buffer
 *
 * ; Example #2
 * ; Request 6 bytes from slave device with address 0x63 (99)
 * M261 A99 B5
 *
 * ; Example #3
 * ; Example serial output of a M261 request
 * echo:i2c-reply: from:99 bytes:5 data:hello
 */

//#define EXPERIMENTAL_I2CBUS
#if ENABLED(EXPERIMENTAL_I2CBUS)
  #define I2C_SLAVE_ADDRESS  0  // Set a value from 8 to 127 to act as a slave
#endif

// @section safety

/**
 * The watchdog hardware timer will do a reset and disable all outputs
 * if the firmware gets too overloaded to read the temperature sensors.
 *
 * If you find that watchdog reboot causes your AVR board to hang forever,
 * enable WATCHDOG_RESET_MANUAL to use a custom timer instead of WDTO.
 * NOTE: This method is less reliable as it can only catch hangups while
 * interrupts are enabled.
 */
#define USE_WATCHDOG
#if ENABLED(USE_WATCHDOG)
  //#define WATCHDOG_RESET_MANUAL
#endif

//===========================================================================
//================================= Buffers =================================
//===========================================================================

// @section serial

// The ASCII buffer for serial input
#define MAX_CMD_SIZE 96
#define BUFSIZE 4

// Transmission to Host Buffer Size
// To save 386 bytes of flash (and TX_BUFFER_SIZE+3 bytes of RAM) set to 0.
// To buffer a simple "ok" you need 4 bytes.
// For ADVANCED_OK (M105) you need 32 bytes.
// For debug-echo: 128 bytes for the optimal speed.
// Other output doesn't need to be that speedy.
// :[0, 2, 4, 8, 16, 32, 64, 128, 256]
#define TX_BUFFER_SIZE 0

// Host Receive Buffer Size
// Without XON/XOFF flow control (see SERIAL_XON_XOFF below) 32 bytes should be enough.
// To use flow control, set this buffer size to at least 1024 bytes.
// :[0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048]
//#define RX_BUFFER_SIZE 1024

#if RX_BUFFER_SIZE >= 1024
  // Enable to have the controller send XON/XOFF control characters to
  // the host to signal the RX buffer is becoming full.
  //#define SERIAL_XON_XOFF
#endif

// Monitor RX buffer usage
// Dump an error to the serial port if the serial receive buffer overflows.
// If you see these errors, increase the RX_BUFFER_SIZE value.
// Not supported on all platforms.
//#define RX_BUFFER_MONITOR

/**
 * Emergency Command Parser
 *
 * Add a low-level parser to intercept certain commands as they
 * enter the serial receive buffer, so they cannot be blocked.
 * Currently handles M112
 * NOTE: Not yet implemented for all platforms.
 */
#define EMERGENCY_PARSER

/**
 * Realtime Reporting (requires EMERGENCY_PARSER)
 *
 * - Report position and state of the machine (like Grbl).
 * - Auto-report position during long moves.
 * - Useful for CNC/LASER.
 *
 * Adds support for commands:
 *  S000 : Report State and Position while moving.
 *  P000 : Instant Pause / Hold while moving.
 *  R000 : Resume from Pause / Hold.
 *
 * - During Hold all Emergency Parser commands are available, as usual.
 * - Enable NANODLP_Z_SYNC and NANODLP_ALL_AXIS for move command end-state reports.
 */
//#define REALTIME_REPORTING_COMMANDS
#if ENABLED(REALTIME_REPORTING_COMMANDS)
  //#define FULL_REPORT_TO_HOST_FEATURE   // Auto-report the machine status like Grbl CNC
#endif

// Bad Serial-connections can miss a received command by sending an 'ok'
// Therefore some clients abort after 30 seconds in a timeout.
// Some other clients start sending commands while receiving a 'wait'.
// This "wait" is only sent when the buffer is empty. 1 second is a good value here.
//#define NO_TIMEOUTS 1000 // Milliseconds

// Some clients will have this feature soon. This could make the NO_TIMEOUTS unnecessary.
//#define ADVANCED_OK

// Printrun may have trouble receiving long strings all at once.
// This option inserts short delays between lines of serial output.
#define SERIAL_OVERRUN_PROTECTION

// For serial echo, the number of digits after the decimal point
//#define SERIAL_FLOAT_PRECISION 4

/**
 * Set the number of proportional font spaces required to fill up a typical character space.
 * This can help to better align the output of commands like `G29 O` Mesh Output.
 *
 * For clients that use a fixed-width font (like OctoPrint), leave this set to 1.0.
 * Otherwise, adjust according to your client and font.
 */
#define PROPORTIONAL_FONT_RATIO 1.0

// @section reporting

/**
 * Auto-report sensor values with M155 S<seconds>
 */
#define AUTO_REPORT_SENSORS

/**
 * Include capabilities in M115 output
 */
#define EXTENDED_CAPABILITIES_REPORT

// @section gcode

/**
 * Spend 28 bytes of SRAM to optimize the G-code parser
 */
#define FASTER_GCODE_PARSER

#if ENABLED(FASTER_GCODE_PARSER)
  //#define GCODE_QUOTED_STRINGS  // Support for quoted string parameters
#endif

/**
 * CNC G-code options
 * Support CNC-style G-code dialects used by laser cutters, drawing machine cams, etc.
 * Note that G0 feedrates should be used with care for 3D printing (if used at all).
 * High feedrates may cause ringing and harm print quality.
 */
#define PAREN_COMMENTS      // Support for parentheses-delimited comments

// @section host

/**
 * Host Action Commands
 *
 * Define host streamer action commands in compliance with the standard.
 *
 * See https://reprap.org/wiki/G-code#Action_commands
 * Common commands ........ poweroff, pause, paused, resume, resumed, cancel
 *
 * Some features add reason codes to extend these commands.
 */
#define HOST_ACTION_COMMANDS

// @section extras

/**
 * I2C position encoders.
 * Developed by Chris Barr at Aus3D.
 *
 * Wiki: https://wiki.aus3d.com.au/Magnetic_Encoder
 * Github: https://github.com/Aus3D/MagneticEncoder
 *
 * Supplier: https://aus3d.com.au/magnetic-encoder-module
 * Alternative Supplier: https://reliabuild3d.com/
 *
 * Reliabuild encoders have been modified to improve reliability.
 * @section i2c encoders
 */

//#define I2C_POSITION_ENCODERS
#if ENABLED(I2C_POSITION_ENCODERS)

  #define I2CPE_ENCODER_CNT         1                       // The number of encoders installed; max of 5
                                                            // encoders supported currently.

  #define I2CPE_ENC_1_ADDR          I2CPE_PRESET_ADDR_X     // I2C address of the encoder. 30-200.
  #define I2CPE_ENC_1_AXIS          X_AXIS                  // Axis the encoder module is installed on.  <X|Y|Z|E>_AXIS.
  #define I2CPE_ENC_1_TYPE          I2CPE_ENC_TYPE_LINEAR   // Type of encoder:  I2CPE_ENC_TYPE_LINEAR -or-
                                                            // I2CPE_ENC_TYPE_ROTARY.
  #define I2CPE_ENC_1_TICKS_UNIT    2048                    // 1024 for magnetic strips with 2mm poles; 2048 for
                                                            // 1mm poles. For linear encoders this is ticks / mm,
                                                            // for rotary encoders this is ticks / revolution.
  //#define I2CPE_ENC_1_TICKS_REV     (16 * 200)            // Only needed for rotary encoders; number of stepper
                                                            // steps per full revolution (motor steps/rev * microstepping)
  //#define I2CPE_ENC_1_INVERT                              // Invert the direction of axis travel.
  #define I2CPE_ENC_1_EC_METHOD     I2CPE_ECM_MICROSTEP     // Type of error error correction.
  #define I2CPE_ENC_1_EC_THRESH     0.10                    // Threshold size for error (in mm) above which the
                                                            // printer will attempt to correct the error; errors
                                                            // smaller than this are ignored to minimize effects of
                                                            // measurement noise / latency (filter).

  #define I2CPE_ENC_2_ADDR          I2CPE_PRESET_ADDR_Y     // Same as above, but for encoder 2.
  #define I2CPE_ENC_2_AXIS          Y_AXIS
  #define I2CPE_ENC_2_TYPE          I2CPE_ENC_TYPE_LINEAR
  #define I2CPE_ENC_2_TICKS_UNIT    2048
  //#define I2CPE_ENC_2_TICKS_REV   (16 * 200)
  //#define I2CPE_ENC_2_INVERT
  #define I2CPE_ENC_2_EC_METHOD     I2CPE_ECM_MICROSTEP
  #define I2CPE_ENC_2_EC_THRESH     0.10

  #define I2CPE_ENC_3_ADDR          I2CPE_PRESET_ADDR_Z     // Encoder 3.  Add additional configuration options
  #define I2CPE_ENC_3_AXIS          Z_AXIS                  // as above, or use defaults below.

  #define I2CPE_ENC_4_ADDR          I2CPE_PRESET_ADDR_E     // Encoder 4.
  #define I2CPE_ENC_4_AXIS          E_AXIS

  #define I2CPE_ENC_5_ADDR          34                      // Encoder 5.
  #define I2CPE_ENC_5_AXIS          E_AXIS

  // Default settings for encoders which are enabled, but without settings configured above.
  #define I2CPE_DEF_TYPE            I2CPE_ENC_TYPE_LINEAR
  #define I2CPE_DEF_ENC_TICKS_UNIT  2048
  #define I2CPE_DEF_TICKS_REV       (16 * 200)
  #define I2CPE_DEF_EC_METHOD       I2CPE_ECM_NONE
  #define I2CPE_DEF_EC_THRESH       0.1

  //#define I2CPE_ERR_THRESH_ABORT  100.0                   // Threshold size for error (in mm) error on any given
                                                            // axis after which the printer will abort. Comment out to
                                                            // disable abort behavior.

  #define I2CPE_TIME_TRUSTED        10000                   // After an encoder fault, there must be no further fault
                                                            // for this amount of time (in ms) before the encoder
                                                            // is trusted again.

  /**
   * Position is checked every time a new command is executed from the buffer but during long moves,
   * this setting determines the minimum update time between checks. A value of 100 works well with
   * error rolling average when attempting to correct only for skips and not for vibration.
   */
  #define I2CPE_MIN_UPD_TIME_MS     4                       // (ms) Minimum time between encoder checks.

  // Use a rolling average to identify persistent errors that indicate skips, as opposed to vibration and noise.
  #define I2CPE_ERR_ROLLING_AVERAGE

#endif // I2C_POSITION_ENCODERS

/**
 * Ethernet. Use M552 to enable and set the IP address.
 * @section network
 */
#if HAS_ETHERNET
  #define MAC_ADDRESS { 0xDE, 0xAD, 0xBE, 0xEF, 0xF0, 0x0D }  // A MAC address unique to your network
#endif

/**
 * WiFi Support (Espressif ESP32 WiFi)
 */
//#define WIFISUPPORT         // Marlin embedded WiFi managenent
//#define ESP3D_WIFISUPPORT   // ESP3D Library WiFi management (https://github.com/luc-github/ESP3DLib)

#if EITHER(WIFISUPPORT, ESP3D_WIFISUPPORT)
  //#define WEBSUPPORT          // Start a webserver (which may include auto-discovery)
  //#define OTASUPPORT          // Support over-the-air firmware updates
  //#define WIFI_CUSTOM_COMMAND // Accept feature config commands (e.g., WiFi ESP3D) from the host

  /**
   * To set a default WiFi SSID / Password, create a file called Configuration_Secure.h with
   * the following defines, customized for your network. This specific file is excluded via
   * .gitignore to prevent it from accidentally leaking to the public.
   *
   *   #define WIFI_SSID "WiFi SSID"
   *   #define WIFI_PWD  "WiFi Password"
   */
  //#include "Configuration_Secure.h" // External file with WiFi SSID / Password
#endif

// @section develop

//
// M100 Free Memory Watcher to debug memory usage
//
#define M100_FREE_MEMORY_WATCHER

//
// M42 - Set pin states
//
#define DIRECT_PIN_CONTROL

//
// M43 - display pin status, toggle pins, watch pins, watch digitalInputs & toggle LED, test servo probe
//
#define PINS_DEBUGGING

// Enable Tests that will run at startup and produce a report
//#define MARLIN_TEST_BUILD

// Enable Marlin dev mode which adds some special commands
//#define MARLIN_DEV_MODE

#if ENABLED(MARLIN_DEV_MODE)
  /**
   * D576 - Buffer Monitoring
   * To help diagnose print quality issues stemming from empty command buffers.
   */
  //#define BUFFER_MONITORING
#endif

/**
 * Postmortem Debugging captures misbehavior and outputs the CPU status and backtrace to serial.
 * When running in the debugger it will break for debugging. This is useful to help understand
 * a crash from a remote location. Requires ~400 bytes of SRAM and 5Kb of flash.
 */
//#define POSTMORTEM_DEBUGGING

/**
 * Software Reset options
 */
//#define SOFT_RESET_VIA_SERIAL         // 'KILL' and '^X' commands will soft-reset the controller
//#define SOFT_RESET_ON_KILL            // Use a digital button to soft-reset the controller after KILL

// Report uncleaned reset reason from register r2 instead of MCUSR. Supported by Optiboot on AVR.
//#define OPTIBOOT_RESET_REASON
