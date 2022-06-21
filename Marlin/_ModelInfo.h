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
//============================ Printer Functions ============================
//===========================================================================
#if MODEL == DM_Mini || MODEL == DM || MODEL == DM_Plus
  #define D_SERIES
  #define LCD12864
  #define HAS_SD_READER
  #define BOWDEN
  // #define POWER_MANAGEMENT
#elif MODEL == DX || MODEL == DX_Plus
  #define D_SERIES
  #define LCD12864
  #define HAS_SD_READER
  #define BOWDEN
  #define MY_KEYPAD
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
#elif MODEL == DE || MODEL == DE_Plus
  #define D_SERIES
  #define LCD12864
  #define COREXY
  #define HAS_SD_READER
  #define BOWDEN
  #define MY_KEYPAD
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
#elif MODEL == D600
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
#elif MODEL == D600_Mini
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define COREXY
  #define HAS_SD_READER
  #define BOWDEN
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
#elif MODEL == D600_SE
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
  #define HAS_CHAMBER_HEATER
  #define AUTO_BED_LEVELING
#elif MODEL == D600_Pro
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
  #define HAS_CHAMBER_HEATER
  #define AUTO_BED_LEVELING
#elif MODEL == F300
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define COREXY
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
#elif MODEL == F160
  #define F_SERIES
  #define LCD480272
  #define COREXY
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
  // #define WIFI_SUPPORT
#elif MODEL == F200
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
  #define HAS_CHAMBER_HEATER
  // #define AUTO_BED_LEVELING
#elif MODEL == F220
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
  #define HAS_CHAMBER_HEATER
  // #define AUTO_BED_LEVELING
#elif MODEL == F260
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
  #define HAS_CHAMBER_HEATER
  #define AUTO_BED_LEVELING
#elif MODEL == F430
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
  #define HAS_CHAMBER_HEATER
  #define AUTO_BED_LEVELING
  // #define COLOR_LED
#elif MODEL == PEEK300
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define HXY
  #define HAS_USB_READER
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
  #define HAS_CHAMBER_HEATER
  #define AUTO_BED_LEVELING
  // #define COLOR_LED
#elif MODEL == F1000
  #define Z_1605
  #define F_SERIES
  #define LCD480272
  #define LINEAR_GUIDE
  #define HAS_USB_READER
  // #define ULTRA_SERIAL
  #define POWER_MANAGEMENT
  #define ACCIDENT_DETECT
  #define HAS_CHAMBER_FAN
  #define HAS_CHAMBER_HEATER
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
