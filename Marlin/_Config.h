#ifndef CREATBOT_CONFIG_H
#define CREATBOT_CONFIG_H

#define MAKE_BEGIN

#define OEM CreatBot
#define MODEL F430
#define EXTRUDERS 2
#define LANGUAGE en
#define KTC
//#define NTC

#define MAKE_BATCH
#ifdef MAKE_BATCH
//  #define MAKE_LCD                  2         // 0 is 2004, 1 is 12864, 2 is 480272
//  #define MAKE_SYSTEM               1         // 0 is Normal, 1 is CoreXY, 2 is Linear guide, 3 is HXY
//  #define MAKE_FILE_TYPE            1         // 0 is SD Card, 1 is USB Disk
//  #define MAKE_HOTEND_SERIES        0         // 0 is D_SERIES, 1 is F_SERIES
//  #define MAKE_NEAR_FEED
//  #define MAKE_Z_1604
//  #define MAKE_D_SERIES_USE_175
//  #define MAKE_MY_KEYPAD
//  #define MAKE_POWER_MANAGEMENT
//  #define MAKE_NOT_AUTO_SHUTDOWN
//  #define MAKE_HOTWIND
//  #define MAKE_AUTO_LEVELING
//  #define MAKE_WIFI_SUPPORT
//  #define MAKE_Y_DUAL_STEPPER
//  #define MAKE_F_SERIES_ALLOW_500
//  #define MAKE_F_SERIES_BOARD_VERSION_LT_V95

  #undef MAKE_BEGIN
#else // not MAKE_BATCH

/******屏幕配置******/
//  #define LCD2004
//  #define LCD12864
  #define LCD480272

/******运动架构******/
//  #define COREXY
  #define LINEAR_GUIDE
//  #define HXY

/******存储支持******/
//  #define SDSUPPORT
  #define UDISKSUPPORT

/******喷头系列******/
  #define D_SERIES
//  #define F_SERIES

//  #define NEAR_FEED                 //近端送丝
//  #define Z_1604                    //丝杆参数
//  #define MY_KEYPAD                 //键盘支持
  #define QUICK_PAUSE               //快速暂停
  #define FILAMENT_CHANGE           //更换耗材
  #define FILAMENT_DETECT           //耗材检测
  #define POWER_MANAGEMENT          //电源管理
  #define ACCIDENT_DETECT           //断电续打
//  #define HAS_AIR_FAN               //过滤风扇
//  #define HOTWIND_SYSTEM            //热风系统
//  #define AUTO_BED_LEVELING         //自动调平
//  #define WIFI_SUPPORT              //支持WIFI
//  #define COLOR_LED                 //灯光支持
//  #define Y_DUAL_STEPPER_DRIVERS    //电机双Y
//  #define MIXING_EXTRUDER						//混合喷头

  #ifdef D_SERIES
//    #define D_SERIES_USE_175        //送丝机1.75
  #elif defined(F_SERIES)
//    #define F_SERIES_ALLOW_500      //温度上限500度
  #endif
//  #define NOT_AUTO_SHUTDOWN         //禁用自动关机

#endif // MAKE_BATCH

#include "_ModelInfo.h"
/************************************************************************************/


/*********************************** DEBUG OPTION ***********************************/
//#define BAUDRATE 115200

//#define REG_SN 000.00

//#define TEMP_ADJUST
//#define DEBUG_FREE

#define BL_TOUCH_SIGNAL_SELF_FILTER

#ifdef QUICK_PAUSE
//  #define DEBUG_CMD
#endif


#ifndef NOT_AUTO_SHUTDOWN
//  #define AUTO_SHUTDOWN_DEBUG
#endif

#ifdef DWIN_LCD
//  #define DWIN_LCD_DEDUG
//  #define DWIN_SERIAL_USE_BUILT_IN
//  #define DWIN_HEX_OPERATE_USE_STR
#endif

#ifdef UDISKSUPPORT
//  #define UDISK_DEBUG
//  #define UDisk_IMPL_NOT_PNP
#endif

#ifdef WIFI_SUPPORT
//  #define WIFI_IMPL_DEBUG
//  #define WIFI_DEBUG
  #ifdef WIFI_IMPL_DEBUG
    #undef WIFI_DEBUG
  #endif
  #ifdef WIFI_DEBUG
    #define STATE_ACTION_STRING_LEN 24
  #endif
#endif

#ifdef REG_SN
  #define TOTAL_TIME_LIMIT  720000 // seconds 200hour For Part-Payment
//	#define REG_USE_HARDWARE
  #ifdef REG_USE_HARDWARE
    #define REG_PLULIC_KEY  0xFFFFFFFF  // 公钥
  #endif
#endif

#ifdef HOTWIND_SYSTEM
  #define HOTWIND_HYSTERESIS  2 //度
#endif
/************************************************************************************/

#define STRING_VERSION "5.6"

#if defined(F_SERIES) && not defined(F_SERIES_BOARD_VERSION_LT_V95)
  #define BOARD_VERSION_GT_V95     //F系列黄主板（主板版本大于9.5）
#endif


#ifdef KTC
  #define T_SENSENR   -1    // KTC
  #define HEATER_0_MAXTEMP 415
  #define HEATER_1_MAXTEMP 415
  #define HEATER_2_MAXTEMP 415
  #define HEATER_3_MAXTEMP 415
#elif defined(NTC)
  #define T_SENSENR   1     // NTC
  #define HEATER_0_MAXTEMP 265
  #define HEATER_1_MAXTEMP 265
  #define HEATER_2_MAXTEMP 265
  #define HEATER_3_MAXTEMP 265
#endif
#define BED_MAXTEMP 135

#define PREHEAT_TEMP_HOTEND   210
#define PREHEAT_TEMP_BED      45
#define PREHEAT_FAN_SPEED     0

#ifdef F_SERIES
  #undef HEATER_0_MAXTEMP
  #undef HEATER_1_MAXTEMP
  #undef HEATER_2_MAXTEMP
  #undef HEATER_3_MAXTEMP
  #undef BED_MAXTEMP
  #ifdef F_SERIES_ALLOW_500
    #define HEATER_0_MAXTEMP  515
    #define HEATER_1_MAXTEMP  515
    #define HEATER_2_MAXTEMP  515
    #define HEATER_3_MAXTEMP  515
    #define BED_MAXTEMP       215

    #undef PREHEAT_TEMP_HOTEND
    #undef PREHEAT_TEMP_BED
    #define PREHEAT_TEMP_HOTEND   400
    #define PREHEAT_TEMP_BED      120
  #else
    #define HEATER_0_MAXTEMP  465
    #define HEATER_1_MAXTEMP  465
    #define HEATER_2_MAXTEMP  465
    #define HEATER_3_MAXTEMP  465
    #define BED_MAXTEMP       215
  #endif
#endif

#ifdef HOTWIND_SYSTEM
  #define TEMP_SENSOR_CHAMBER     1     // NTC
  #define CHAMBER_MINTEMP         -12
  #define CHAMBER_MAXTEMP         85
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


#ifdef AUTO_BED_LEVELING
  #define AUTO_BED_LEVELING_BILINEAR
  #define BLTOUCH

  #define NUM_SERVOS 1
  #define Z_ENDSTOP_SERVO_NR 0
  #define DEACTIVATE_SERVOS_AFTER_MOVE

  #define Z_SAFE_HOMING        // safe homing to prevent servo cann't probe the bed.

  #if MODEL == DE || MODEL == DE_Plus
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
  #elif MODEL == D600 || MODEL == D600_SE || MODEL == D600_Pro
    #ifdef BLTOUCH
      #define PROBE_XY_NUM 5
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
    #define PROBE_XY_NUM 5
    #define X_PROBE_OFFSET_FROM_EXTRUDER -21
    #define Y_PROBE_OFFSET_FROM_EXTRUDER 22
    #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
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
      #define X_PROBE_OFFSET_FROM_EXTRUDER 22
      #define Y_PROBE_OFFSET_FROM_EXTRUDER 41
      #define Z_PROBE_OFFSET_FROM_EXTRUDER -1
    #else
      #error "Not BL_TOUCH??"
    #endif
  #elif MODEL == F1000
    #define PROBE_XY_NUM 10
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

  #define LEFT_PROBE_BED_POSITION   (max(X_MIN_POS, X_MIN_POS + X_PROBE_OFFSET_FROM_EXTRUDER) + 5)
  #define RIGHT_PROBE_BED_POSITION  (min(X_MAX_POS, X_MAX_POS + X_PROBE_OFFSET_FROM_EXTRUDER) - 5)
  #define FRONT_PROBE_BED_POSITION  (max(Y_MIN_POS, Y_MIN_POS + Y_PROBE_OFFSET_FROM_EXTRUDER) + 5)
  #define BACK_PROBE_BED_POSITION   (min(Y_MAX_POS, Y_MAX_POS + Y_PROBE_OFFSET_FROM_EXTRUDER) - 5)
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


#ifdef Z_1605
  #if MODEL == F1000
    #define Z_STEP_VALUE 1600
  #else
    #define Z_STEP_VALUE 640
  #endif
#else
  #if MODEL == F1000
    #define Z_STEP_VALUE 2000
  #else
    #define Z_STEP_VALUE 800
  #endif
#endif

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

#ifdef LINEAR_GUIDE
  #if MODEL == D600 || MODEL == D600_SE || MODEL == D600_Pro || MODEL == F1000
    #define DEFAULT_AXIS_STEPS_PER_UNIT   {800.0/3, 800.0/3, Z_STEP_VALUE, E_STEP_VALUE }
  #elif MODEL == Other_MODEL
    #define DEFAULT_AXIS_STEPS_PER_UNIT   {128.0/3, 128.0/3, Z_STEP_VALUE, E_STEP_VALUE }
  #elif MODEL == F430 || MODEL == F260 || MODEL == F200 || MODEL == F220
    #ifdef MAKE_Y_DUAL_STEPPER
      #define DEFAULT_AXIS_STEPS_PER_UNIT   {78.7402, 52.4934, Z_STEP_VALUE, E_STEP_VALUE }
    #else
      #define DEFAULT_AXIS_STEPS_PER_UNIT   {78.7402, 131.2336, Z_STEP_VALUE, E_STEP_VALUE }
    #endif
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

#define LCD_LANGUAGE LANGUAGE

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
      #define AUTO_SHUTDONW_TIME_DISPALY  180   //seconds = 3mins
      #define AUTO_SHUTDOWN_TIME_IDLE     300   //seconds = 5mins
      #define AUTO_SHUTDOWN_TIME_HEATING  1800  //seconds = 30mins
      #ifdef FILAMENT_DETECT
        #define FILAMENT_WAIT_TIME        3600  //seconds = 2hours
      #endif
    #endif //AUTO_SHUTDOWN_DEBUG
  #endif  //AUTO_SHUTDOWN
#endif //POWER_MANAGEMENT

#ifdef QUICK_PAUSE
  #ifdef NEAR_FEED
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
    #define FILAMENT_ERROR_BLOCK            50 //ms
  #endif
#endif

#ifdef MY_KEYPAD
  #define MY_KEYPAD_Z_MOVE_SCALE  5
#endif //MY_KEYPAD

#ifdef DWIN_LCD
  #define DWIN_Z_MOVE_SCALE       5
#endif //DWIN_LCD


#ifdef HAS_AIR_FAN
  #define DEFAULT_AIR_FAN_SPEED 255
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

#endif //CREATBOT_CONFIG_H
