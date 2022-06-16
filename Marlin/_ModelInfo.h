/*
 * _ModelInfo.h
 *
 *  Created on: 2017年9月26日
 *      Author: CreatBot-SW
 */

#ifndef MARLIN__MODELINFO_H_
#define MARLIN__MODELINFO_H_

#include "_OEMInfo.h"

#if defined(BOARD_VERSION_GT_V95) && not defined(F_SERIES)
  #error "BOARD_VERSION_GT_V95 is only for F-Series printers."
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
    #ifdef KTC
      #define Z_MAX_POS 250
    #else // NTC
      #define Z_MAX_POS 255
    #endif
  #else
    #define X_MAX_POS 255
    #define Y_MAX_POS 255
    #if MODEL == DM
      #ifdef KTC
        #define Z_MAX_POS 300
      #else // NTC
        #define Z_MAX_POS 305
      #endif
    #else // MODEL == DM_Plus
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
  #else // EXTRUDERS <= 2
    #define X_MAX_POS 305
  #endif
  #define Y_MAX_POS 255
  #if MODEL == DX
    #ifdef KTC
      #define Z_MAX_POS 300
    #else // NTC
      #define Z_MAX_POS 305
    #endif
  #else // MODEL == DX_Plus
    #ifdef KTC
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
//============================ Printer Functions ============================
//===========================================================================
#if MODEL == DM_Mini || MODEL == DM || MODEL == DM_Plus
  #define D_SERIES
  #define LCD12864
  #define SDSUPPORT
  #define BOWDEN
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  // #define POWER_MANAGEMENT
#elif MODEL == DX || MODEL == DX_Plus
  #define D_SERIES
  #define LCD12864
  #define SDSUPPORT
  #define BOWDEN
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
  #define BOWDEN
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
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
#elif MODEL == D600_Mini
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define COREXY
  #define SDSUPPORT
  #define BOWDEN
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
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
  #define HEATED_CHAMBER
  #define AUTO_BED_LEVELING
#elif MODEL == D600_Pro
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define UDISKSUPPORT
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
  #define HEATED_CHAMBER
  #define AUTO_BED_LEVELING
#elif MODEL == F300
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define COREXY
  #define UDISKSUPPORT
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
#elif MODEL == F160
  #define F_SERIES
  #define LCD480272
  #define COREXY
  #define UDISKSUPPORT
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
  // #define WIFI_SUPPORT
#elif MODEL == F200
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define UDISKSUPPORT
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
  #define HEATED_CHAMBER
  // #define AUTO_BED_LEVELING
#elif MODEL == F220
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define UDISKSUPPORT
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
  #define HEATED_CHAMBER
  // #define AUTO_BED_LEVELING
#elif MODEL == F260
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define UDISKSUPPORT
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
  #define HEATED_CHAMBER
  #define AUTO_BED_LEVELING
#elif MODEL == F430
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define UDISKSUPPORT
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
  #define HEATED_CHAMBER
  #define AUTO_BED_LEVELING
  // #define COLOR_LED
#elif MODEL == PEEK300
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define HXY
  #define UDISKSUPPORT
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
  #define HEATED_CHAMBER
  #define AUTO_BED_LEVELING
  // #define COLOR_LED
#elif MODEL == F1000
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define UDISKSUPPORT
  // #define ULTRA_SERIAL
  #define QUICK_PAUSE
  #define FILAMENT_CHANGE
  #define FILAMENT_DETECT
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define CHAMBER_FAN
  #define HEATED_CHAMBER
  #define AUTO_BED_LEVELING
  // #define COLOR_LED
#else
  #error "Unknown config info."
#endif

//===========================================================================
//=============================== LCD Config ================================
//===========================================================================
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
