/*
  dwin_lcd.h - Hardware serial library for DWIN UART LCM.
  Copyright (c) 2015 CreatBot. All Right Reserved.

  Developer Lyn Lee, 3 June 2015.
*/

#ifndef DWIN_LCD_H
#define DWIN_LCD_H

#include "Marlin.h"

#ifdef DWIN_LCD

#include "dwin_lcd_implementation.h"

#define DWIN_VER                    "5.1.0"
//#define DWIN_CENTER_MSG           //T5L support align center in DGUS.
#define DWIN_FILENAME_USE_ICO


#define LCD_SETUP_FIRST             PSTR("0000")
#define LCD_SETUP_LOOP_STRAT        25    // "0019"
#define LCD_SETUP_LOOP_END          39    // "0027"
#define LCD_SETUP_NUM               40    // "0028"
#define LCD_SETUP_LOOP_TIMES        1     // must greater than 0


#if defined(LCD480272) || defined(LCD800480)
  #define LCD_SIZE_43
#elif defined(LCD1024768)
  #define LCD_SIZE_97
#else
  #error "Please defined the lcd's size."
#endif

#if defined(LCD_SIZE_43)
  #define LCD_FILE_ITEM_PER_PAGE    7
#elif defined(LCD_SIZE_97)
  #define LCD_FILE_ITEM_PER_PAGE    9
#endif
#ifdef LCD_FILE_CHAR_MAXIMIZE
  #define LCD_FILE_CHAR_LEN         (LONG_FILENAME_LENGTH - NULL_CHAR_LENGTH)
#else
  #define LCD_FILE_CHAR_LEN         30    // 30 char = 15 word
#endif
#define LCD_FILE_CHAR_LEN_WIFI      32    // 32 char = 16 word

#define LCD_MAX_STR_LEN             32
#define LCD_VER_CHAR_LEN            16    // 16 char = 8 word
#define LCD_UI_CHAR_LEN             16    // 16 char = 8 word
#define LCD_SN_CHAR_LEN             16    // 16 char = 8 word
#define LCD_MSG_CHAR_LEN            32    // 32 char = 16 word
#define LCD_HOST_CHAR_LEN           32    // 32 char = 16 word
#define LCD_WIFI_CHAR_LEN           30    // 30 char = 15 word
#define LCD_WIFI_KEY_LEN            32    // 32 char = 16 word
#define LCD_WIFI_SSID_LEN           32    // 32 char = 16 word
#define LCD_WIFI_UUID_LEN           16    // 16 char = 8 word
#define LCD_TIMEOUT_TO_STATUS       15000  // 15s
#define LCD_TIMEOUT_TO_LOCK         20000  // 20s

#if defined(LCD480272)
  #define PROGRESS_BAR_X0           0
  #define PROGRESS_BAR_Y0           134
  #define PROGRESS_BAR_X1           480
  #define PROGRESS_BAR_Y1           174
#elif defined(LCD800480)
  #define PROGRESS_BAR_X0           0
  #define PROGRESS_BAR_Y0           240
  #define PROGRESS_BAR_X1           800
  #define PROGRESS_BAR_Y1           310
#elif defined(LCD1024768)
  #define PROGRESS_BAR_X0           0
  #define PROGRESS_BAR_Y0           382
  #define PROGRESS_BAR_X1           1024
  #define PROGRESS_BAR_Y1           487
#endif
#define COLOR_PRIMARY               PSTR("EB65") //YELLOW
#define COLOR_ASSIST                PSTR("632C") //GRAY



// the addr of page that defined in DWIN UART LCM.
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_1             PSTR("0029")  //41
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_2             PSTR("002A")  //42
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_3             PSTR("002B")  //43
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_1_CHAMBER     PSTR("002C")  //44
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT_2_CHAMBER     PSTR("002D")  //45
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_1            PSTR("002E")  //46
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_2            PSTR("002F")  //47
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_3            PSTR("0030")  //48
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_1_CHAMBER    PSTR("0031")  //49
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT_2_CHAMBER    PSTR("0032")  //50
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_1             PSTR("0033")  //51
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_2             PSTR("0034")  //52
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_3             PSTR("0035")  //53
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_1_CHAMBER     PSTR("0036")  //54
#define PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE_2_CHAMBER     PSTR("0037")  //55
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_1            PSTR("0038")  //56
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_2            PSTR("0039")  //57
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_3            PSTR("003A")  //58
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_1_CHAMBER    PSTR("003B")  //59
#define PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE_2_CHAMBER    PSTR("003C")  //60
#define PAGE_DEFAULT_PRINT_PAUSE_1                            PSTR("003D")  //61
#define PAGE_DEFAULT_PRINT_PAUSE_2                            PSTR("003E")  //62
#define PAGE_DEFAULT_PRINT_PAUSE_3                            PSTR("003F")  //63
#define PAGE_DEFAULT_PRINT_PAUSE_1_CHAMBER                    PSTR("0040")  //64
#define PAGE_DEFAULT_PRINT_PAUSE_2_CHAMBER                    PSTR("0041")  //65
#define PAGE_DEFAULT_PRINT_RESUME_1                           PSTR("0042")  //66
#define PAGE_DEFAULT_PRINT_RESUME_2                           PSTR("0043")  //67
#define PAGE_DEFAULT_PRINT_RESUME_3                           PSTR("0044")  //68
#define PAGE_DEFAULT_PRINT_RESUME_1_CHAMBER                   PSTR("0045")  //69
#define PAGE_DEFAULT_PRINT_RESUME_2_CHAMBER                   PSTR("0046")  //70
#define PAGE_DEFAULT_ONLINE_1                                 PSTR("0047")  //71
#define PAGE_DEFAULT_ONLINE_2                                 PSTR("0048")  //72
#define PAGE_DEFAULT_ONLINE_3                                 PSTR("0049")  //73
#define PAGE_DEFAULT_ONLINE_1_CHAMBER                         PSTR("004A")  //74
#define PAGE_DEFAULT_ONLINE_2_CHAMBER                         PSTR("004B")  //75

#define PAGE_MOVE                                             PSTR("0051")  //81
#define PAGE_FILE_ONE                                         PSTR("0053")  //83
#define PAGE_FILE_NEXT                                        PSTR("0054")  //84
#define PAGE_FILE_LAST                                        PSTR("0055")  //85
#define PAGE_FILE_BOTH                                        PSTR("0056")  //86
#define PAGE_FILE_UP_ONE                                      PSTR("0057")  //87
#define PAGE_FILE_UP_NEXT                                     PSTR("0058")  //88
#define PAGE_FILE_UP_LAST                                     PSTR("0059")  //89
#define PAGE_FILE_UP_BOTH                                     PSTR("005A")  //90
#define PAGE_FILAMENT_1                                       PSTR("005C")  //92
#define PAGE_FILAMENT_2                                       PSTR("005D")  //93
#define PAGE_FILAMENT_3                                       PSTR("005E")  //94
#define PAGE_FILAMENT_1_CHAMBER                               PSTR("005C")  //92
#define PAGE_FILAMENT_2_CHAMBER                               PSTR("005D")  //93

#define PAGE_ADJUST                                           PSTR("0060")  //96
#define PAGE_SETTING                                          PSTR("0063")  //99
#define PAGE_SETTING_WIFI                                     PSTR("0064")  //100
#define PAGE_SETTING_LEVELING                                 PSTR("0065")  //101
#define PAGE_SETTING_WIFI_LEVELING                            PSTR("0066")  //102
#define PAGE_MOTION_SETTING                                   PSTR("0069")  //105
#define PAGE_FANSPEED_SETTING                                 PSTR("006A")  //106
#define PAGE_FANSPEED_SETTING_FILTER                          PSTR("006B")  //107
#define PAGE_PREHEAT_SETTING                                  PSTR("006C")  //108
#define PAGE_PREHEAT_SETTING_CHAMBER                          PSTR("006D")  //109
#define PAGE_WIFI_SETTING_OFF                                 PSTR("006E")  //110
#define PAGE_WIFI_SETTING_ON                                  PSTR("006F")  //111
#define PAGE_LEVELING_SETTING_OFF                             PSTR("0070")  //112
#define PAGE_LEVELING_SETTING_ON                              PSTR("0071")  //113
#define PAGE_LEVELING_SETTING_DISABLE                         PSTR("0072")  //114

#define PAGE_REG                                              PSTR("0074")  //116
#define PAGE_INFO_PRINTER                                     PSTR("0075")  //117
#define PAGE_WIFI_INFO_STATION                                PSTR("0077")  //119
#define PAGE_WIFI_INFO_AP                                     PSTR("0079")  //121
#define PAGE_PRINT_CONFIRM                                    PSTR("007B")  //123
#define PAGE_SHUTDOWN_HOTTEMP                                 PSTR("007D")  //125
#define PAGE_UNFINISH_CHOOSE                                  PSTR("007F")  //127

#define PAGE_CONNECT_WIFI_12                                  PSTR("0081")  //129
#define PAGE_CONNECT_WIFI_01                                  PSTR("0082")  //130


// the addr of varible that defined in DWIN UART LCM.
#ifdef DWIN_LCD_USE_T5_CPU
  #define EX0_TAR_ADDR                    PSTR("5000")  //2 bytes  (3.0)
  #define EX1_TAR_ADDR                    PSTR("5001")  //2 bytes  (3.0)
  #define EX2_TAR_ADDR                    PSTR("5002")  //2 bytes  (3.0)
  #define EX0_CUR_ADDR                    PSTR("5003")  //2 bytes  (3.0)
  #define EX1_CUR_ADDR                    PSTR("5004")  //2 bytes  (3.0)
  #define EX2_CUR_ADDR                    PSTR("5005")  //2 bytes  (3.0)
  #define BED_TAR_ADDR                    PSTR("5006")  //2 bytes  (3.0)
  #define BED_CUR_ADDR                    PSTR("5007")  //2 bytes  (3.0)
  #define CHAMBER_TAR_ADDR                PSTR("5008")  //2 bytes  (3.0)
  #define CHAMBER_CUR_ADDR                PSTR("5009")  //2 bytes  (3.0)
  #define SPEED_GLOBAL_ADDR               PSTR("500B")  //2 bytes  (3.0)
  #define FAN_SPEED_ADDR                  PSTR("500C")  //2 bytes  (3.0)
  #define EXT_MULTIPLY_ADDR               PSTR("500D")  //2 bytes  (3.0)
  #define PREHEAT_FAN_SPEED_ADDR          PSTR("500E")  //2 bytes  (3.0)
  #define PREHEAT_EXT_TEMP_ADDR_0         PSTR("500F")  //2 bytes  (3.0)
  #define PREHEAT_EXT_TEMP_ADDR_1         PSTR("501A")  //2 bytes  (3.0)
  #define PREHEAT_EXT_TEMP_ADDR_2         PSTR("501B")  //2 bytes  (3.0)
  #define PREHEAT_BED_TEMP_ADDR           PSTR("5010")  //2 bytes  (3.0)
  #define X_STEP_ADDR                     PSTR("5011")  //4 bytes  (3.4)
  #define Y_STEP_ADDR                     PSTR("5013")  //4 bytes  (3.4)
  #define Z_STEP_ADDR                     PSTR("5015")  //4 bytes  (4.3)
  #define E_STEP_ADDR                     PSTR("5017")  //4 bytes  (3.4)
  #define PREHEAT_CHAMBER_TEMP_ADDR       PSTR("5019")  //2 bytes  (3.0)

  #define MOVE_DISTANCE_INDEX_ADDR        PSTR("5020")  //2 bytes  (1.0)  (1,2,3,4 => 0.1mm, 1mm, 10mm, 100mm)
  #define FILAMENT_EXT_INDEX_ADDR         PSTR("5021")  //2 bytes  (1.0)  (1,2,3,4,5,6; 1=2=4 => Ext0; 3=5 => Ext1; 6 => Ext2)
  #define FILAMENT_EXT_DISTANCE_ADDR      PSTR("5022")  //2 bytes  (3.0)
  #define PERCENT_ADDR                    PSTR("5023")  //2 bytes  (3.2)
  #define PRINT_TIME_ADDR                 PSTR("5024")  //5 bytes  (HEX)  (ddd:hh:mm)
  #define USED_TIME_ADDR                  PSTR("5027")  //4 bytes  (HEX)  (hhhh:mm:ss)
  #define TRIAL_PERIOD_ADDR               PSTR("5029")  //4 bytes  (HEX)  (hhhh:mm:ss)
  #define REG_KEY_ADDR                    PSTR("502B")  //2 bytes  (3.2)
  #define REG_STATE_INDEX_ADDR            PSTR("502C")  //2 bytes  (1.0)  (0,1 => invalid, success)

  #define VER_ADDR                        PSTR("5030")  //16 bytes =  8 words
  #define UI_ADDR                         PSTR("5038")  //16 bytes =  8 words
  #define MSG_ADDR                        PSTR("5040")  //32 bytes = 16 words
  #define PAINT_TOOL1_ADDR                PSTR("5050")  //16 bytes =  8 words
  #define PAINT_TOOL2_ADDR                PSTR("5058")  //16 bytes =  8 words
  #define X_STR_ADDR                      PSTR("5060")  // 2 bytes =  1 word
  #define Y_STR_ADDR                      PSTR("5061")  // 2 bytes =  1 word
  #define Z_STR_ADDR                      PSTR("5062")  // 2 bytes =  1 word
  #define SN_CAHR_ADDR                    PSTR("5130")  //16 bytes =  8 words
  #define MSG_ADDR_SHORT                  PSTR("5140")  //32 bytes = 16 words

  #define CUR_MAX_TEMP_ADDR               PSTR("5070")  //2 bytes  (3.0)
  #define TEMP_FAN_SPEED_ADDR             PSTR("5071")  //2 bytes  (3.0)
  #define AIR_FAN_SPEED_ADDR              PSTR("5072")  //2 bytes  (3.0)
  #define X_AXIS_ADDR                     PSTR("5073")  //4 bytes  (4.2)
  #define Y_AXIS_ADDR                     PSTR("5075")  //4 bytes  (4.2)
  #define Z_AXIS_ADDR                     PSTR("5077")  //4 bytes  (4.2)
  #define SERVO_Z_OFFSET_ADDR             PSTR("5079")  //2 bytes  (2.2)
  #define FADE_HEIGHT_ADDR                PSTR("507A")  //2 bytes  (3.1)
  #define WIFI_MODE_INDEX_ADDR            PSTR("507B")  //2 bytes  (1.0)  (0,1 => STA, AP)
  #define WIFI_STATE_INDEX_ADDR           PSTR("507C")  //2 btyes  (1.0)  (0,1,2 => connected, connecting, no_connect)
  #define WIFI_SCAN_LAST_ICO_ADDR         PSTR("507D")  //2 bytes  (1.0)  (0,1 => HIDE,SHOW)
  #define WIFI_SCAN_NEXT_ICO_ADDR         PSTR("507E")  //2 bytes  (1.0)  (0,1 => HIDE,SHOW)

  #define WIFI_IP_ADDR                    PSTR("5080")  //32 bytes = 16 words
  #define WIFI_SSID_ADDR                  PSTR("5090")  //32 bytes = 16 words
  #define WIFI_KEY_ADDR                   PSTR("50A0")  //32 bytes = 16 words
  #define WIFI_KEY_MASK_ADDR              PSTR("50B0")  //16 bytes (****************)
  #define WIFI_UUID_ADDR                  PSTR("50B8")  //16 bytes (991234xxxxxxxxx)
  #define WIFI_SSID_SET_ADDR              PSTR("50C0")  //32 bytes = 16 words

  #define SET_EX0_TAR_ADDR                PSTR("5100")  //2 bytes  (3.0)
  #define SET_EX1_TAR_ADDR                PSTR("5101")  //2 bytes  (3.0)
  #define SET_EX2_TAR_ADDR                PSTR("5102")  //2 bytes  (3.0)
  #define SET_BED_TAR_ADDR                PSTR("5106")  //2 bytes  (3.0)
  #define SET_CHAMBER_TAR_ADDR            PSTR("5108")  //2 bytes  (3.0)
  #define SET_SPEED_GLOBAL_ADDR           PSTR("510B")  //2 bytes  (3.0)
  #define SET_FAN_SPEED_ADDR              PSTR("510C")  //2 bytes  (3.0)
  #define SET_EXT_MULTIPLY_ADDR           PSTR("510D")  //2 bytes  (3.0)
  #define SET_PREHEAT_FAN_SPEED_ADDR      PSTR("510E")  //2 bytes  (3.0)
  #define SET_PREHEAT_EXT_TEMP_ADDR_0     PSTR("510F")  //2 bytes  (3.0)
  #define SET_PREHEAT_EXT_TEMP_ADDR_1     PSTR("511A")  //2 bytes  (3.0)
  #define SET_PREHEAT_EXT_TEMP_ADDR_2     PSTR("511B")  //2 bytes  (3.0)
  #define SET_PREHEAT_BED_TEMP_ADDR       PSTR("5110")  //2 bytes  (3.0)
  #define SET_PREHEAT_CHAMBER_TEMP_ADDR   PSTR("5119")  //2 bytes  (3.0)


  #define SET_X_STEP_ADDR                 PSTR("5111")  //4 bytes  (3.4)
  #define SET_Y_STEP_ADDR                 PSTR("5113")  //4 bytes  (3.4)
  #define SET_Z_STEP_ADDR                 PSTR("5115")  //4 bytes  (4.3)
  #define SET_E_STEP_ADDR                 PSTR("5117")  //4 bytes  (3.4)

  #define SET_REG_KEY_ADDR                PSTR("512B")  //2 bytes  (3.2)

  #define SET_TEMP_FAN_SPEED_ADDR         PSTR("5171")  //2 bytes  (3.0)
  #define SET_AIR_FAN_SPEED_ADDR          PSTR("5172")  //2 bytes  (3.0)
  #define SET_SERVO_Z_OFFSET_ADDR         PSTR("5179")  //2 bytes  (2.2)
  #define SET_FADE_HEIGHT_ADDR            PSTR("517A")  //2 bytes  (3.1)

  // about file setting
#ifdef LCD_FILE_CHAR_MAXIMIZE
  #define FILE_ITEM_STR_0                 PSTR("6000")  //512 bytes = 256 words
  #define FILE_ITEM_STR_1                 PSTR("6100")  //512 bytes = 256 words
  #define FILE_ITEM_STR_2                 PSTR("6200")  //512 bytes = 256 words
  #define FILE_ITEM_STR_3                 PSTR("6300")  //512 bytes = 256 words
  #define FILE_ITEM_STR_4                 PSTR("6400")  //512 bytes = 256 words
  #define FILE_ITEM_STR_5                 PSTR("6500")  //512 bytes = 256 words
  #define FILE_ITEM_STR_6                 PSTR("6600")  //512 bytes = 256 words
  #define FILE_ITEM_STR_7                 PSTR("6700")  //512 bytes = 256 words
  #define FILE_ITEM_STR_8                 PSTR("6800")  //512 bytes = 256 words
  #define FILE_SELECT                     PSTR("6F00")  //512 bytes = 256 words
#else
  #define FILE_ITEM_STR_0                 PSTR("5300")  //30 bytes = 15 words
  #define FILE_ITEM_STR_1                 PSTR("5310")  //30 bytes = 15 words
  #define FILE_ITEM_STR_2                 PSTR("5320")  //30 bytes = 15 words
  #define FILE_ITEM_STR_3                 PSTR("5330")  //30 bytes = 15 words
  #define FILE_ITEM_STR_4                 PSTR("5340")  //30 bytes = 15 words
  #define FILE_ITEM_STR_5                 PSTR("5350")  //30 bytes = 15 words
  #define FILE_ITEM_STR_6                 PSTR("5360")  //30 bytes = 15 words
  #define FILE_ITEM_STR_7                 PSTR("5370")  //30 bytes = 15 words
  #define FILE_ITEM_STR_8                 PSTR("5380")  //30 bytes = 15 words
  #define FILE_SELECT                     PSTR("53F0")  //30 bytes = 15 words
#endif

  #define FILE_ITEM_ICO_0                 PSTR("530F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_1                 PSTR("531F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_2                 PSTR("532F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_3                 PSTR("533F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_4                 PSTR("534F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_5                 PSTR("535F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_6                 PSTR("536F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_7                 PSTR("537F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_8                 PSTR("538F")  //2 bytes (1.0)  (0,1,2)

  #ifdef LCD_SIZE_43
    #define FILE_ITEM_CENTER              FILE_ITEM_STR_3
  #elif defined(LCD_SIZE_97)
    #define FILE_ITEM_CENTER              FILE_ITEM_STR_4
  #endif


//  #define FILE_ITEM_0_ATTR                PSTR("5500")
//  #define FILE_ITEM_1_ATTR                PSTR("5510")
//  #define FILE_ITEM_2_ATTR                PSTR("5520")
//  #define FILE_ITEM_3_ATTR                PSTR("5530")
//  #define FILE_ITEM_4_ATTR                PSTR("5540")
//  #define FILE_ITEM_5_ATTR                PSTR("5550")
//  #define FILE_ITEM_6_ATTR                PSTR("5560")
//  #define FILE_ITEM_7_ATTR                PSTR("5570")
//  #define FILE_ITEM_8_ATTR                PSTR("5580")
//  #define FILE_PRINT_ARRT                 PSTR("55F0")


  // about WIFI setting
  #define WIFI_ITEM_STR_0                 PSTR("5390")  //30 bytes = 15 words
  #define WIFI_ITEM_STR_1                 PSTR("53A0")  //30 bytes = 15 words
  #define WIFI_ITEM_STR_2                 PSTR("53B0")  //30 bytes = 15 words
  #define WIFI_ITEM_STR_3                 PSTR("53C0")  //30 bytes = 15 words
  #define WIFI_ITEM_STR_4                 PSTR("53D0")  //30 bytes = 15 words
  #define WIFI_ITEM_ICO_0                 PSTR("539F")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_ITEM_ICO_1                 PSTR("53AF")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_ITEM_ICO_2                 PSTR("53BF")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_ITEM_ICO_3                 PSTR("53CF")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_ITEM_ICO_4                 PSTR("53DF")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_HOME_ICO                   PSTR("53EF")  //2 bytes  (1.0)  (0,1,2,3,4)

  #define WIFI_SELECT                     "53E0"  //30 bytes = 15 words
  #define WIFI_ITEM_CENTER                WIFI_ITEM_2
#else
  #define EX0_TAR_ADDR                    PSTR("0000")  //2 bytes  (3.0)
  #define EX1_TAR_ADDR                    PSTR("0001")  //2 bytes  (3.0)
  #define EX2_TAR_ADDR                    PSTR("0002")  //2 bytes  (3.0)
  #define EX0_CUR_ADDR                    PSTR("0003")  //2 bytes  (3.0)
  #define EX1_CUR_ADDR                    PSTR("0004")  //2 bytes  (3.0)
  #define EX2_CUR_ADDR                    PSTR("0005")  //2 bytes  (3.0)
  #define BED_TAR_ADDR                    PSTR("0006")  //2 bytes  (3.0)
  #define BED_CUR_ADDR                    PSTR("0007")  //2 bytes  (3.0)
  #define CHAMBER_TAR_ADDR                PSTR("0008")  //2 bytes  (3.0)
  #define CHAMBER_CUR_ADDR                PSTR("0009")  //2 bytes  (3.0)
  #define SPEED_GLOBAL_ADDR               PSTR("000B")  //2 bytes  (3.0)
  #define FAN_SPEED_ADDR                  PSTR("000C")  //2 bytes  (3.0)
  #define EXT_MULTIPLY_ADDR               PSTR("000D")  //2 bytes  (3.0)
  #define PREHEAT_FAN_SPEED_ADDR          PSTR("000E")  //2 bytes  (3.0)
  #define PREHEAT_EXT_TEMP_ADDR_0         PSTR("000F")  //2 bytes  (3.0)
  #define PREHEAT_EXT_TEMP_ADDR_1         PSTR("001A")  //2 bytes  (3.0)
  #define PREHEAT_EXT_TEMP_ADDR_2         PSTR("001B")  //2 bytes  (3.0)
  #define PREHEAT_BED_TEMP_ADDR           PSTR("0010")  //2 bytes  (3.0)
  #define X_STEP_ADDR                     PSTR("0011")  //4 bytes  (3.4)
  #define Y_STEP_ADDR                     PSTR("0013")  //4 bytes  (3.4)
  #define Z_STEP_ADDR                     PSTR("0015")  //4 bytes  (4.3)
  #define E_STEP_ADDR                     PSTR("0017")  //4 bytes  (3.4)
  #define PREHEAT_CHAMBER_TEMP_ADDR       PSTR("0019")  //2 bytes  (3.0)

  #define MOVE_DISTANCE_INDEX_ADDR        PSTR("0020")  //2 bytes  (1.0)  (1,2,3,4 => 0.1mm, 1mm, 10mm, 100mm)
  #define FILAMENT_EXT_INDEX_ADDR         PSTR("0021")  //2 bytes  (1.0)  (1,2,3,4,5,6; 1=2=4 => Ext0; 3=5 => Ext1; 6 => Ext2)
  #define FILAMENT_EXT_DISTANCE_ADDR      PSTR("0022")  //2 bytes  (3.0)
  #define PERCENT_ADDR                    PSTR("0023")  //2 bytes  (3.2)
  #define PRINT_TIME_ADDR                 PSTR("0024")  //5 bytes  (HEX)  (ddd:hh:mm)
  #define USED_TIME_ADDR                  PSTR("0027")  //4 bytes  (HEX)  (hhhh:mm:ss)
  #define TRIAL_PERIOD_ADDR               PSTR("0029")  //4 bytes  (HEX)  (hhhh:mm:ss)
  #define REG_KEY_ADDR                    PSTR("002B")  //2 bytes  (3.2)
  #define REG_STATE_INDEX_ADDR            PSTR("002C")  //2 bytes  (1.0)  (0,1 => invalid, success)

  #define VER_ADDR                        PSTR("0030")  //16 bytes =  8 words
  #define UI_ADDR                         PSTR("0038")  //16 bytes =  8 words
  #define MSG_ADDR                        PSTR("0040")  //32 bytes = 16 words
  #define PAINT_TOOL1_ADDR                PSTR("0050")  //16 bytes =  8 words
  #define PAINT_TOOL2_ADDR                PSTR("0058")  //16 bytes =  8 words
  #define X_STR_ADDR                      PSTR("0060")  // 2 bytes =  1 word
  #define Y_STR_ADDR                      PSTR("0061")  // 2 bytes =  1 word
  #define Z_STR_ADDR                      PSTR("0062")  // 2 bytes =  1 word
  #define SN_CAHR_ADDR                    PSTR("0130")  //16 bytes =  8 words
  #define MSG_ADDR_SHORT                  PSTR("0140")  //32 bytes = 16 words

  #define CUR_MAX_TEMP_ADDR               PSTR("0070")  //2 bytes  (3.0)
  #define TEMP_FAN_SPEED_ADDR             PSTR("0071")  //2 bytes  (3.0)
  #define AIR_FAN_SPEED_ADDR              PSTR("0072")  //2 bytes  (3.0)
  #define X_AXIS_ADDR                     PSTR("0073")  //4 bytes  (4.2)
  #define Y_AXIS_ADDR                     PSTR("0075")  //4 bytes  (4.2)
  #define Z_AXIS_ADDR                     PSTR("0077")  //4 bytes  (4.2)
  #define SERVO_Z_OFFSET_ADDR             PSTR("0079")  //2 bytes  (2.2)
  #define FADE_HEIGHT_ADDR                PSTR("007A")  //2 bytes  (3.1)
  #define WIFI_MODE_INDEX_ADDR            PSTR("007B")  //2 bytes  (1.0)  (0,1 => STA, AP)
  #define WIFI_STATE_INDEX_ADDR           PSTR("007C")  //2 btyes  (1.0)  (0,1,2 => connected, connecting, no_connect)
  #define WIFI_SCAN_LAST_ICO_ADDR         PSTR("007D")  //2 bytes  (1.0)  (0,1 => HIDE,SHOW)
  #define WIFI_SCAN_NEXT_ICO_ADDR         PSTR("007E")  //2 bytes  (1.0)  (0,1 => HIDE,SHOW)

  #define WIFI_IP_ADDR                    PSTR("0080")  //32 bytes = 16 words
  #define WIFI_SSID_ADDR                  PSTR("0090")  //32 bytes = 16 words
  #define WIFI_KEY_ADDR                   PSTR("00A0")  //32 bytes = 16 words
  #define WIFI_KEY_MASK_ADDR              PSTR("00B0")  //16 bytes (****************)
  #define WIFI_UUID_ADDR                  PSTR("00B8")  //16 bytes (991234xxxxxxxxx)
  #define WIFI_SSID_SET_ADDR              PSTR("00C0")  //32 bytes = 16 words

  #define SET_EX0_TAR_ADDR                PSTR("0100")  //2 bytes  (3.0)
  #define SET_EX1_TAR_ADDR                PSTR("0101")  //2 bytes  (3.0)
  #define SET_EX2_TAR_ADDR                PSTR("0102")  //2 bytes  (3.0)
  #define SET_BED_TAR_ADDR                PSTR("0106")  //2 bytes  (3.0)
  #define SET_CHAMBER_TAR_ADDR            PSTR("0108")  //2 bytes  (3.0)
  #define SET_SPEED_GLOBAL_ADDR           PSTR("010B")  //2 bytes  (3.0)
  #define SET_FAN_SPEED_ADDR              PSTR("010C")  //2 bytes  (3.0)
  #define SET_EXT_MULTIPLY_ADDR           PSTR("010D")  //2 bytes  (3.0)
  #define SET_PREHEAT_FAN_SPEED_ADDR      PSTR("010E")  //2 bytes  (3.0)
  #define SET_PREHEAT_EXT_TEMP_ADDR_0     PSTR("010F")  //2 bytes  (3.0)
  #define SET_PREHEAT_EXT_TEMP_ADDR_1     PSTR("011A")  //2 bytes  (3.0)
  #define SET_PREHEAT_EXT_TEMP_ADDR_2     PSTR("011B")  //2 bytes  (3.0)
  #define SET_PREHEAT_BED_TEMP_ADDR       PSTR("0110")  //2 bytes  (3.0)
  #define SET_PREHEAT_CHAMBER_TEMP_ADDR   PSTR("0119")  //2 bytes  (3.0)


  #define SET_X_STEP_ADDR                 PSTR("0111")  //4 bytes  (3.4)
  #define SET_Y_STEP_ADDR                 PSTR("0113")  //4 bytes  (3.4)
  #define SET_Z_STEP_ADDR                 PSTR("0115")  //4 bytes  (4.3)
  #define SET_E_STEP_ADDR                 PSTR("0117")  //4 bytes  (3.4)

  #define SET_REG_KEY_ADDR                PSTR("012B")  //2 bytes  (3.2)

  #define SET_TEMP_FAN_SPEED_ADDR         PSTR("0171")  //2 bytes  (3.0)
  #define SET_AIR_FAN_SPEED_ADDR          PSTR("0172")  //2 bytes  (3.0)
  #define SET_SERVO_Z_OFFSET_ADDR         PSTR("0179")  //2 bytes  (2.2)
  #define SET_FADE_HEIGHT_ADDR            PSTR("017A")  //2 bytes  (3.1)

  // about file setting
#ifdef LCD_FILE_CHAR_MAXIMIZE
  #define FILE_ITEM_STR_0                 PSTR("6000")  //512 bytes = 256 words
  #define FILE_ITEM_STR_1                 PSTR("6100")  //512 bytes = 256 words
  #define FILE_ITEM_STR_2                 PSTR("6200")  //512 bytes = 256 words
  #define FILE_ITEM_STR_3                 PSTR("6300")  //512 bytes = 256 words
  #define FILE_ITEM_STR_4                 PSTR("6400")  //512 bytes = 256 words
  #define FILE_ITEM_STR_5                 PSTR("6500")  //512 bytes = 256 words
  #define FILE_ITEM_STR_6                 PSTR("6600")  //512 bytes = 256 words
  #define FILE_ITEM_STR_7                 PSTR("6700")  //512 bytes = 256 words
  #define FILE_ITEM_STR_8                 PSTR("6800")  //512 bytes = 256 words
  #define FILE_SELECT                     PSTR("6F00")  //512 bytes = 256 words
#else
  #define FILE_ITEM_STR_0                 PSTR("0300")  //30 bytes = 15 words
  #define FILE_ITEM_STR_1                 PSTR("0310")  //30 bytes = 15 words
  #define FILE_ITEM_STR_2                 PSTR("0320")  //30 bytes = 15 words
  #define FILE_ITEM_STR_3                 PSTR("0330")  //30 bytes = 15 words
  #define FILE_ITEM_STR_4                 PSTR("0340")  //30 bytes = 15 words
  #define FILE_ITEM_STR_5                 PSTR("0350")  //30 bytes = 15 words
  #define FILE_ITEM_STR_6                 PSTR("0360")  //30 bytes = 15 words
  #define FILE_ITEM_STR_7                 PSTR("0370")  //30 bytes = 15 words
  #define FILE_ITEM_STR_8                 PSTR("0380")  //30 bytes = 15 words
  #define FILE_SELECT                     PSTR("03F0")  //30 bytes = 15 words
#endif

  #define FILE_ITEM_ICO_0                 PSTR("030F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_1                 PSTR("031F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_2                 PSTR("032F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_3                 PSTR("033F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_4                 PSTR("034F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_5                 PSTR("035F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_6                 PSTR("036F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_7                 PSTR("037F")  //2 bytes (1.0)  (0,1,2)
  #define FILE_ITEM_ICO_8                 PSTR("038F")  //2 bytes (1.0)  (0,1,2)

  #ifdef LCD_SIZE_43
    #define FILE_ITEM_CENTER              FILE_ITEM_STR_3
  #elif defined(LCD_SIZE_97)
    #define FILE_ITEM_CENTER              FILE_ITEM_STR_4
  #endif


//  #define FILE_ITEM_0_ATTR                PSTR("0500")
//  #define FILE_ITEM_1_ATTR                PSTR("0510")
//  #define FILE_ITEM_2_ATTR                PSTR("0520")
//  #define FILE_ITEM_3_ATTR                PSTR("0530")
//  #define FILE_ITEM_4_ATTR                PSTR("0540")
//  #define FILE_ITEM_5_ATTR                PSTR("0550")
//  #define FILE_ITEM_6_ATTR                PSTR("0560")
//  #define FILE_ITEM_7_ATTR                PSTR("0570")
//  #define FILE_ITEM_8_ATTR                PSTR("0580")
//  #define FILE_PRINT_ARRT                 PSTR("05F0")


  // about WIFI setting
  #define WIFI_ITEM_STR_0                 PSTR("0390")  //30 bytes = 15 words
  #define WIFI_ITEM_STR_1                 PSTR("03A0")  //30 bytes = 15 words
  #define WIFI_ITEM_STR_2                 PSTR("03B0")  //30 bytes = 15 words
  #define WIFI_ITEM_STR_3                 PSTR("03C0")  //30 bytes = 15 words
  #define WIFI_ITEM_STR_4                 PSTR("03D0")  //30 bytes = 15 words
  #define WIFI_ITEM_ICO_0                 PSTR("039F")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_ITEM_ICO_1                 PSTR("03AF")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_ITEM_ICO_2                 PSTR("03BF")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_ITEM_ICO_3                 PSTR("03CF")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_ITEM_ICO_4                 PSTR("03DF")  //2 bytes  (1.0)  (0,1,2,3,4)
  #define WIFI_HOME_ICO                   PSTR("03EF")  //2 bytes  (1.0)  (0,1,2,3,4)

  #define WIFI_SELECT                     PSTR("03E0")  //30 bytes = 15 words
  #define WIFI_ITEM_CENTER                WIFI_ITEM_2
#endif


// the cmd_code defined in LCD
#ifdef DWIN_LCD_USE_T5_CPU
  #define CMD_ADDR                    PSTR("5700")  //2 bytes
#else
  #define CMD_ADDR                    PSTR("0700")  //2 bytes
#endif
#define CMD_EMPTY                     PSTR("0000")

#define RETURN_UP_LEVEL_BUTTON        0x0700
#define RETURN_DEFAULT_BUTTON         0x0701

#define PAUSE_BUTTON                  0x0702
#define REUSE_BUTTON                  0x0703
#define CANCEL_BUTTON                 0x0704
#define PREHEAT_BUTTON                0x0705
#define COOLDOWN_BUTTON               0x0706
#define OPEN_FILE_BUTTON              0x0707
#define OPEN_FILAMENT_BUTTON          0x0708
#define CHANGE_BUTTON                 0x0709
#define Z_UP_BUTTON                   0x070A
#define Z_DOWN_BUTTON                 0x070B
#define SETTING_BUTTON                0x070C
#define ADJUST_BUTTON                 0x070D
#define REG_INFO_BUTTON               0x070E

#define MOVE_HOME_ALL                 0x0710
#define MOVE_HOME_X                   0x0711
#define MOVE_HOME_Y                   0x0712
#define MOVE_HOME_Z                   0x0713
#define MOVE_X_PLUS                   0x0714
#define MOVE_Y_PLUS                   0x0715
#define MOVE_Z_PLUS                   0x0716
#define MOVE_X_MINUS                  0x0717
#define MOVE_Y_MINUS                  0x0718
#define MOVE_Z_MINUS                  0x0719

#define FILAMENT_EXT                  0x0720
#define FILAMENT_REC                  0x0721
#define FILAMENT_UNLOAD               0x0722

#define STORE_SETTINGS                0x0730
#define RESET_SETTINGS                0x0731
//#define JUMP_SETGINS_2                0x0732
//#define JUMP_LEVELING                 0x0733
//#define JUMP_SETGINS                  0x0734
#define JUMP_SETGING_MOTION           0x0734
#define JUMP_SETTING_FAN              0x0735
#define JUMP_SETTING_PREHEAT          0x0736
#define JUMP_SETTING_WIFI             0x0737
#define JUMP_SETTING_LEVELING         0x0738

#define FILE_NEXT_PAGE                0x0740
#define FILE_LAST_PAGE                0x0741
#define FILE_UP_LEVEL                 0x0742

#define WIFI_SWITCH_TOGGLE            0x075F
//#define WIFI_SWITCH_MODE              0x0750
#define WIFI_RESET                    0x0751
#define WIFI_SCAN                     0x0752
#define WIFI_NEXT_PAGE                0x0753
#define WIFI_LAST_PAGE                0x0754
#define WIFI_MODE_STA                 0x075C
#define WIFI_MODE_AP                  0x075D
#define WIFI_SETTING_ENTRY            0x075E

#define LEVELING_SWITCH_TOGGLE        0x076F
#define LEVELING_PROBE                0x0760

#define FILE_ITEM_0_ACTION            0x07A0
#define FILE_ITEM_1_ACTION            0x07A1
#define FILE_ITEM_2_ACTION            0x07A2
#define FILE_ITEM_3_ACTION            0x07A3
#define FILE_ITEM_4_ACTION            0x07A4
#define FILE_ITEM_5_ACTION            0x07A5
#define FILE_ITEM_6_ACTION            0x07A6
#define FILE_ITEM_7_ACTION            0x07A7
#define FILE_ITEM_8_ACTION            0x07A8

#define WIFI_ITEM_0_ACTION            0x07A9
#define WIFI_ITEM_1_ACTION            0x07AA
#define WIFI_ITEM_2_ACTION            0x07AB
#define WIFI_ITEM_3_ACTION            0x07AC
#define WIFI_ITEM_4_ACTION            0x07AD

//#define FILE_ACTION_OPEN              0x00B0
#define FILE_ACTION_PRINT             0x00B1
#define FILE_ACTION_CANCEL            0x00B2
#define SHUTTING_CANCEL               0x00B3
#define ACCIDENT_REUSE                0x00B4
#define ACCIDENT_CANCEL               0x00B5
#define CHANGE_DONE                   0x00B6
//#define ONLINE_RETURN                 0x00B7
#define WIFI_CONNECT                  0x00B8
#define WIFI_CANCEL                   0x00B9
#define RESET_SETTINGS_CONFIRM        0x00BA
#define STOP_PRINT_CONFIRM            0x00BB
#define LEVELING_SKIP_CONFIRM         0x00BC
#define LEVELING_AGAIN_CONFIRM        0x00BD

// Key control Setting
//#define RESET_INFO_KEY                "09"  // "01"   // pop a reset info
//#define STORE_INFO_KEY                "0A"  // "02"   // pop a store info
//#define MOVE_INFO_KEY                 "0B"  // "03"   // pop a move info
//#define COLD_INFO_KEY                 "0C"  // "04"   // pop a cold info
//#define EXT_INFO_KEY                  "0D"  // "05"   // pop a extrusion info
//#define REC_INFO_KEY                  "0E"  // "06"   // pop a retract info
//#define UNLOAD_INFO_KEY               "0F"  // "07"   // pop a unload info
//#define RESET_DONE_INFO_KEY           "10"  // "08"   // pop a reset done info
//#define STORE_DONE_INFO_KEY           "11"  // "09"   // pop a store done info
//#define SHUT_COLD_KEY                 "12"  // "0A"   // pop a window that shutting down with hot extruder
//#define ACCIDENT_KEY                  "13"  // "0B"   // pop a window that accident shutting down
//#define CHANGE_FILAMENT_KEY           "14"  // "0C"   // pop a filament change window
//#define WAIT_KEY                      "15"  // "0D"   // pop a wait window
//#define EXT_INDEX_KEY_1               "18"  // "10"   // pop a extruder chose window
//#define EXT_INDEX_KEY_2               "19"  // "11"   // pop a extruder chose window
//#define EXT_INDEX_KEY_3               "1A"  // "12"   // pop a extruder chose window
//#define FILE_FILE_KEY                 "30"            // pop a print window
//#define FILE_DIR_KEY                  "31"            // pop a open window

// OS_POP_ICO
#define POP_ICO_EXTRUDE_FILAMENT      PSTR("0101")
#define POP_ICO_RECRACT_FILAMENT      PSTR("0102")
#define POP_ICO_PREPARE_UNLOAD        PSTR("0103")
#define POP_ICO_UNLOAD_FILAMENT       PSTR("0104")
#define POP_ICO_SETTING_RESTORING     PSTR("0105")
#define POP_ICO_SETTING_SAVING        PSTR("0106")
#define POP_ICO_HOMING                PSTR("0107")
#define POP_ICO_MOVING                PSTR("0108")
#define POP_ICO_PARKING               PSTR("0109")
#define POP_ICO_RESUMING              PSTR("010A")
#define POP_ICO_PROBING               PSTR("010B")
#define POP_ICO_WAITING               PSTR("010C")
#define POP_ICO_CUR_TEMP_LOW          PSTR("010D")
#define POP_ICO_TAR_TEMP_LOW          PSTR("010E")
#define POP_ICO_INVALID_LEVLEL_DATA   PSTR("010F")
#define POP_ICO_PROBER_UNAVAILABLE    PSTR("0110")
#define POP_ICO_WIFI_CONNECTING       PSTR("0111")
// OS_POP_KEY
#define POP_KEY_RESET_CONFIRM         PSTR("0009")    // control by LCD_OS
#define POP_KEY_CHANGE_FILAMENT       PSTR("FF14")
#define POP_KEY_STOP_CONFIRM          PSTR("0016")    // control by LCD_OS
#define POP_KEY_PROBE_CONFIRM         PSTR("FF17")


// the DEMO addr that will init just MCU setup
//#define COLOR_BLACK                   PSTR("0400")  //2 bytes
//#define COLOR_WHITE                   PSTR("0401")  //2 bytes
//#define COLOR_RED                     PSTR("0402")  //2 bytes
//#define COLOR_ORANGE                  PSTR("0403")  //2 bytes
//#define COLOR_YELLOW                  PSTR("0404")  //2 bytes
//#define COLOR_GREEN                   PSTR("0405")  //2 bytes
//#define COLOR_CYAN                    PSTR("0406")  //2 bytes
//#define COLOR_BLUE                    PSTR("0407")  //2 bytes
//#define COLOR_PURPLE                  PSTR("0408")  //2 bytes
//#define COLOR_TEMP                    PSTR("04FF")  //2 bytes



#if HAS_READER
  #ifdef SDSUPPORT
    #define PAGE_NORAML_SERIAL              PAGE_NORAML_SERIAL_SD
    #define PAGE_NORMAL_PREHEAT_READER      PAGE_NORMAL_PREHEAT_SD
    #define PAGE_NORMAL_PREHEAT_NO_READER   PAGE_NORMAL_PREHEAT_NO_SD
    #define PAGE_NORMAL_COOLDOWN_READER     PAGE_NORMAL_COOLDOWN_SD
    #define PAGE_NORMAL_COOLDOWN_NO_READER  PAGE_NORMAL_COOLDOWN_NO_SD
    #define DWIN_MSG_READER_CONN            DWIN_MSG_SD_INSERTED
    #define DWIN_MSG_READER_READY           DWIN_MSG_SD_INSERTED
    #define DWIN_MSG_READER_REMOVED         DWIN_MSG_SD_REMOVED
  #elif defined(UDISKSUPPORT)
    #define PAGE_NORAML_SERIAL              PAGE_NORAML_SERIAL_USB
    #define PAGE_NORMAL_PREHEAT_READER      PAGE_NORMAL_PREHEAT_USB
    #define PAGE_NORMAL_PREHEAT_NO_READER   PAGE_NORMAL_PREHEAT_NO_USB
    #define PAGE_NORMAL_COOLDOWN_READER     PAGE_NORMAL_COOLDOWN_USB
    #define PAGE_NORMAL_COOLDOWN_NO_READER  PAGE_NORMAL_COOLDOWN_NO_USB
    #define DWIN_MSG_READER_CONN            DWIN_MSG_USB_INSERTED
    #define DWIN_MSG_READER_READY           DWIN_MSG_USB_READY
    #define DWIN_MSG_READER_REMOVED         DWIN_MSG_USB_REMOVED
  #endif
#else
  #define PAGE_NORAML_SERIAL                PAGE_NORAML_SERIAL_SD
  #define PAGE_NORMAL_PREHEAT_NO_READER     PAGE_NORMAL_PREHEAT_NO_SD
  #define PAGE_NORMAL_COOLDOWN_NO_READER    PAGE_NORMAL_COOLDOWN_NO_SD
#endif

#if EXTRUDERS > 2
  #ifdef HOTWIND_SYSTEM
    #error "DWIN_V5 don't support 3 EXTRUDER with CHAMBER"
  #else
    #define PAGE_PARSE(x)    (x##_3)
  #endif
#elif EXTRUDERS > 1
  #ifdef HOTWIND_SYSTEM
    #define PAGE_PARSE(x)    (x##_2_CHAMBER)
  #else
    #define PAGE_PARSE(x)    (x##_2)
  #endif
#elif EXTRUDERS > 0
  #ifdef HOTWIND_SYSTEM
    #define PAGE_PARSE(x)    (x##_1_CHAMBER)
  #else
    #define PAGE_PARSE(x)    (x##_1)
  #endif
#endif




extern int lcd_preheat_hotend_temp[EXTRUDERS];
extern int lcd_preheat_bed_temp;
#ifdef HOTWIND_SYSTEM
  extern int lcd_preheat_chamber_temp;
#endif
extern int lcd_preheat_fan_speed;

extern bool cancel_heatup;

#ifdef POWER_MANAGEMENT
  extern char dwin_shutting_info[LCD_MSG_CHAR_LEN + 1];
#endif

void returnDefaultButtonAction();

void printPauseButtonAction();
void printReuseButtonAction();
void printCancelButtonAction();

enum FILE_UPDATE_MODE{ OPEN, NEXT_PAGE, LAST_PAGE, UP_LEVEL };
void dwinGetFileName(uint8_t fileItem = 0xFF);
void lcdUpdateFiles(uint8_t mode);
void fileItemAction(uint8_t index);

#ifdef WIFI_SUPPORT
  void jumpSettingWifiAction();
  void dwin_update_wifi_list_info();
  void dwin_update_station_info(const char* ssid, const char* key);
  void dwin_update_wifi_uuid(const char* uuid);
#endif

//void setupDEMO();
void dwin_setup();
void dwin_run();

void dwin_update_msg(const char* msg);
void dwin_update_msg_P(const char* msg);

void dwin_change_page_P(const char* page);
bool dwin_is_page_P(const char* page);

#define DWIN_MSG(msg)               dwin_update_msg(msg)
#define DWIN_MSG_P(msg)             dwin_update_msg_P(PSTR(msg))

#ifdef DWIN_USE_OS
void dwin_hide_popup();
#define HAS_POPUP                   (getPop() != POPUP_INDEX_NO_POP)
#define IS_POPUP(index)             (uint16_tCompHex_P(index, getPop()))
#define POP_WINDOW(index, delay)    (setPop_P(index, delay))
#define HIDE_POPUP                  dwin_hide_popup()
#else
#define HAS_POPUP                   (getPop() != POPUP_HIDDEN_KEY)
#define IS_POPUP(index)             (uint16_tCompHex_P(index, getPop()))
#define POP_WINDOW(key, delay)      (setPop_P(key))
#define HIDE_POPUP                  (setPop(POPUP_HIDDEN_KEY))
#endif

#define DWIN_TOUCH                  (touchState() == 1)


#define GO_PAGE(x)                  dwin_change_page_P(x)
#define DWIN_IS_PAGE(index)         (dwin_is_page_P(index))

#define IS_IDLE_PAGE                (DWIN_IS_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT)) || DWIN_IS_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE)) || \
                                      DWIN_IS_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT)) || DWIN_IS_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE)))
#define IS_PRINT_PAGE               (DWIN_IS_PAGE(PAGE_PARSE(PAGE_DEFAULT_PRINT_PAUSE)) || DWIN_IS_PAGE(PAGE_PARSE(PAGE_DEFAULT_PRINT_RESUME)))
#define IS_ONLINE_PAGE              (DWIN_IS_PAGE(PAGE_PARSE(PAGE_DEFAULT_ONLINE)))
#define IS_DEFAULT_PAGE             (IS_IDLE_PAGE || IS_PRINT_PAGE || IS_ONLINE_PAGE )

#define IS_MOVE_PAGE                (DWIN_IS_PAGE(PAGE_MOVE))
#define IS_FILAMENT_PAGE            (DWIN_IS_PAGE(PAGE_PARSE(PAGE_FILAMENT)))
#define IS_AXIS_INFO_PAGE           (IS_PRINT_PAGE || DWIN_IS_PAGE(PAGE_MOVE))
#define IS_FILE_PAGE                (DWIN_IS_PAGE(PAGE_FILE_ONE) || DWIN_IS_PAGE(PAGE_FILE_NEXT) || DWIN_IS_PAGE(PAGE_FILE_BOTH) || DWIN_IS_PAGE(PAGE_FILE_LAST) || \
                                      DWIN_IS_PAGE(PAGE_FILE_UP_ONE) || DWIN_IS_PAGE(PAGE_FILE_UP_NEXT) || DWIN_IS_PAGE(PAGE_FILE_UP_BOTH) || DWIN_IS_PAGE(PAGE_FILE_UP_LAST))

#define IS_WIFI_SETTING_PAGE        (DWIN_IS_PAGE(PAGE_WIFI_SETTING_OFF) || DWIN_IS_PAGE(PAGE_WIFI_SETTING_ON) || DWIN_IS_PAGE(PAGE_CONNECT_WIFI_12))
#define IS_WIFI_INFO_PAGE           (DWIN_IS_PAGE(PAGE_WIFI_INFO_STATION) || DWIN_IS_PAGE(PAGE_WIFI_INFO_AP))
#define IS_WIFI_PAGE                (IS_WIFI_SETTING_PAGE || IS_WIFI_INFO_PAGE)

#define IS_LEVELING_PAGE            (DWIN_IS_PAGE(PAGE_LEVELING_SETTING_ON) || DWIN_IS_PAGE(PAGE_LEVELING_SETTING_OFF) || DWIN_IS_PAGE(PAGE_LEVELING_SETTING_DISABLE))

#define IS_REG_PAGE                 (DWIN_IS_PAGE(PAGE_REG))

#define IS_HOLDING_PAGE             (DWIN_IS_PAGE(PAGE_UNFINISH_CHOOSE) || DWIN_IS_PAGE(PAGE_SHUTDOWN_HOTTEMP) || DWIN_IS_PAGE(PAGE_REG) || IS_WIFI_INFO_PAGE)


#else  //!DWIN_LCD

#define DWIN_TOUCH 0
#define DWIN_MSG(msg)
#define DWIN_MSG_P(msg)

#endif //DWIN_LCD

#endif //DWIN_LCD_H
