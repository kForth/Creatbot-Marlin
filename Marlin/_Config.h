#ifndef CREATBOT_CONFIG_H
#define CREATBOT_CONFIG_H

//===========================================================================
//============================== Printer Model ==============================
//===========================================================================

/**
 * Select your printer model
 *
 * :[ 
 *    DM_Mini, DM, DM_Plus, DX, DX_Plus, DE, DE_Plus,D600, D600_Mini, D600_SE,
 *    D600_Pro, F160, F200, F220, F260, F300, F430, F1000, PEEK300 
 *  ]
 */
#define MODEL F430

#include "_ModelInfo.h"

// Enable this for printers with a yellow motherboard (v9.5 or newer)
// Leave this disabled for printers with a green motherboard (older than v9.5)
//#define BOARD_VERSION_GT_V95

// This defines the number of extruders
// :[1, 2, 3, 4, 5]
#define EXTRUDERS 2

// This defines the extruder temp sensor type
// :[KTC, NTC]
#define TEMP_SENSOR_EXTRUDER KTC

/**
 * LCD LANGUAGE
 *
 * Select the language to display on the LCD. These languages are available:
 *
 *    en, an, bg, ca, cn, cz, cz_utf8, de, el, el-gr, es, eu, fi, fr, gl, hr,
 *    it, kana, kana_utf8, nl, pl, pt, pt_utf8, pt-br, pt-br_utf8, ru, sk_utf8,
 *    tr, uk, zh_CN, zh_TW, test
 *
 * :{ 'en':'English', 'an':'Aragonese', 'bg':'Bulgarian', 'ca':'Catalan', 'cn':'Chinese', 'cz':'Czech', 'cz_utf8':'Czech (UTF8)', 'de':'German', 'el':'Greek', 'el-gr':'Greek (Greece)', 'es':'Spanish', 'eu':'Basque-Euskera', 'fi':'Finnish', 'fr':'French', 'gl':'Galician', 'hr':'Croatian', 'it':'Italian', 'kana':'Japanese', 'kana_utf8':'Japanese (UTF8)', 'nl':'Dutch', 'pl':'Polish', 'pt':'Portuguese', 'pt-br':'Portuguese (Brazilian)', 'pt-br_utf8':'Portuguese (Brazilian UTF8)', 'pt_utf8':'Portuguese (UTF8)', 'ru':'Russian', 'sk_utf8':'Slovak (UTF8)', 'tr':'Turkish', 'uk':'Ukrainian', 'zh_CN':'Chinese (Simplified)', 'zh_TW':'Chinese (Taiwan)', test':'TEST' }
 */
#define LANGUAGE en

#define MOTHERBOARD BOARD_CREATBOT


//===========================================================================
//============================= Stepper Drivers =============================
//===========================================================================

// Stepper Driver Types taken from this website
//https://www.3dprima.com/en/parts/spare-parts/creatbot/creatbot-stepper-motor-driver/a-22193?currency=USD
#if (MODEL == D600_Pro) || (MODEL == D600)
    #define X_DRIVER_TYPE  TB6600      // Supposedly
    #define Y_DRIVER_TYPE  TB6600      // Supposedly
    #define Z_DRIVER_TYPE  TB6600      // Supposedly
    #define E0_DRIVER_TYPE TB6560      // Supposedly a THB6032
#elif (MODEL == F430) || (MODEL == F160) || (MODEL == DE_Plus) || (MODEL == DX) || (MODEL == DX_Plus)
    #if MODEL == F430
        #define X_DRIVER_TYPE  TMC2100 // Supposedly a TMC2208 but my F430 had a TMC2100
        #define Y_DRIVER_TYPE  TB6560  // Supposedly a THB6032
    #else
        #define X_DRIVER_TYPE  TMC2208 // Supposedly
        #define Y_DRIVER_TYPE  TMC2208 // Supposedly
    #endif
    #define Z_DRIVER_TYPE  TB6560      // Supposedly a THB6032
    #define E0_DRIVER_TYPE TB6560      // Supposedly a THB6032
#elif (MODEL == F1000) // || (MODEL == New_D600_Pro) || (MODEL == New_D600)
    #define X_DRIVER_TYPE  A4988       // Supposedly a Servo
    #define Y_DRIVER_TYPE  A4988       // Supposedly a Servo
    #define Z_DRIVER_TYPE  TB6560      // Supposedly a THB6032
    #if MODEL == F1000
        #define E0_DRIVER_TYPE TB6600  // Supposedly
    #else
        #define E0_DRIVER_TYPE TB6560  // Supposedly a THB6032
    #endif
#else
    #define X_DRIVER_TYPE  A4988
    #define Y_DRIVER_TYPE  A4988
    #define Z_DRIVER_TYPE  A4988
    #define E0_DRIVER_TYPE A4988
#endif
#if EXTRUDER > 1
    #define E1_DRIVER_TYPE E0_DRIVER_TYPE
    #if EXTRUDERS > 2
        #define E2_DRIVER_TYPE E0_DRIVER_TYPE
    #endif
#endif


//===========================================================================
//============================= Thermal Settings ============================
//===========================================================================

// Preheat Constants
#if defined(F_SERIES) && defined(F_SERIES_ALLOW_500)
  #define PREHEAT_TEMP_HOTEND   400
  #define PREHEAT_TEMP_BED      120
  #define PREHEAT_FAN_SPEED     0
  #define PREHEAT_TEMP_CHAMBER  0
#else
  #define PREHEAT_TEMP_HOTEND   210
  #define PREHEAT_TEMP_BED      60
  #define PREHEAT_FAN_SPEED     0
  #define PREHEAT_TEMP_CHAMBER  0
#endif

#ifdef F_SERIES
  #ifdef F_SERIES_ALLOW_500
    #define HEATER_0_MAXTEMP  515
    #define HEATER_1_MAXTEMP  515
    #define HEATER_2_MAXTEMP  515
    #define HEATER_3_MAXTEMP  515
    #define HEATER_4_MAXTEMP  515
    #define HEATER_5_MAXTEMP  515
    #define HEATER_6_MAXTEMP  515
    #define HEATER_7_MAXTEMP  515
  #else
    #define HEATER_0_MAXTEMP  465
    #define HEATER_1_MAXTEMP  465
    #define HEATER_2_MAXTEMP  465
    #define HEATER_3_MAXTEMP  465
    #define HEATER_4_MAXTEMP  465
    #define HEATER_5_MAXTEMP  465
    #define HEATER_6_MAXTEMP  465
    #define HEATER_7_MAXTEMP  465
  #endif
  #define BED_MAXTEMP         215
#elif TEMP_SENSOR_EXTRUDER == KTC
  #define HEATER_0_MAXTEMP    415
  #define HEATER_1_MAXTEMP    415
  #define HEATER_2_MAXTEMP    415
  #define HEATER_3_MAXTEMP    415
  #define HEATER_4_MAXTEMP    465
  #define HEATER_5_MAXTEMP    465
  #define HEATER_6_MAXTEMP    465
  #define HEATER_7_MAXTEMP    465
  #define BED_MAXTEMP         135
#else // TEMP_SENSOR_EXTRUDER == NTC
  #define HEATER_0_MAXTEMP    265
  #define HEATER_1_MAXTEMP    265
  #define HEATER_2_MAXTEMP    265
  #define HEATER_3_MAXTEMP    265
  #define HEATER_4_MAXTEMP    465
  #define HEATER_5_MAXTEMP    465
  #define HEATER_6_MAXTEMP    465
  #define HEATER_7_MAXTEMP    465
  #define BED_MAXTEMP         135
#endif

// Heated Chamber
#ifdef HAS_CHAMBER_HEATER
  #define TEMP_SENSOR_CHAMBER     NTC
  #define CHAMBER_MINTEMP         -12
  #define CHAMBER_MAXTEMP         85
#else
  #define TEMP_SENSOR_CHAMBER     0
  #define CHAMBER_MINTEMP         5
  #define CHAMBER_MAXTEMP         75
#endif

// Set Temperature Sensors
#if EXTRUDERS > 0
  #define TEMP_SENSOR_0 TEMP_SENSOR_EXTRUDER
#else
  #define TEMP_SENSOR_0 0
#endif
#if EXTRUDERS > 1
  #define TEMP_SENSOR_1 TEMP_SENSOR_EXTRUDER
#else
  #define TEMP_SENSOR_1 0
#endif
#if EXTRUDERS > 2
  #define TEMP_SENSOR_2 TEMP_SENSOR_EXTRUDER
#else
  #define TEMP_SENSOR_2 0
#endif
#if EXTRUDERS > 3
  #define TEMP_SENSOR_3 TEMP_SENSOR_EXTRUDER
#else
  #define TEMP_SENSOR_3 0
#endif
#if EXTRUDERS > 4
  #define TEMP_SENSOR_4 TEMP_SENSOR_EXTRUDER
#else
  #define TEMP_SENSOR_4 0
#endif
#define TEMP_SENSOR_BED NTC

//These defines help to calibrate the AD595 sensor in case you get wrong temperature measurements.
//The measured temperature is defined as "actualTemp = (measuredTemp * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET"
#ifdef TEMP_ADJUST
  #if MODEL == F160 || MODEL == F260 || MODEL == F430
    #define TEMP_SENSOR_AD595_OFFSET  0.0
    #define TEMP_SENSOR_AD595_GAIN    1.0
  #else
    #define TEMP_SENSOR_AD595_OFFSET  4.796657303
    #define TEMP_SENSOR_AD595_GAIN    0.900138703
  #endif
#else
  #define TEMP_SENSOR_AD595_OFFSET    0.0
  #define TEMP_SENSOR_AD595_GAIN      1.0
#endif

// Default auto fan speed when eeprom resets
#define DEFAULT_AUTO_FAN_SPEED 0

// Default chamber fan speed when eeprom resets
#ifdef HAS_CHAMBER_FAN
    #define CHAMBER_FAN
    #define DEFAULT_AIR_FAN_SPEED 0
#endif

//===========================================================================
//============================= PID Settings ================================
//===========================================================================
#define PIDTEMP
#if ENABLED(PIDTEMP)
  #ifdef F_SERIES
    #define  DEFAULT_Kp 12.9
    #define  DEFAULT_Ki 0.76
    #define  DEFAULT_Kd 55
  #else
    #define  DEFAULT_Kp 16.57
    #define  DEFAULT_Ki 1.26
    #define  DEFAULT_Kd 55
  #endif
#endif // PIDTEMP

#define PIDTEMPBED
#if ENABLED(PIDTEMPBED)
	// CreatBot
	// 24V ??W silicone heater into
	#define  DEFAULT_bedKp 345.78
	#define  DEFAULT_bedKi 14.59
	#define  DEFAULT_bedKd 2047.99
#endif // PIDTEMP

//===========================================================================
//============================== Endstop Settings ===========================
//===========================================================================

#define X_MIN_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.
#define Y_MIN_ENDSTOP_INVERTING true // Set to true to invert the logic of the endstop.
#define Z_MIN_ENDSTOP_INVERTING false // Set to true to invert the logic of the endstop.
#define Z_MIN_PROBE_ENDSTOP_INVERTING false // Set to true to invert the logic of the probe.


//=============================================================================
//============================== Movement Settings ============================
//=============================================================================
// @section motion

/**
 * Default Axis Steps Per Unit (steps/mm)
 * Override with M92
 * 
 * Specify as {X, Y, Z, E0, E1?, E2?, E3?, E4?}
 */
#ifdef LINEAR_GUIDE
  #if MODEL == D600 || MODEL == D600_SE || MODEL == D600_Pro || MODEL == F1000
    #define DEFAULT_AXIS_STEPS_PER_UNIT   {800.0/3, 800.0/3, Z_STEP_VALUE, E_STEP_VALUE }
  #elif MODEL == Other_MODEL
    #define DEFAULT_AXIS_STEPS_PER_UNIT   {128.0/3, 128.0/3, Z_STEP_VALUE, E_STEP_VALUE }
  #elif MODEL == F430 || MODEL == F260 || MODEL == F200 || MODEL == F220
    // #define DEFAULT_AXIS_STEPS_PER_UNIT   {78.7402, 131.2336, Z_STEP_VALUE, E_STEP_VALUE }
    #define DEFAULT_AXIS_STEPS_PER_UNIT   {78.6816, 131.2591, Z_STEP_VALUE, E_STEP_VALUE } //Kestin
  #else
    #error "This MODEL don't have LINEAR_GUIDE system."
  #endif
#elif defined(HXY)
  #if MODEL == PEEK300
    #define DEFAULT_AXIS_STEPS_PER_UNIT   {78.7402, 78.7402, Z_STEP_VALUE, E_STEP_VALUE }
  #else
    #error "This MODEL don't have HXY system."
  #endif
#else
  #define DEFAULT_AXIS_STEPS_PER_UNIT   {78.7402, 78.7402, Z_STEP_VALUE, E_STEP_VALUE }
#endif

/**
 * Default Max Feed Rate (mm/s)
 * Override with M203
 *                                      X, Y, Z, E0 [, E1[, E2[, E3[, E4]]]]
 */
#define DEFAULT_MAX_FEEDRATE          { 300, 300, 10, 40 }

/**
 * Default Max Acceleration (speed change with time) (linear=mm/(s^2), rotational=°/(s^2))
 * (Maximum start speed for accelerated moves)
 * Override with M201
 *                                      X, Y, Z [, I [, J [, K...]]], E0 [, E1[, E2...]]
 */
#define DEFAULT_MAX_ACCELERATION      { 2000, 2000, 100, 3000 }

/**
 * Default Acceleration (speed change with time) (linear=mm/(s^2), rotational=°/(s^2))
 * Override with M204
 *
 *   M204 P    Acceleration
 *   M204 R    Retract Acceleration
 *   M204 T    Travel Acceleration
 */
#define DEFAULT_ACCELERATION          1500    // X, Y, Z and E acceleration for printing moves
#define DEFAULT_RETRACT_ACCELERATION  1500    // E acceleration for retracts
#define DEFAULT_TRAVEL_ACCELERATION   2000    // X, Y, Z acceleration for travel (non printing) moves

// Invert the stepper direction. Change (or reverse the motor connector) if an axis goes the wrong way.
#if defined(COREXY) || defined(HXY)
  #define INVERT_X_DIR true
  #define INVERT_Y_DIR true
  #define INVERT_Z_DIR true
#else
  #define INVERT_X_DIR true
  #define INVERT_Y_DIR false
  #define INVERT_Z_DIR true
#endif

// Default Z-Axis Steps Per Unit (steps/mm)
#ifdef Z_1605
  #if MODEL == F1000
    #define Z_STEP_VALUE 1600
  #else
    // #define Z_STEP_VALUE 640
    #define Z_STEP_VALUE 638.0153 //Kestin
  #endif
#else
  #if MODEL == F1000
    #define Z_STEP_VALUE 2000
  #else
    #define Z_STEP_VALUE 800
  #endif
#endif

// Default E-Axis Steps Per Unit (steps/mm)
#ifdef D_SERIES
  #ifdef D_SERIES_USE_175
    #define E_STEP_VALUE 810
  #else
    #define E_STEP_VALUE 600
  #endif
#elif defined(F_SERIES)
  #define E_STEP_VALUE 130
#else
  #error "Need define the model's series."
#endif

//===========================================================================
//============================= Z Probe Options =============================
//===========================================================================

#ifdef AUTO_BED_LEVELING
    #define BLTOUCH
    #define BL_TOUCH_SIGNAL_SELF_FILTER

    #define NUM_SERVOS 1
    #define SERVO_DELAY { 200 }
    
    // #define DEACTIVATE_SERVOS_AFTER_MOVE
    
    #define Z_SAFE_HOMING // safe homing to prevent servo cann't probe the bed.
    #if MODEL == F430
        #define Z_SAFE_HOMING_X_POINT PROBING_MARGIN  // X point for Z homing
        #define Z_SAFE_HOMING_Y_POINT PROBING_MARGIN  // Y point for Z homing
    #else
        #define Z_SAFE_HOMING_X_POINT X_CENTER  // X point for Z homing
        #define Z_SAFE_HOMING_Y_POINT Y_CENTER  // Y point for Z homing
    #endif

  #if MODEL == DE || MODEL == DE_Plus
    #ifdef BLTOUCH
      #define NOZZLE_TO_PROBE_OFFSET { -25, 60, -1 }
    #else
      #define Z_SERVO_ANGLES {135,45} // {45,135}
      #define NOZZLE_TO_PROBE_OFFSET { -25, 60,  -8 }// -6
    #endif
  #elif MODEL == D600 || MODEL == D600_SE || MODEL == D600_Pro
    #ifdef BLTOUCH
      #ifdef F_SERIES
        #define NOZZLE_TO_PROBE_OFFSET { 30, 74, -1 }
      #elif defined(D_SERIES)
        #define NOZZLE_TO_PROBE_OFFSET { 51, 45, -1 }
      #endif
    #else
      #error "Not BL_TOUCH??"
    #endif
  #elif MODEL == F430 || MODEL == F260
    #ifdef BLTOUCH
      #ifdef LINEAR_GUIDE
        #define NOZZLE_TO_PROBE_OFFSET { 36, 72, -1 }
      #else
        #define NOZZLE_TO_PROBE_OFFSET { 16, 28, -1 }
      #endif
    #else
      #define Z_SERVO_ANGLES {45,135}
      #ifdef LINEAR_GUIDE
        #define NOZZLE_TO_PROBE_OFFSET { 36, 70, -8 }
      #else
        #define NOZZLE_TO_PROBE_OFFSET { 16, 28, -8 }
      #endif
    #endif
  #elif MODEL == F160 || MODEL == F200 || MODEL == F220
    #ifdef BLTOUCH
      #if MODEL == F160 || MODEL == F200
        #define NOZZLE_TO_PROBE_OFFSET { 16, 71, -1 }
      #elif MODEL == F220
        #define NOZZLE_TO_PROBE_OFFSET { 13, 75, -1 }
      #endif
    #else
      #define Z_SERVO_ANGLES {45,135}
      #define NOZZLE_TO_PROBE_OFFSET { 16, 28, -8 }
    #endif
  #elif MODEL == PEEK300
    #ifdef BLTOUCH
      #define NOZZLE_TO_PROBE_OFFSET { 22, 41, -1 }
    #else
      #error "Not BL_TOUCH??"
    #endif
  #elif MODEL == F1000
    #ifdef BLTOUCH
      #define NOZZLE_TO_PROBE_OFFSET { 30, 31, -1 }
    #else
      #error "Not BL_TOUCH??"
    #endif
  #elif MODEL == Other_MODEL
    #define NOZZLE_TO_PROBE_OFFSET { -21, 22, -1 }
  #else
    #error "This MODEL don't have AUTO_BED_LEVELING Features."
  #endif

//   #define PROBING_MARGIN_LEFT   max(NOZZLE_TO_PROBE_OFFSET[0], 5)
//   #define PROBING_MARGIN_RIGHT  min(NOZZLE_TO_PROBE_OFFSET[0], 5)
//   #define PROBING_MARGIN_FRONT  max(NOZZLE_TO_PROBE_OFFSET[1], 5)
//   #define PROBING_MARGIN_BACK   min(NOZZLE_TO_PROBE_OFFSET[1], 5)
    #define PROBING_MARGIN 15
#endif


//===========================================================================
//=============================== Bed Leveling ==============================
//===========================================================================

#ifdef AUTO_BED_LEVELING

  #define AUTO_BED_LEVELING_BILINEAR

  #define RESTORE_LEVELING_AFTER_G28

  #define HOMING_FEEDRATE_MM_M { (50*60), (50*60), (20*60) }

  #if MODEL == DE || MODEL == DE_Plus
    #define GRID_MAX_POINTS_X 5
    #define GRID_MAX_POINTS_Y 5
  #elif MODEL == D600 || MODEL == D600_SE || MODEL == D600_Pro
    #define GRID_MAX_POINTS_X 5
    #define GRID_MAX_POINTS_Y 5
  #elif MODEL == Other_MODEL
    #define GRID_MAX_POINTS_X 5
    #define GRID_MAX_POINTS_Y 5
  #elif MODEL == F430
    #define GRID_MAX_POINTS_X 7
    #define GRID_MAX_POINTS_Y 5
  #elif MODEL == F260
    #define GRID_MAX_POINTS_X 4
    #define GRID_MAX_POINTS_Y 4
  #elif MODEL == F160 || MODEL == F200 || MODEL == F220
    #define GRID_MAX_POINTS_X 3
    #define GRID_MAX_POINTS_Y 3
  #elif MODEL == PEEK300
    #define GRID_MAX_POINTS_X 5
    #define GRID_MAX_POINTS_Y 5
  #elif MODEL == F1000
    #define GRID_MAX_POINTS_X 10
    #define GRID_MAX_POINTS_XY10
  #else
    #error "This MODEL don't have AUTO_BED_LEVELING Features."
  #endif

#endif

//===========================================================================
//================================ Bed Size =================================
//===========================================================================
#define X_MIN_POS 0
#define Y_MIN_POS 0
#define Z_MIN_POS 0

#if MODEL == DM_Mini || MODEL == DM || MODEL == DM_Plus
  #if MODEL == DM_Mini
    #define X_MAX_POS 205
    #define Y_MAX_POS 205
    #ifdef TEMP_SENSOR_EXTRUDER == KTC
      #define Z_MAX_POS 250
    #else // TEMP_SENSOR_EXTRUDER == NTC
      #define Z_MAX_POS 255
    #endif
  #else
    #define X_MAX_POS 255
    #define Y_MAX_POS 255
    #if MODEL == DM
      #ifdef TEMP_SENSOR_EXTRUDER == KTC
        #define Z_MAX_POS 300
      #else // TEMP_SENSOR_EXTRUDER == NTC
        #define Z_MAX_POS 305
      #endif
    #else // MODEL == DM_Plus
      #ifdef TEMP_SENSOR_EXTRUDER == KTC
        #define Z_MAX_POS 450
      #else
        #define Z_MAX_POS 455
      #endif
    #endif
  #endif
#elif MODEL == DX || MODEL == DX_Plus
  #if EXTRUDERS == 3
    #define X_MAX_POS 280
  #else // EXTRUDERS <= 2
    #define X_MAX_POS 305
  #endif
  #define Y_MAX_POS 255
  #if MODEL == DX
    #ifdef TEMP_SENSOR_EXTRUDER == KTC
      #define Z_MAX_POS 300
    #else // TEMP_SENSOR_EXTRUDER == NTC
      #define Z_MAX_POS 305
    #endif
  #else // MODEL == DX_Plus
    #ifdef TEMP_SENSOR_EXTRUDER == KTC
      #define Z_MAX_POS 520
    #else
      #define Z_MAX_POS 525
    #endif
  #endif
#elif MODEL == DE || MODEL == DE_Plus
  #if EXTRUDERS == 3
    #define X_MAX_POS 385
  #else // EXTRUDERS <= 2
    #define X_MAX_POS 405
  #endif
  #define Y_MAX_POS 305
  #if MODEL == DE
    #define Z_MAX_POS 300
  #else // MODEL == DE_Plus
    #define Z_MAX_POS 520
  #endif
#elif MODEL == D600  || MODEL == D600_Pro
  #if EXTRUDERS == 3
    #define X_MAX_POS 580
  #else
    #define X_MAX_POS 600
  #endif
  #define Y_MAX_POS 600
  #define Z_MAX_POS 600
#elif MODEL == D600_Mini
  #define X_MAX_POS 600
  #define Y_MAX_POS 600
  #define Z_MAX_POS 300
#elif MODEL == D600_SE
  #define X_MAX_POS 600
  #define Y_MAX_POS 400
  #define Z_MAX_POS 400
#elif MODEL == F300
  #define X_MAX_POS 300
  #define Y_MAX_POS 300
  #define Z_MAX_POS 400
#elif MODEL == F160
  #define X_MAX_POS 160
  #define Y_MAX_POS 160
  #define Z_MAX_POS 200
#elif MODEL == F200
  #define X_MAX_POS 200
  #define Y_MAX_POS 200
  #define Z_MAX_POS 300
#elif MODEL == F220
  #define X_MAX_POS 220
  #define Y_MAX_POS 220
  #define Z_MAX_POS 260
#elif MODEL == F260
  #define X_MAX_POS 260
  #define Y_MAX_POS 260
  #define Z_MAX_POS 300
#elif MODEL == F430
  #define X_MAX_POS 400
  #define Y_MAX_POS 300
  #define Z_MAX_POS 300
#elif MODEL == PEEK300
  #define X_MAX_POS 300
  #define Y_MAX_POS 300
  #define Z_MAX_POS 300
#elif MODEL == F1000
  #define X_MAX_POS 1000
  #define Y_MAX_POS 1000
  #define Z_MAX_POS 1000
#elif MODEL == Other_MODEL
  #define X_MAX_POS 600
  #define Y_MAX_POS 600
  #define Z_MAX_POS 50
#else
  #error "Unknown size info."
#endif

//=============================================================================
//============================== Filament Runout ==============================
//=============================================================================

#define FILAMENT_RUNOUT_SENSOR
#if ENABLED(FILAMENT_RUNOUT_SENSOR)
    #define NUM_RUNOUT_SENSORS EXTRUDERS // Number of sensors, up to one per extruder. Define a FIL_RUNOUT#_PIN for each.

    #define FIL_RUNOUT_STATE     LOW        // Pin state indicating that filament is NOT present.
    #define FIL_RUNOUT_PULLUP               // Use nternal pullup for filament runout pins.
    //#define FIL_RUNOUT_PULLDOWN           // Use internal pulldown for filament runout pins.
    //#define WATCH_ALL_RUNOUT_SENSORS      // Execute runout script on any triggering sensor, not only for the active extruder.
    
    // Commands to execute on filament runout.
    // With multiple runout sensors use the %c placeholder for the current tool in commands (e.g., "M600 T%c")
    // NOTE: After 'M412 H1' the host handles filament runout and this script does not apply.
    #define FILAMENT_RUNOUT_SCRIPT "M600"
    
#endif

// Time intervals to save time used statistic
#define AUTO_TIME_USED_INTERVAL         1   //seconds
#define AUTO_SAVE_SETTING_INTERVAL      600 //seconds

/**
 * Continue after Power-Loss (Creality3D)
 *
 * Store the current state to the SD Card at the start of each layer
 * during SD printing. If the recovery file is found at boot time, present
 * an option on the LCD screen to continue the print from the last-known
 * point in the file.
 */
#ifdef ACCIDENT_DETECT
    // #define POWER_LOSS_RECOVERY //TODO: Needs SD/USB support first

    #ifdef BOWDEN
        #define POWER_LOSS_RETRACT_LEN     15
    #else // DIRECT_DRIVE
        #define POWER_LOSS_RETRACT_LEN     6
    #endif
#endif // ACCIDENT_DETECT


/**
 * Power Supply Control
 */
#ifdef POWER_MANAGEMENT
    #define PSU_CONTROL
    #ifdef AUTO_SHUTDOWN
      #define AUTO_SHUTDONW_TIME_DISPALY   180  //seconds = 3mins
      #define AUTO_SHUTDOWN_TIME_IDLE     3600  //seconds = 1hour
      #define AUTO_SHUTDOWN_TIME_HEATING  3600  //seconds = 1hour
      #define FILAMENT_WAIT_TIME          3600  //seconds = 2hours
    #endif  // AUTO_SHUTDOWN
#endif // POWER_MANAGEMENT

/**
 * Nozzle Park
 */
#define NOZZLE_PARK_FEATURE // Park the nozzle at the given XYZ position on idle or G27.
#define ADVANCED_PAUSE_FEATURE // Advanced Pause for Filament Change
#define PARK_HEAD_ON_PAUSE  // Park the nozzle during pause and filament change.
// Specify a park position as { X, Y, Z_raise }
#ifdef BOWDEN
    #define NOZZLE_PARK_POINT { 3, 3, 10 }
#else // DIRECT_DRIVE
    #if MODEL == F160
        #define PAUSE_STOP_X      (X_MAX_POS - 10)
        #define PAUSE_STOP_Y      (Y_MAX_POS - 10)
    #elif MODEL == F430 || MODEL == PEEK300
        #define PAUSE_STOP_X      (X_MAX_POS/2)
        #define PAUSE_STOP_Y      (220)
    #elif MODEL == D600 || MODEL == D600_SE || MODEL == D600_Pro
        #define PAUSE_STOP_X      (200)
        #define PAUSE_STOP_Y      (220)
    #elif MODEL == F1000
        #define PAUSE_STOP_X      (X_MAX_POS/2)
        #define PAUSE_STOP_Y      (10)
    #elif MODEL == Other_MODEL
        #define PAUSE_STOP_X      (X_MAX_POS/2)
        #define PAUSE_STOP_Y      (10)
    #else
        #define PAUSE_STOP_X      (X_MAX_POS - 10)
        #define PAUSE_STOP_Y      (Y_MAX_POS - 10)
    #endif
    #define NOZZLE_PARK_POINT { PAUSE_STOP_X, PAUSE_STOP_Y, 10 }
#endif

// Filament change settings
#define FILAMENT_CHANGE_UNLOAD_FEEDRATE     50  // (mm/s)
#define FILAMENT_UNLOAD_EXTRUDER_LENGTH     10  // (mm)
#ifdef BOWDEN
  #define FILAMENT_CHANGE_UNLOAD_LENGTH     150
#else // DIRECT_DRIVE
  #define FILAMENT_CHANGE_UNLOAD_LENGTH     20
#endif

//=============================================================================
//======================== LCD / Controller Selection =========================
//=============================================================================

/**
 * LCD LANGUAGE
 *
 * Select the language to display on the LCD.
 */
#define LCD_LANGUAGE LANGUAGE

#ifdef HAS_KEYPAD
  #define MY_KEYPAD_Z_MOVE_SCALE  5
#endif //HAS_KEYPAD

#ifdef HAS_LCD
  #define DWIN_Z_MOVE_SCALE       5
#endif //HAS_LCD

#ifdef HAS_SD_READER
    #define SDSUPPORT
#endif

#ifdef HAS_USB_READER
    // #define SDSUPPORT
    // #define USB_FLASH_DRIVE_SUPPORT
    // #define USE_CH376_USB // TODO: Implement this
#endif

//=============================================================================
//========================== Extruder Cooling Fans ============================
//=============================================================================

#define E0_AUTO_FAN_PIN 8
#define E1_AUTO_FAN_PIN 8
#define E2_AUTO_FAN_PIN 8
#define E3_AUTO_FAN_PIN 8
#define E4_AUTO_FAN_PIN 8
#define E5_AUTO_FAN_PIN 8
#define E6_AUTO_FAN_PIN 8
#define E7_AUTO_FAN_PIN 8

#define EXTRUDER_AUTO_FAN_TEMPERATURE 100

#endif //CREATBOT_CONFIG_H
