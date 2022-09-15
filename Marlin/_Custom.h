/*
 * _Custom.h
 *
 *  Created on: 2017年11月10日
 *      Author: CreatBot-SW
 */

#ifndef MARLIN__CUSTOM_H_
#define MARLIN__CUSTOM_H_

#define STRING_VERSION "6.1"

/* ************MODEL************
 * DMMini, DM, DMPlus
 * DX, DXPlus
 * DE, DEPlus
 * D600, D600Mini, D600SE, D600Pro
 * F160, F160PEEK
 * F200, F220, F260, F300
 * F430, F430Plus
 * F1000
 * PEEK300
 *
 *
 * **********EXTRUDERS**********
 *  { 1, 2, 3 }
 *
 *
 * **********LANGUAGE***********
 *  { en, cn }
 */


#define MAKE_BEGIN

#define OEM CreatBot
#define MODEL PEEK300
#define EXTRUDERS 2
#define LANGUAGE en
#define KTC
//#define NTC

#define MAKE_BATCH
#ifdef MAKE_BATCH
//  #define MAKE_LCD                  2         // 0 is 2004, 1 is 12864, 2 is 480272, 3 is 800480, 4 is 1024768
//  #define MAKE_SYSTEM               1         // 0 is Normal, 1 is CoreXY, 2 is Linear guide, 3 is HXY
//  #define MAKE_FILE_TYPE            1         // 0 is SD Card, 1 is USB Disk
//  #define MAKE_HOTEND_SERIES        0         // 0 is D_SERIES, 1 is F_SERIES
//  #define MAKE_NEAR_FEED
//  #define MAKE_Z04
//  #define MAKE_Z05
//  #define MAKE_D_SERIES_USE_300
//  #define MAKE_MY_KEYPAD
//  #define MAKE_POWER_MANAGEMENT
//  #define MAKE_NOT_AUTO_SHUTDOWN
//  #define MAKE_HOTWIND
//  #define MAKE_AUTO_LEVELING
//  #define MAKE_WIFI_SUPPORT
//  #define MAKE_Y_DUAL_STEPPER
//  #define MAKE_BOARD_PIN_EXCHANGE
//  #define MAKE_NOT_SIFU

  #undef MAKE_BEGIN
#else // not MAKE_BATCH

/******屏幕配置******/
//  #define LCD2004
//  #define LCD12864
  #define LCD480272
//  #define LCD800480
//  #define LCD1024768

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
//  #define Z04                       //丝杆参数
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
//  #define MIXING_EXTRUDER           //混合喷头

  #ifdef D_SERIES
//    #define D_SERIES_USE_300       //送丝机3.0
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
  #define DWIN_USE_OS
  #define LCD_FILE_CHAR_MAXIMIZE
//  #define DWIN_SERIAL_USE_BUILT_IN
//  #define DWIN_HEX_OPERATE_USE_STR
  #define DWIN_LCD_USE_T5_CPU
#endif

#ifdef UDISKSUPPORT
//  #define UDISK_DEBUG
//  #define UDisk_IMPL_NOT_PNP
  #define FILE_UNICODE_SUPPORT
#endif

#ifdef WIFI_SUPPORT
//  #define WIFI_IMPL_DEBUG
//  #define WIFI_DEBUG
//  #define WIFI_MODULE_ESP8266_01
  #define WIFI_MODULE_ESP8266_12
  #ifdef WIFI_IMPL_DEBUG
    #undef WIFI_DEBUG
  #endif
  #ifdef WIFI_DEBUG
    #define STATE_ACTION_STRING_LEN 24
  #endif
#endif

#ifdef REG_SN
  #define TOTAL_TIME_LIMIT  720000  // seconds 200hour For Part-Payment
  #define REG_USE_HARDWARE
  #ifdef REG_USE_HARDWARE
    #define REG_PLULIC_KEY  0xF0F0F0F0  // 公钥
  #endif
#endif

/************************************************************************************/


#endif /* MARLIN__CUSTOM_H_ */
