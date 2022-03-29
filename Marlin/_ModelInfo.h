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
#if MODEL == DM_Mini || MODEL == DM || MODEL == DM_Plus
  #if MODEL == DM_Mini                      // DM_Mini
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
    #else                                   // DM_Plus
      #ifdef KTC
        #define Z_MAX_POS 450
      #else
        #define Z_MAX_POS 455
      #endif
    #endif
  #endif
#elif MODEL == DX || MODEL == DX_Plus
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
  #else                                     // DX_Plus
    #ifdef KTC
      #define Z_MAX_POS 520
    #else
      #define Z_MAX_POS 525
    #endif
  #endif
#elif MODEL == DE || MODEL == DE_Plus
  #if EXTRUDERS == 3
    #define X_MAX_POS 385
  #else
    #define X_MAX_POS 405
  #endif
  #define Y_MAX_POS 305
  #if MODEL == DE                           // DE
    #define Z_MAX_POS 300
  #else                                     // DE_Plus
    #define Z_MAX_POS 520
  #endif
#elif MODEL == D600  || MODEL == D600_Pro   // D600
  #if EXTRUDERS == 3
    #define X_MAX_POS 580
  #else
    #define X_MAX_POS 600
  #endif
  #define Y_MAX_POS 600
  #define Z_MAX_POS 600
#elif MODEL == D600_Mini                    // D600_Mini
  #define X_MAX_POS 600
  #define Y_MAX_POS 600
  #define Z_MAX_POS 300
#elif MODEL == D600_SE                      // D600_SE
  #define X_MAX_POS 600
  #define Y_MAX_POS 400
  #define Z_MAX_POS 400
#elif MODEL == F300                         // F300
  #define X_MAX_POS 300
  #define Y_MAX_POS 300
  #define Z_MAX_POS 400
#elif MODEL == F160                         // F160
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
#elif MODEL == PEEK300                      // PEEK300
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


#ifdef MAKE_BATCH
  /********************************** Function ****************************************/
  #if MODEL == DM_Mini || MODEL == DM || MODEL == DM_Plus
    #define D_SERIES
    #define LCD12864
    #define SDSUPPORT
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
  //  #define POWER_MANAGEMENT
  #elif MODEL == DX || MODEL == DX_Plus
    #define D_SERIES
    #define LCD12864
    #define SDSUPPORT
    #define MY_KEYPAD
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
  #elif MODEL == DE || MODEL == DE_Plus
    #define D_SERIES
    #define LCD12864
    #define COREXY
    #define SDSUPPORT
    #define MY_KEYPAD
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
  #elif MODEL == D600
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define LINEAR_GUIDE
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
  #elif MODEL == D600_Mini
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define COREXY
    #define SDSUPPORT
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
  #elif MODEL == D600_SE
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define LINEAR_GUIDE
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
    #define HOTWIND_SYSTEM
    #define AUTO_BED_LEVELING
  #elif MODEL == D600_Pro
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define LINEAR_GUIDE
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
    #define HOTWIND_SYSTEM
    #define AUTO_BED_LEVELING
  #elif MODEL == F300
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define COREXY
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
  #elif MODEL == F160
    #define F_SERIES
    #define LCD480272
    #define COREXY
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
  //  #define WIFI_SUPPORT
  #elif MODEL == F200
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define LINEAR_GUIDE
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
    #define HOTWIND_SYSTEM
//    #define AUTO_BED_LEVELING
  #elif MODEL == F220
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define LINEAR_GUIDE
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
    #define HOTWIND_SYSTEM
//    #define AUTO_BED_LEVELING
  #elif MODEL == F260
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define LINEAR_GUIDE
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
    #define HOTWIND_SYSTEM
    #define AUTO_BED_LEVELING
  #elif MODEL == F430
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define LINEAR_GUIDE
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
    #define HOTWIND_SYSTEM
    #define AUTO_BED_LEVELING
  //  #define COLOR_LED
  #elif MODEL == PEEK300
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define HXY
    #define UDISKSUPPORT
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
    #define HOTWIND_SYSTEM
    #define AUTO_BED_LEVELING
  //  #define COLOR_LED
  #elif MODEL == F1000
    #define Z_1605
    #define F_SERIES
    #define LCD480272
    #define LINEAR_GUIDE
    #define UDISKSUPPORT
//    #define ULTRA_SERIAL
    #define NEAR_FEED
    #define QUICK_PAUSE
    #define FILAMENT_CHANGE
    #define FILAMENT_DETECT
    #define POWER_MANAGEMENT
    #define ACCIDENT_DETECT
    #define HAS_AIR_FAN
    #define HOTWIND_SYSTEM
    #define AUTO_BED_LEVELING
//    #define COLOR_LED
  #else
    #error "Unknown config info."
  #endif


  /********************************* Special Parts *************************************/
  #ifdef MAKE_LCD
    #ifdef LCD2004
      #undef LCD2004
    #endif
    #ifdef LCD12864
      #undef LCD12864
    #endif
    #ifdef LCD480272
      #undef LCD480272
    #endif

    #if MAKE_LCD == 0
      #define LCD2004
    #elif MAKE_LCD == 1
      #define LCD12864
    #elif MAKE_LCD == 2
      #define LCD480272
    #else
      #error "New LCD ???"
    #endif
  #endif // MAKE_LCD

  #ifdef MAKE_SYSTEM
    #ifdef COREXY
      #undef COREXY
    #endif
    #ifdef LINEAR_GUIDE
      #undef LINEAR_GUIDE
    #endif
    #ifdef HXY
      #undef HXY
    #endif

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
    #ifdef SDSUPPORT
      #undef SDSUPPORT
    #endif
    #ifdef UDISKSUPPORT
      #undef UDISKSUPPORT
    #endif

    #if MAKE_FILE_TYPE == 0
      #define SDSUPPORT
    #elif MAKE_FILE_TYPE == 1
      #define UDISKSUPPORT
    #else
      #error "New file system ???"
    #endif
  #endif

  #ifdef MAKE_HOTEND_SERIES
    #ifdef D_SERIES
      #undef D_SERIES
    #endif
    #ifdef F_SERIES
      #undef F_SERIES
    #endif

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

  #if defined(MAKE_Z_1604) && defined(Z_1605)
    #undef Z_1605
  #endif

  #if defined(MAKE_D_SERIES_USE_175) && defined(D_SERIES)
    #define D_SERIES_USE_175
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

  #if defined(MAKE_F_SERIES_ALLOW_500) && defined(F_SERIES)
    #define F_SERIES_ALLOW_500
  #endif

  #if defined(MAKE_F_SERIES_BOARD_VERSION_LT_V95) && defined(F_SERIES)
    #define F_SERIES_BOARD_VERSION_LT_V95
  #endif

#endif // MAKE_BATCH

#ifdef LCD12864
  #define LCD_SCREEN_ROT_180
  #define REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER
#endif
#ifdef LCD2004
  #define REPRAP_DISCOUNT_SMART_CONTROLLER
#endif
#ifdef LCD480272
  #define DWIN_LCD
  #ifdef MY_KEYPAD
    #undef MY_KEYPAD
  #endif
#endif

#endif /* MARLIN__MODELINFO_H_ */
