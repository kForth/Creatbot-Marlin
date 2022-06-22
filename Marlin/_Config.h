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

// Enable this for printers with a yellow motherboard (v9.5 or newer)
// Leave this disabled for printers with a green motherboard (older than v9.5)
//#define BOARD_VERSION_GT_V95

// This defines the number of extruders
// :[1, 2, 3, 4, 5]
#define EXTRUDERS 2

// This defines the extruder temp sensor type
// :[KTC, NTC]
#define TEMP_SENSOR KTC

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

#define USE_AUTOMATIC_VERSIONING

#define MOTHERBOARD BOARD_CREATBOT

#define BL_TOUCH_SIGNAL_SELF_FILTER

#include "_ModelInfo.h"

//===========================================================================
//============================= Thermal Settings ============================
//===========================================================================

// Preheat Constants
#if defined(F_SERIES) && defined(F_SERIES_ALLOW_500)
  #define PREHEAT_TEMP_HOTEND   400
  #define PREHEAT_TEMP_BED      120
  #define PREHEAT_FAN_SPEED     0
#else
  #define PREHEAT_TEMP_HOTEND   210
  #define PREHEAT_TEMP_BED      60
  #define PREHEAT_FAN_SPEED     0
#endif

#ifdef F_SERIES
  #ifdef F_SERIES_ALLOW_500
    #define HEATER_0_MAXTEMP  515
    #define HEATER_1_MAXTEMP  515
    #define HEATER_2_MAXTEMP  515
    #define HEATER_3_MAXTEMP  515
  #else
    #define HEATER_0_MAXTEMP  465
    #define HEATER_1_MAXTEMP  465
    #define HEATER_2_MAXTEMP  465
    #define HEATER_3_MAXTEMP  465
  #endif
  #define BED_MAXTEMP         215
#elif TEMP_SENSOR == KTC
  #define HEATER_0_MAXTEMP 415
  #define HEATER_1_MAXTEMP 415
  #define HEATER_2_MAXTEMP 415
  #define HEATER_3_MAXTEMP 415
  #define BED_MAXTEMP      135
#else // TEMP_SENSOR == NTC
  #define HEATER_0_MAXTEMP 265
  #define HEATER_1_MAXTEMP 265
  #define HEATER_2_MAXTEMP 265
  #define HEATER_3_MAXTEMP 265
  #define BED_MAXTEMP      135
#endif

// Heated Chamber
#ifdef HEATED_CHAMBER
  #define TEMP_SENSOR_CHAMBER     NTC
  #define CHAMBER_MINTEMP         -12
  #define CHAMBER_MAXTEMP         85
  #define MAX_CHAMBER_POWER       255
  #define PREHEAT_TEMP_CHAMBER    0
#else
  #define TEMP_SENSOR_CHAMBER     0
#endif

// Set Temperature Sensors
#if EXTRUDERS > 0
  #define TEMP_SENSOR_0 TEMP_SENSOR
#else
  #define TEMP_SENSOR_0 0
#endif
#if EXTRUDERS > 1
  #define TEMP_SENSOR_1 TEMP_SENSOR
#else
  #define TEMP_SENSOR_1 0
#endif
#if EXTRUDERS > 2
  #define TEMP_SENSOR_2 TEMP_SENSOR
#else
  #define TEMP_SENSOR_2 0
#endif
#if EXTRUDERS > 3
  #define TEMP_SENSOR_3 TEMP_SENSOR
#else
  #define TEMP_SENSOR_3 0
#endif
#if EXTRUDERS > 4
  #define TEMP_SENSOR_4 TEMP_SENSOR
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
#ifdef CHAMBER_FAN
  #define DEFAULT_AIR_FAN_SPEED 0
#endif

//===========================================================================
//============================= PID Settings ================================
//===========================================================================
#define PIDTEMP
#if ENABLED(PIDTEMP)
  //#define PID_DEBUG
  //#define PID_OPENLOOP 1
  #define PID_FUNCTIONAL_RANGE 10
  #define K1 0.95 //smoothing factor within the PID

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

#ifdef HEATED_CHAMBER
  #define HEATED_CHAMBER_HYSTERESIS  2
#endif // HEATED_CHAMBER


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
    #ifdef TEMP_SENSOR == KTC
      #define Z_MAX_POS 250
    #else // TEMP_SENSOR == NTC
      #define Z_MAX_POS 255
    #endif
  #else
    #define X_MAX_POS 255
    #define Y_MAX_POS 255
    #if MODEL == DM
      #ifdef TEMP_SENSOR == KTC
        #define Z_MAX_POS 300
      #else // TEMP_SENSOR == NTC
        #define Z_MAX_POS 305
      #endif
    #else // MODEL == DM_Plus
      #ifdef TEMP_SENSOR == KTC
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
    #ifdef TEMP_SENSOR == KTC
      #define Z_MAX_POS 300
    #else // TEMP_SENSOR == NTC
      #define Z_MAX_POS 305
    #endif
  #else // MODEL == DX_Plus
    #ifdef TEMP_SENSOR == KTC
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

//===========================================================================
//=============================== Bed Leveling ==============================
//===========================================================================
#ifdef AUTO_BED_LEVELING
  #define AUTO_BED_LEVELING_BILINEAR
  #define BLTOUCH

  #define NUM_SERVOS 1
  #define Z_ENDSTOP_SERVO_NR 0
  #define DEACTIVATE_SERVOS_AFTER_MOVE

  #define Z_SAFE_HOMING // safe homing to prevent servo cann't probe the bed.

  #if MODEL == DE || MODEL == DE_Plus
    #define GRID_MAX_POINTS_X 5
    #define GRID_MAX_POINTS_Y GRID_MAX_POINTS_X
    #ifdef BLTOUCH
      #define X_PROBE_OFFSET_FROM_EXTRUDER -25
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 60
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
    #else
      #define Z_SERVO_ANGLES {135,45} // {45,135}
      #define X_PROBE_OFFSET_FROM_EXTRUDER -25
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 60
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -8 // -6
    #endif
  #elif MODEL == D600 || MODEL == D600_SE || MODEL == D600_Pro
    #ifdef BLTOUCH
      #define GRID_MAX_POINTS_X 5
      #define GRID_MAX_POINTS_Y GRID_MAX_POINTS_X
      #ifdef F_SERIES
        #define X_PROBE_OFFSET_FROM_EXTRUDER 30
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 74
      #elif defined(D_SERIES)
        #define X_PROBE_OFFSET_FROM_EXTRUDER 51
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 45
      #endif
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
    #else
      #error "Not BL_TOUCH??"
    #endif
  #elif MODEL == Other_MODEL
    #define GRID_MAX_POINTS_X 5
    #define GRID_MAX_POINTS_Y GRID_MAX_POINTS_X
    #define X_PROBE_OFFSET_FROM_EXTRUDER -21
    #define Y_PROBE_OFFSET_FROM_EXTRUDER 22
    #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
  #elif MODEL == F430 || MODEL == F260
    #if MODEL == F260
      #define GRID_MAX_POINTS_X 4
      #define GRID_MAX_POINTS_Y GRID_MAX_POINTS_X
    #else
      #define GRID_MAX_POINTS_X 7
      #define GRID_MAX_POINTS_Y 5
    #endif
    #ifdef BLTOUCH
      #ifdef LINEAR_GUIDE
        #define X_PROBE_OFFSET_FROM_EXTRUDER 36
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 72
        #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
      #else
        #define X_PROBE_OFFSET_FROM_EXTRUDER 16
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 28
        #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
      #endif
    #else
      #define Z_SERVO_ANGLES {45,135}
      #ifdef LINEAR_GUIDE
        #define X_PROBE_OFFSET_FROM_EXTRUDER 36
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 70
        #define Z_PROBE_OFFSET_FROM_EXTRUDER -8
      #else
        #define X_PROBE_OFFSET_FROM_EXTRUDER 16
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 28
        #define Z_PROBE_OFFSET_FROM_EXTRUDER -8
      #endif
    #endif
  #elif MODEL == F160 || MODEL == F200 || MODEL == F220
    #define GRID_MAX_POINTS_X 3
    #define GRID_MAX_POINTS_Y GRID_MAX_POINTS_X
    #ifdef BLTOUCH
      #if MODEL == F160 || MODEL == F200
        #define X_PROBE_OFFSET_FROM_EXTRUDER 16
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 71
        #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
      #elif MODEL == F220
        #define X_PROBE_OFFSET_FROM_EXTRUDER 13
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 75
        #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
      #endif
    #else
      #define Z_SERVO_ANGLES {45,135}
      #define X_PROBE_OFFSET_FROM_EXTRUDER 16
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 28
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -8
    #endif
  #elif MODEL == PEEK300
    #define GRID_MAX_POINTS_X 5
    #define GRID_MAX_POINTS_Y GRID_MAX_POINTS_X
    #ifdef BLTOUCH
      #define X_PROBE_OFFSET_FROM_EXTRUDER 22
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 41
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
    #else
      #error "Not BL_TOUCH??"
    #endif
  #elif MODEL == F1000
    #define GRID_MAX_POINTS_X 10
    #define GRID_MAX_POINTS_Y GRID_MAX_POINTS_X
    #ifdef BLTOUCH
      #define X_PROBE_OFFSET_FROM_EXTRUDER 30
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 31
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
    #else
      #error "Not BL_TOUCH??"
    #endif
  #else
    #error "This MODEL don't have AUTO_BED_LEVELING Features."
  #endif

  #define LEFT_PROBE_BED_POSITION   (max(X_MIN_POS, X_MIN_POS + X_PROBE_OFFSET_FROM_EXTRUDER) + 15)
  #define RIGHT_PROBE_BED_POSITION  (min(X_MAX_POS, X_MAX_POS + X_PROBE_OFFSET_FROM_EXTRUDER) - 15)
  #define FRONT_PROBE_BED_POSITION  (max(Y_MIN_POS, Y_MIN_POS + Y_PROBE_OFFSET_FROM_EXTRUDER) + 15)
  #define BACK_PROBE_BED_POSITION   (min(Y_MAX_POS, Y_MAX_POS + Y_PROBE_OFFSET_FROM_EXTRUDER) - 15)
  
  #if MODEL == F430
    #define Z_SAFE_HOMING_X_POINT LEFT_PROBE_BED_POSITION // X point for Z homing when homing all axis (G28).
    #define Z_SAFE_HOMING_Y_POINT FRONT_PROBE_BED_POSITION // Y point for Z homing when homing all axis (G28).
  #else
    #define Z_SAFE_HOMING_X_POINT ((X_BED_SIZE) / 2)    // X point for Z homing when homing all axis (G28).
    #define Z_SAFE_HOMING_Y_POINT ((Y_BED_SIZE) / 2)    // Y point for Z homing when homing all axis (G28).
  #endif
#endif

//=============================================================================
//============================== Movement Settings ============================
//=============================================================================

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

//=============================================================================
//=============================== Extra Features ==============================
//=============================================================================

// Time intervals to save time used statistic
#define AUTO_TIME_USED_INTERVAL         1   //seconds
#define AUTO_SAVE_SETTING_INTERVAL      600 //seconds

// Auto shutdown timeouts
#ifdef POWER_MANAGEMENT
  #define AUTO_SHUTDOWN
  #ifdef AUTO_SHUTDOWN
    #define AUTO_SHUTDONW_TIME_DISPALY   180  //seconds = 3mins
    #define AUTO_SHUTDOWN_TIME_IDLE     3600  //seconds = 1hour
    #define AUTO_SHUTDOWN_TIME_HEATING  3600  //seconds = 1hour
    #ifdef FILAMENT_DETECT
      #define FILAMENT_WAIT_TIME        3600  //seconds = 2hours
    #endif
  #endif  //AUTO_SHUTDOWN
#endif //POWER_MANAGEMENT

// Quick Pause feature, similar to NOZZLE_PARK_FEATURE
#ifdef QUICK_PAUSE
  #ifdef BOWDEN
    #define PAUSE_STOP_X      3
    #define PAUSE_STOP_Y      3
    #define PAUSE_LIFT_HEIGH_INIT           1
    #define PAUSE_RETRACTION_LENGTH_INIT    5
    #define PAUSE_LIFT_HEIGH                10
    #define PAUSE_RETRACTION_LENGTH         15
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
    #define PAUSE_LIFT_HEIGH_INIT           1
    #define PAUSE_RETRACTION_LENGTH_INIT    2
    #define PAUSE_LIFT_HEIGH                10
    #define PAUSE_RETRACTION_LENGTH         6
  #endif

  #define PAUSE_REUSE_SPEED_LIFT            50    // (mm/s)
  #define PAUSE_REUSE_SPEED_MOVE            70    // (mm/s)
  #define REUSE_CLOSE_Z                     0.1
  #define REUSE_CLOSE_E                     1
  #define REUSE_SPEED_CLOSE                 1     // (mm/s)

  #ifdef ACCIDENT_DETECT
    #define ACCIDENT_Z_DOWN             2
    #ifdef BOWDEN
      #define ACCIDENT_E_RETRACTION     15  // Maybe it should be greater than 10
    #else // DIRECT_DRIVE
      #define ACCIDENT_E_RETRACTION     6
    #endif
    #define ACCIDENT_SPEED_TEST         1   // (mm/s)
  #endif //ACCIDENT_DETECT
#endif //QUICK_PAUSE

// Filament change settings
#define FILAMENT_UNLOAD_SPEED               50  // (mm/s)
#define FILAMENT_UNLOAD_EXTRUDER_LENGTH     10  // (mm)
#ifdef BOWDEN
  #define FILAMENT_UNLOAD_LENGTH            150
#else // DIRECT_DRIVE
  #define FILAMENT_UNLOAD_LENGTH            20
#endif

// Filament runout sensor settings
#ifdef FILAMENT_DETECT
  #define FILAMENT_DETECT_AGAIN
  #ifdef FILAMENT_DETECT_AGAIN
    #define FILAMENT_ERROR_BLOCK            50 //ms
  #endif
#endif

//=============================================================================
//============================= LCD and SD support ============================
//=============================================================================
#define LCD_LANGUAGE LANGUAGE

#ifdef MY_KEYPAD
  #define MY_KEYPAD_Z_MOVE_SCALE  5
#endif //MY_KEYPAD

#ifdef DWIN_LCD
  #define DWIN_Z_MOVE_SCALE       5
#endif //DWIN_LCD

#endif //CREATBOT_CONFIG_H
