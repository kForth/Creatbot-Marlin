/*
 * _MakeBatch.h
 *
 *  Created on: 2017年9月26日
 *      Author: CreatBot-SW
 */

#ifndef MARLIN__MODELINFO_H_
#define MARLIN__MODELINFO_H_

#include "_OEMInfo.h"

/********************************** Molding Size ************************************/
#define X_MIN_POS 0
#define Y_MIN_POS 0
#define Z_MIN_POS 0
#if MODEL == DMMini || MODEL == DM || MODEL == DMPlus
  #if MODEL == DMMini                      // DMMini
    #define X_MAX_POS 205
    #define Y_MAX_POS 205
    #ifdef KTC
      #define Z_MAX_POS 250
    #else
      #define Z_MAX_POS 255
    #endif
  #else
    #define X_MAX_POS 255
    #define Y_MAX_POS 255
    #if MODEL == DM                         // DM
      #ifdef KTC
        #define Z_MAX_POS 300
      #else
        #define Z_MAX_POS 305
      #endif
    #else                                   // DMPlus
      #ifdef KTC
        #define Z_MAX_POS 450
      #else
        #define Z_MAX_POS 455
      #endif
    #endif
  #endif
#elif MODEL == DX || MODEL == DXPlus
  #if EXTRUDERS == 3
    #define X_MAX_POS 280
  #else
    #define X_MAX_POS 305
  #endif
  #define Y_MAX_POS 255
  #if MODEL == DX                           // DX
    #ifdef KTC
      #define Z_MAX_POS 300
    #else
      #define Z_MAX_POS 305
    #endif
  #else                                     // DXPlus
    #ifdef KTC
      #define Z_MAX_POS 520
    #else
      #define Z_MAX_POS 525
    #endif
  #endif
#elif MODEL == DE || MODEL == DEPlus
  #if EXTRUDERS == 3
    #define X_MAX_POS 385
  #else
    #define X_MAX_POS 405
  #endif
  #define Y_MAX_POS 305
  #if MODEL == DE                           // DE
    #define Z_MAX_POS 300
  #else                                     // DEPlus
    #define Z_MAX_POS 520
  #endif
#elif MODEL == D600 || MODEL == D600Pro     // D600
  #if EXTRUDERS == 3
    #define X_MAX_POS 580
  #else
    #define X_MAX_POS 600
  #endif
  #define Y_MAX_POS 600
  #define Z_MAX_POS 600
#elif MODEL == D600Mini                    // D600Mini
  #define X_MAX_POS 600
  #define Y_MAX_POS 600
  #define Z_MAX_POS 300
#elif MODEL == D600SE                      // D600SE
  #define X_MAX_POS 600
  #define Y_MAX_POS 400
  #define Z_MAX_POS 400
#elif MODEL == F300                         // F300
  #define X_MAX_POS 300
  #define Y_MAX_POS 300
  #define Z_MAX_POS 400
#elif MODEL == F160 || MODEL == F160PEEK    // F160
  #define X_MAX_POS 160
  #define Y_MAX_POS 160
  #define Z_MAX_POS 200
#elif MODEL == F200                         // F200
  #define X_MAX_POS 200
  #define Y_MAX_POS 200
  #define Z_MAX_POS 300
#elif MODEL == F220                         // F220
  #define X_MAX_POS 220
  #define Y_MAX_POS 220
  #define Z_MAX_POS 260
#elif MODEL == F260                         // F260
  #define X_MAX_POS 260
  #define Y_MAX_POS 260
  #define Z_MAX_POS 300
#elif MODEL == F430                         // F430
  #define X_MAX_POS 400
  #define Y_MAX_POS 300
  #define Z_MAX_POS 300
#elif MODEL == F430Plus                     // F430Plus
  #define X_MAX_POS 400
  #define Y_MAX_POS 300
  #define Z_MAX_POS 500
#elif MODEL == PEEK300                      // PEEK300
  #define X_MAX_POS 300
  #define Y_MAX_POS 300
  #define Z_MAX_POS 400
#elif MODEL == F1000
  #define X_MAX_POS 1000
  #define Y_MAX_POS 1000
  #define Z_MAX_POS 1000
#else
  #error "Unknown size info."
#endif


#ifdef MAKE_BATCH
  /********************************** Function ****************************************/
  /*****基础配置*****/
  #define LCD480272               //屏幕配置
  #define UDISKSUPPORT            //闪存支持
  #define QUICK_PAUSE             //快速暂停
  #define FILAMENT_CHANGE         //更换耗材
  #define FILAMENT_DETECT         //耗材检测
  #define POWER_MANAGEMENT        //电源管理
  #define ACCIDENT_DETECT         //断电续打

  /*****差异配置*****/
  #if MODEL == DX || MODEL == DXPlus || MODEL == DE || MODEL == DEPlus
    #define D_SERIES              //一代送丝机 系列D
    #define T350                  //一代机温度最高350度
  #else
    #define F_SERIES              //二代送丝机 系列F
    #define T450                  //二代机温度最高450度
    #define NEAR_FEED             //近端送丝
    #define HAS_AIR_FAN           //过滤风扇
  #endif

  #if MODEL == DE || MODEL == DEPlus ||\
      MODEL == F160 || MODEL == F160PEEK || MODEL == F200 ||\
      MODEL == PEEK300
    #define COREXY                //CoreXY
  #elif MODEL == D600 || MODEL == D600SE || MODEL == D600Pro ||\
      MODEL == F430 || MODEL == F430Plus || MODEL == F260 ||\
      MODEL == F1000
    #define LINEAR_GUIDE          //线性导轨
  #endif

  #if MODEL == PEEK300
    #undef T450
    #define T500                  //最高500度
  #endif

  #if MODEL == D600 || MODEL == D600SE || MODEL == D600Pro
    #define Z05
    #define USE_SIFU_MOTOR
    #define USE_HEATING_TUBE_80W
    #if MODEL == D600SE || MODEL == D600Pro
      #define HOTWIND_SYSTEM
      #define AUTO_BED_LEVELING
    #endif
  #elif MODEL == F160PEEK
    #define HOTWIND_SYSTEM
  #elif MODEL == F430 || MODEL == F430Plus || MODEL == F260
    #if MODEL == F260
      #define Z05
    #endif
    #define HOTWIND_SYSTEM
    #define AUTO_BED_LEVELING
    #if MODEL == F430Plus
      #define SWITCHING_NOZZLE
    #endif
  #elif MODEL == PEEK300
    #define USE_HEATING_TUBE_80W
    #define HOTWIND_SYSTEM
    #define AUTO_BED_LEVELING
    #define SWITCHING_NOZZLE
    #define USE_CUSTOM_SERVO0
    #define SERVO_SPEED_SLOWLY
  #elif MODEL == F1000
    #undef LCD480272
    #define LCD1024768
    #define Z05
    #define USE_SIFU_MOTOR
    #define USE_HEATING_TUBE_80W
    #define HOTWIND_SYSTEM
    #define AUTO_BED_LEVELING
    #define SWITCHING_NOZZLE
    #define USE_CUSTOM_SERVO0
    #define SERVO_SPEED_SLOWLY
  #endif

  /*****停产机型*****/
  #if MODEL == DMMini || MODEL == DM || MODEL == DMPlus
    #error "Current version is incompatible with DM Series"
  #elif MODEL == D600Mini
    #error "Current version is incompatible with D600Mini"
  #elif MODEL == F300 || MODEL == F220
    #error "Current version is incompatible"
  #endif


  /********************************* Special Parts *************************************/
  #ifdef MAKE_LCD
      #undef LCD2004
      #undef LCD12864
      #undef LCD480272
      #undef LCD1024768

    #if MAKE_LCD == 0
      #define LCD2004
    #elif MAKE_LCD == 1
      #define LCD12864
    #elif MAKE_LCD == 2
      #define LCD480272
    #elif MAKE_LCD == 3
      #define LCD1024768
    #else
      #error "New LCD ???"
    #endif
  #endif // MAKE_LCD

  #ifdef MAKE_SYSTEM
      #undef COREXY
      #undef LINEAR_GUIDE
      #undef HXY

    #if MAKE_SYSTEM == 0
      //do nothing
    #elif MAKE_SYSTEM == 1
      #define COREXY
      #ifdef Y_DUAL_STEPPER_DRIVERS
        #undef Y_DUAL_STEPPER_DRIVERS
      #endif
    #elif MAKE_SYSTEM == 2
      #define LINEAR_GUIDE
    #elif MAKE_SYSTEM == 3
      #define HXY
    #else
      #error "New movement system ???"
    #endif
  #endif // MAKE_SYSTEM

  #ifdef MAKE_FILE_TYPE
      #undef SDSUPPORT
      #undef UDISKSUPPORT

    #if MAKE_FILE_TYPE == 0
      #define SDSUPPORT
    #elif MAKE_FILE_TYPE == 1
      #define UDISKSUPPORT
    #else
      #error "New file system ???"
    #endif
  #endif

  #ifdef MAKE_HOTEND_SERIES
      #undef D_SERIES
      #undef F_SERIES

    #if MAKE_HOTEND_SERIES == 0
      #define D_SERIES
    #elif MAKE_HOTEND_SERIES == 1
      #define F_SERIES
    #else
      #error "New hotend type ???"
    #endif
  #endif

  #if defined(MAKE_NEAR_FEED) && not defined(NEAR_FEED)
    #define NEAR_FEED
  #endif

  #if defined(MAKE_Z04) && defined(Z05)
    #undef Z05
  #endif

  #if defined(MAKE_Z05) && not defined(Z05)
    #define Z05
  #endif

  #if defined(MAKE_D_SERIES_USE_300) && defined(D_SERIES)
    #define D_SERIES_USE_300
  #endif

  #if defined(MAKE_MY_KEYPAD) && not defined(MY_KEYPAD)
    #define MY_KEYPAD
  #endif

  #if defined(MAKE_POWER_MANAGEMENT) && not defined(POWER_MANAGEMENT)
    #define POWER_MANAGEMENT
  #endif

  #ifdef MAKE_NOT_AUTO_SHUTDOWN
    #define NOT_AUTO_SHUTDOWN
  #endif

  #if defined(MAKE_HOTWIND) && not defined(HOTWIND_SYSTEM)
    #define HOTWIND_SYSTEM
  #endif

  #if defined(MAKE_AUTO_LEVELING) && not defined(AUTO_BED_LEVELING)
    #define AUTO_BED_LEVELING
  #endif

  #if defined(MAKE_WIFI_SUPPORT) && not defined(WIFI_SUPPORT)
    #define WIFI_SUPPORT
  #endif

  #ifdef MAKE_Y_DUAL_STEPPER
    #define Y_DUAL_STEPPER_DRIVERS
  #endif


  #if defined(MAKE_BOARD_PIN_EXCHANGE)
    #define BOARD_PIN_EXCHANGE
  #endif

  #if defined(MAKE_NOT_SIFU) && defined(USE_SIFU_MOTOR)
    #undef USE_SIFU_MOTOR
  #endif

#endif // MAKE_BATCH

#ifdef LCD12864
  #define LCD_SCREEN_ROT_180
  #define REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER
#endif
#ifdef LCD2004
  #define REPRAP_DISCOUNT_SMART_CONTROLLER
#endif
#if defined(LCD480272) || defined(LCD1024768)
  #define DWIN_LCD
#endif

#endif /* MARLIN__MODELINFO_H_ */
