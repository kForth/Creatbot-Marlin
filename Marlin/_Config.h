#ifndef _CONFIG_H
#define _CONFIG_H

#include "_Custom.h"

#ifdef NTC
  #define T_SENSENR   1     // NTC
  #define HEATER_0_MAXTEMP    265
  #define HEATER_1_MAXTEMP    265
  #define HEATER_2_MAXTEMP    265
  #define HEATER_3_MAXTEMP    265
  #define BED_MAXTEMP         135
#elif defined(KTC)
  #define T_SENSENR   -1    // KTC
  #ifdef T350
    #define HEATER_0_MAXTEMP  365
    #define HEATER_1_MAXTEMP  365
    #define HEATER_2_MAXTEMP  365
    #define HEATER_3_MAXTEMP  365
    #define BED_MAXTEMP       135
  #elif defined(T450)
    #define HEATER_0_MAXTEMP  465
    #define HEATER_1_MAXTEMP  465
    #define HEATER_2_MAXTEMP  465
    #define HEATER_3_MAXTEMP  465
    #define BED_MAXTEMP       165
  #elif defined(T500)
    #define HEATER_0_MAXTEMP  515
    #define HEATER_1_MAXTEMP  515
    #define HEATER_2_MAXTEMP  515
    #define HEATER_3_MAXTEMP  515
    #define BED_MAXTEMP       215
  #endif
#endif

#define PREHEAT_TEMP_HOTEND   210
#define PREHEAT_TEMP_BED      45
#define PREHEAT_FAN_SPEED     0

#ifdef HOTWIND_SYSTEM
  #define TEMP_SENSOR_CHAMBER     1     // NTC
  #define CHAMBER_MINTEMP         -12
  #ifdef T500
    #define CHAMBER_MAXTEMP       125
  #else
    #define CHAMBER_MAXTEMP       75
  #endif
  #define MAX_CHAMBER_POWER       255
  #define PREHEAT_TEMP_CHAMBER    0
#else
  #define TEMP_SENSOR_CHAMBER     0
#endif


#if EXTRUDERS > 0
  #define TEMP_SENSOR_0 T_SENSENR
#else
  #define TEMP_SENSOR_0 0
#endif
#if EXTRUDERS > 1
  #define TEMP_SENSOR_1 T_SENSENR
#else
  #define TEMP_SENSOR_1 0
#endif
#if EXTRUDERS > 2
  #define TEMP_SENSOR_2 T_SENSENR
#else
  #define TEMP_SENSOR_2 0
#endif
#if EXTRUDERS > 3
  #define TEMP_SENSOR_3 T_SENSENR
#else
  #define TEMP_SENSOR_3 0
#endif
#if EXTRUDERS > 4
  #define TEMP_SENSOR_4 T_SENSENR
#else
  #define TEMP_SENSOR_4 0
#endif
#define TEMP_SENSOR_BED 1         // NTC

// KTC TEMP ADJUST
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

// PID Settting
#define PIDTEMP
#if ENABLED(PIDTEMP)
  //#define PID_DEBUG
  //#define PID_OPENLOOP 1
  #define K1 0.95 //smoothing factor within the PID
  #ifdef USE_HEATING_TUBE_80W
    #define PID_FUNCTIONAL_RANGE 30
  #else
    #define PID_FUNCTIONAL_RANGE 20
  #endif

  #ifdef F_SERIES
    #if MODEL == PEEK300
      #define DEFAULT_Kp 8.53
      #define DEFAULT_Ki 0.42
      #define DEFAULT_Kd 43
      #define PID_PARAMS_TEMP_RANGE   // Add PID_PARAMS_TEMP_RANGE
    #else
      #define DEFAULT_Kp 12.9
      #define DEFAULT_Ki 0.76
      #define DEFAULT_Kd 55
    #endif

    #ifdef PID_PARAMS_TEMP_RANGE
      #define PID_PARAMS_TEMP_RANGE_NUM  3
      #define PID_TEMP_RANGE_ARRAY { 0, 350, 450 }

      #define DEFAULT_Kp_ARRAY { DEFAULT_Kp, 10.48, 10.79 }
      #define DEFAULT_Ki_ARRAY { DEFAULT_Ki, 0.55,  0.52  }
      #define DEFAULT_Kd_ARRAY { DEFAULT_Kd, 50,    56    }
    #endif
  #else
    #define  DEFAULT_Kp 16.57
    #define  DEFAULT_Ki 1.26
    #define  DEFAULT_Kd 55
  #endif

  #define PID_PARAMS_USE_TEMP_RANGE (ENABLED(PID_PARAMS_TEMP_RANGE) && PID_PARAMS_TEMP_RANGE_NUM > 1)
#endif // PIDTEMP

#define PIDTEMPBED
#if ENABLED(PIDTEMPBED)
  #if MODEL == D600 || MODEL == D600SE || MODEL == D600Pro
    // 220 1250W silicone heater
    #define SLOW_PID_BED
    #ifdef SLOW_PID_BED
      #define  DEFAULT_bedKp 37.28
      #define  DEFAULT_bedKi 1.23
      #define  DEFAULT_bedKd 281.7
    #else
      #define  DEFAULT_bedKp 43.54
      #define  DEFAULT_bedKi 3.72
      #define  DEFAULT_bedKd 127.5
    #endif
  #else
    // 24V ??W silicone heater
    #define  DEFAULT_bedKp 345.78
    #define  DEFAULT_bedKi 14.59
    #define  DEFAULT_bedKd 2047.99
  #endif
#endif // PIDTEMPBED

#ifdef HOTWIND_SYSTEM
  #define PIDTEMP_CHAMBER
  #define chamberK1 0.95
  #ifdef PIDTEMP_CHAMBER
    #define SLOW_PID_CHAMBER
    #if MODEL == F160 || MODEL == F160PEEK
      #define PID_RANGE_CHAMBER 3         //积分时间24秒，3个PID周期
      #define DEFAULT_chamberKp 115.24
      #define DEFAULT_chamberKi 4.78
      #define DEFAULT_chamberKd 693.86
    #elif MODEL == F430
      #define PID_RANGE_CHAMBER 3         //积分时间24秒，3个PID周期
      #define DEFAULT_chamberKp 88.87
      #define DEFAULT_chamberKi 3.69
      #define DEFAULT_chamberKd 535.07
    #elif MODEL == D600 || MODEL == D600SE || MODEL == D600Pro
      #define PID_RANGE_CHAMBER 3         //积分时间40秒，5个PID周期
      #define DEFAULT_chamberKp 42.02
      #define DEFAULT_chamberKi 1.05
      #define DEFAULT_chamberKd 421.64
    #elif MODEL == F1000
      #undef PIDTEMP_CHAMBER    //F1000采用分段加热继电器
    #elif MODEL == PEEK300
      #define PID_RANGE_CHAMBER 10        //积分时间40秒，5个PID周期
      #define DEFAULT_chamberKp 16.57
      #define DEFAULT_chamberKi 0.41
      #define DEFAULT_chamberKd 166.31
    #else
      #error "need test"
    #endif
  #else
    #define CHAMBER_HYSTERESIS        2 // 度
  #endif // PIDTEMP_CHAMBER
#endif 

#ifdef AUTO_BED_LEVELING
  #define AUTO_BED_LEVELING_BILINEAR

  #ifndef USE_CUSTOM_SERVO0
    #define BLTOUCH
  #else
    #define DEACTIVATE_SERVOS_AFTER_MOVE
  #endif

  #define NUM_SERVOS 1
  #define Z_ENDSTOP_SERVO_NR 0
  #define Z_SAFE_HOMING        // safe homing to prevent servo cann't probe the bed.

  #if MODEL == DE || MODEL == DEPlus
    #define PROBE_XY_NUM 5
    #ifdef BLTOUCH
      #define X_PROBE_OFFSET_FROM_EXTRUDER -25
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 60
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
    #else
      #define Z_SERVO_ANGLES {135,45}
//      #define Z_SERVO_ANGLES {45,135}
      #define X_PROBE_OFFSET_FROM_EXTRUDER -25
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 60
//      #define Z_PROBE_OFFSET_FROM_EXTRUDER -6
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -8
    #endif
  #elif MODEL == D600 || MODEL == D600SE || MODEL == D600Pro
    #define PROBE_XY_NUM 7
    #ifdef BLTOUCH
      #ifdef F_SERIES
        #define X_PROBE_OFFSET_FROM_EXTRUDER 30
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 74
      #elif defined(D_SERIES)
        #define X_PROBE_OFFSET_FROM_EXTRUDER 51
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 45
      #endif
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
    #else
      #ifdef USE_CUSTOM_SERVO0
        #define SERVO0_MIN_PULSE  348
        #define SERVO0_MAX_PULSE  2636
      #endif
      #define Z_SERVO_ANGLES {5, 175}
      #define X_PROBE_OFFSET_FROM_EXTRUDER 74
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 72
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -14
    #endif
  #elif MODEL == F430 || MODEL == F260
    #if MODEL == F260
      #define PROBE_XY_NUM 4
    #else
      #define PROBE_XY_NUM 5
    #endif
    #ifdef MAKE_Y_DUAL_STEPPER
      #undef BLTOUCH
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
  #elif MODEL == F430Plus
    #define PROBE_XY_NUM 5
    #ifdef BLTOUCH
      #define X_PROBE_OFFSET_FROM_EXTRUDER 30
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 64
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
    #else
      #error "Not BL_TOUCH??"
    #endif
  #elif MODEL == F160 || MODEL == F200 || MODEL == F220
    #define PROBE_XY_NUM 3
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
    #define PROBE_XY_NUM 5
    #ifdef BLTOUCH
      #ifdef HXY
        #define X_PROBE_OFFSET_FROM_EXTRUDER 22
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 41
        #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
      #else
        #define X_PROBE_OFFSET_FROM_EXTRUDER 23
        #define Y_PROBE_OFFSET_FROM_EXTRUDER 42
        #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
      #endif
    #else
      #ifdef USE_CUSTOM_SERVO0
        #define SERVO0_MIN_PULSE  512
        #define SERVO0_MAX_PULSE  2528
        #define SERVO0_DEALY 350
      #endif
      #define Z_SERVO_ANGLES {75, 105}
      #define X_PROBE_OFFSET_FROM_EXTRUDER 35
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 68
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -2
    #endif
    #elif MODEL == F1000
    #define PROBE_XY_NUM 10
    #ifdef BLTOUCH
      #define X_PROBE_OFFSET_FROM_EXTRUDER 30
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 31
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
    #else
      #ifdef USE_CUSTOM_SERVO0
        #define SERVO0_MIN_PULSE  348
        #define SERVO0_MAX_PULSE  2636
        #define SERVO0_DEALY      1800
      #endif
      #define Z_SERVO_ANGLES {20, 160}
      #define X_PROBE_OFFSET_FROM_EXTRUDER 46
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 57
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -13
    #endif
  #else
    #error "This MODEL don't have AUTO_BED_LEVELING Features."
  #endif

  #define LEFT_PROBE_BED_POSITION   (max(X_MIN_POS, X_MIN_POS + X_PROBE_OFFSET_FROM_EXTRUDER) + 5)
  #define RIGHT_PROBE_BED_POSITION  (min(X_MAX_POS, X_MAX_POS + X_PROBE_OFFSET_FROM_EXTRUDER) - 5)
  #define FRONT_PROBE_BED_POSITION  (max(Y_MIN_POS, Y_MIN_POS + Y_PROBE_OFFSET_FROM_EXTRUDER) + 5)
  #define BACK_PROBE_BED_POSITION   (min(Y_MAX_POS, Y_MAX_POS + Y_PROBE_OFFSET_FROM_EXTRUDER) - 5)
#endif

#ifdef BLTOUCH
  #undef SERVO_SPEED_SLOWLY //incompatible
  #define SERVO0_DEALY  50  //ms
#else
  #ifndef SERVO0_DEALY
    #define SERVO0_DEALY  300 //ms
  #endif
#endif

#define SERVO_DELAY { SERVO0_DEALY }

#ifdef SWITCHING_NOZZLE
  #undef NUM_SERVOS
  #define NUM_SERVOS 2

  #undef SERVO_DELAY
  #define SERVO_DELAY { SERVO0_DEALY, 1400 }

  #define SWITCHING_NOZZLE_SERVO_NR     1
  #define SWITCHING_NOZZLE_SERVO_ANGLES { 30, 150 }   // Angles for E0, E1
  #define ALIGN_NOZZLE_AGNLE            90    //deg

  #define SWITCHING_NOZZLE_MIN_PULSE    512   //us
  #define SWITCHING_NOZZLE_MAX_PULSE    2528  //us

  #define SWITCHING_NOZZLE_E_CHANGE     4     //mm
  #define SWITCHING_NOZZLE_E_SPEED      80    //mm/s
#endif


#if defined(COREXY) || defined(HXY)
  #define INVERT_X_DIR true
  #define INVERT_Y_DIR true
  #define INVERT_Z_DIR true
#else
  #define INVERT_X_DIR true
  #define INVERT_Y_DIR false
  #define INVERT_Z_DIR true
#endif


#ifdef Z05
  #if MODEL == F1000
    #define Z_STEP_VALUE 1920
  #elif MODEL == PEEK300
    #define Z_STEP_VALUE 1600
  #else
    #define Z_STEP_VALUE 640
  #endif
#else
  #if MODEL == F1000
    #define Z_STEP_VALUE 2400
  #elif MODEL == PEEK300
    #define Z_STEP_VALUE 2000
  #else
    #define Z_STEP_VALUE 800
  #endif
#endif

#ifdef D_SERIES
  #ifdef D_SERIES_USE_300
    #define E_STEP_VALUE 600
  #else
    #define E_STEP_VALUE 810
  #endif
#elif defined(F_SERIES)
  #define E_STEP_VALUE 142
#else
  #error "Need define the model's series."
#endif

#ifdef LINEAR_GUIDE
  #if MODEL == D600 || MODEL == D600SE || MODEL == D600Pro || MODEL == F1000
    #ifdef USE_SIFU_MOTOR
      #define DEFAULT_AXIS_STEPS_PER_UNIT   {800.0/3, 800.0/3, Z_STEP_VALUE, E_STEP_VALUE }
    #else
      #define DEFAULT_AXIS_STEPS_PER_UNIT   {320.0/3, 320.0/3, Z_STEP_VALUE, E_STEP_VALUE }
    #endif
  #elif MODEL == F430 || MODEL == F430Plus || MODEL == F260 || MODEL == F200 || MODEL == F220
    #ifdef MAKE_Y_DUAL_STEPPER
      #define DEFAULT_AXIS_STEPS_PER_UNIT   {10000.0/127, 20000.0/381, Z_STEP_VALUE, E_STEP_VALUE }
    #else
      #define DEFAULT_AXIS_STEPS_PER_UNIT   {10000.0/127, 50000.0/381, Z_STEP_VALUE, E_STEP_VALUE }
    #endif
  #else
    #error "This MODEL don't have LINEAR_GUIDE system."
  #endif
#elif defined(HXY)
  #if MODEL == PEEK300
    #define DEFAULT_AXIS_STEPS_PER_UNIT   {10000.0/127, 10000.0/127, Z_STEP_VALUE, E_STEP_VALUE }
  #else
    #error "This MODEL don't have HXY system."
  #endif
#else
  #if MODEL == PEEK300
    #define DEFAULT_AXIS_STEPS_PER_UNIT   {80, 80, Z_STEP_VALUE, E_STEP_VALUE }
  #else
    #define DEFAULT_AXIS_STEPS_PER_UNIT   {10000.0/127, 10000.0/127, Z_STEP_VALUE, E_STEP_VALUE }
  #endif
#endif

#if MODEL == F1000
  #define DEFAULT_MAX_FEEDRATE          { 300, 300, 7.5, 40 }
#elif MODEL == PEEK300
  #define DEFAULT_MAX_FEEDRATE          { 300, 300, 6, 40 }
#else
  #define DEFAULT_MAX_FEEDRATE          { 300, 300, 10, 40 }
#endif
#define DEFAULT_MAX_ACCELERATION        { 2000, 2000, 10, 3000 }


#define LCD_LANGUAGE LANGUAGE

#ifdef ULTRA_SERIAL
  #define TX_BUFFER_SIZE 256
  #define EMERGENCY_PARSER
#else
  #define TX_BUFFER_SIZE 0
  #undef EMERGENCY_PARSER
#endif

#define AUTO_TIME_USED_INTERVAL         1   //seconds
#define AUTO_SAVE_SETTING_INTERVAL      600 //seconds
#ifdef REG_SN
  #ifndef TOTAL_TIME_LIMIT
    #define TOTAL_TIME_LIMIT            300 //seconds = 5mins For common
  #endif //TOTAL_TIME_LIMIT
#endif

#ifdef POWER_MANAGEMENT
  #ifndef NOT_AUTO_SHUTDOWN
    #define AUTO_SHUTDOWN
  #endif
  #ifdef AUTO_SHUTDOWN
    #ifdef AUTO_SHUTDOWN_DEBUG
      #define AUTO_SHUTDONW_TIME_DISPALY  5     //seconds
      #define AUTO_SHUTDOWN_TIME_IDLE     10    //seconds
      #define AUTO_SHUTDOWN_TIME_HEATING  30    //seconds
      #ifdef FILAMENT_DETECT
        #define FILAMENT_WAIT_TIME        60    //seconds = 1min
      #endif
    #else //!AUTO_SHUTDOWN_DEBUG
      #define AUTO_SHUTDONW_TIME_DISPALY  60    //seconds = 1mins
      #define AUTO_SHUTDOWN_TIME_IDLE     600   //seconds = 10mins
      #define AUTO_SHUTDOWN_TIME_HEATING  1200  //seconds = 20mins
      #ifdef FILAMENT_DETECT
        #define FILAMENT_WAIT_TIME        1200  //seconds = 20mins
      #endif
    #endif //AUTO_SHUTDOWN_DEBUG
  #endif  //AUTO_SHUTDOWN
#endif //POWER_MANAGEMENT

#ifdef QUICK_PAUSE
  #ifdef NEAR_FEED
    #if MODEL == F160
      #define PAUSE_STOP_X      (X_MAX_POS - 10)
      #define PAUSE_STOP_Y      (Y_MAX_POS - 10)
    #elif MODEL == F430 || MODEL == F430Plus || MODEL == PEEK300
      #define PAUSE_STOP_X      (X_MAX_POS/2)
      #define PAUSE_STOP_Y      (220)
    #elif MODEL == D600 || MODEL == D600SE || MODEL == D600Pro
      #define PAUSE_STOP_X      (X_MAX_POS/2)
      #define PAUSE_STOP_Y      (200)
    #elif MODEL == F1000
      #define PAUSE_STOP_X      (X_MAX_POS/2)
      #define PAUSE_STOP_Y      (200)
    #else
      #define PAUSE_STOP_X      (X_MAX_POS - 10)
      #define PAUSE_STOP_Y      (Y_MAX_POS - 10)
    #endif
    #define PAUSE_LIFT_HEIGH_INIT           1
    #define PAUSE_RETRACTION_LENGTH_INIT    2
    #define PAUSE_LIFT_HEIGH                10
    #define PAUSE_RETRACTION_LENGTH         6
  #else
    #define PAUSE_STOP_X      3
    #define PAUSE_STOP_Y      3
    #define PAUSE_LIFT_HEIGH_INIT           1
    #define PAUSE_RETRACTION_LENGTH_INIT    5
    #define PAUSE_LIFT_HEIGH                10
    #define PAUSE_RETRACTION_LENGTH         15
  #endif
  #define PAUSE_REUSE_SPEED_LIFT            50    // (mm/s)
  #define PAUSE_REUSE_SPEED_MOVE            70    // (mm/s)
  #define REUSE_CLOSE_Z                     0.1
  #define REUSE_CLOSE_E                     1
  #define REUSE_SPEED_CLOSE                 1     // (mm/s)

  #ifdef ACCIDENT_DETECT
    #define ACCIDENT_Z_DOWN             2
    #ifdef NEAR_FEED
      #define ACCIDENT_E_RETRACTION     6
    #else
      #define ACCIDENT_E_RETRACTION     15  // Maybe it should be greater than 10
    #endif
    #define ACCIDENT_SPEED_TEST         1   // (mm/s)
  #endif //ACCIDENT_DETECT
#endif //QUICK_PAUSE

#define FILAMENT_UNLOAD_SPEED               50  // (mm/s)
#define FILAMENT_UNLOAD_EXTRUDER_LENGTH     10  // (mm)
#ifdef NEAR_FEED
  #define FILAMENT_UNLOAD_LENGTH            20
#else
  #define FILAMENT_UNLOAD_LENGTH            150
#endif

#ifdef FILAMENT_DETECT
  #define FILAMENT_DETECT_AGAIN
  #ifdef FILAMENT_DETECT_AGAIN
    #define FILAMENT_ERROR_BLOCK            500 //ms
  #endif
#endif

#ifdef MY_KEYPAD
  #define MY_KEYPAD_Z_MOVE_SCALE  5
#endif //MY_KEYPAD

#ifdef DWIN_LCD
  #define DWIN_Z_MOVE_SCALE       50
#endif //DWIN_LCD


#ifdef HAS_AIR_FAN
  #define DEFAULT_AIR_FAN_SPEED 0
#endif

#define MOTHERBOARD BOARD_CREATBOT

#undef SHORT_BUILD_VERSION
#undef DETAILED_BUILD_VERSION
#undef STRING_DISTRIBUTION_DATE
#undef PROTOCOL_VERSION
#undef MACHINE_NAME
#undef SOURCE_CODE_URL
#undef DEFAULT_MACHINE_UUID
#undef WEBSITE_URL

#define SHORT_BUILD_VERSION       STRING_VERSION
#define DETAILED_BUILD_VERSION    SHORT_BUILD_VERSION " (LYN)"
#define STRING_DISTRIBUTION_DATE  __DATE__
#define PROTOCOL_VERSION          "1.0"
#define MACHINE_NAME              MachineName
#define SOURCE_CODE_URL           "http://www.CreatBot.com/en/support.html"
#define DEFAULT_MACHINE_UUID      "00000000-0000-0000-0000-000000000000"
#define WEBSITE_URL               "www.CreatBot.com"

#endif //_CONFIG_H
