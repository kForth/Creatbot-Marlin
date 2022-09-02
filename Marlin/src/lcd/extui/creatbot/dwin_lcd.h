/*
  dwin_lcd.h - Hardware serial library for DWIN UART LCM.
  Copyright (c) 2015 CreatBot. All Right Reserved.

  Developer Lyn Lee, 3 June 2015.
*/

#ifndef DWIN_LCD_H
#define DWIN_LCD_H

#include "../../../inc/MarlinConfigPre.h"

#ifdef CREATBOT_LCD

#include "dwin_lcd_implementation.h"

#define LCD_SETUP_FIRST							"0000"
#define LCD_SETUP_LOOP_STRAT				0			// "000D"
#define LCD_SETUP_LOOP_END					6			// "0024"
#define LCD_SETUP_NUM								40		// "0028"
#define LCD_SETUP_LOOP_TIMES				1			// must greater than 0
#define LCD_FILE_ITEM_PER_PAGE			8
#define LCD_FILE_CHAR_LEN						32		// 32 char = 16 word
#define LCD_VER_CHAR_LEN						32		// 32 char = 16 word
#define LCD_MSG_CHAR_LEN						32		// 32 char = 16 word
#define LCD_HOST_CHAR_LEN						32		// 32 char = 16 word
#define LCD_WIFI_CHAR_LEN						30		// 30 char = 15 word
#define LCD_TIMEOUT_TO_STATUS				15000	// 15s

#if HAS_READER
	#define FILE_NAME_LEN		(LCD_FILE_CHAR_LEN < (LONG_FILENAME_LENGTH - 1)  ? LCD_FILE_CHAR_LEN : (LONG_FILENAME_LENGTH - 1))
#endif

#define PROGRESS_BAR_0_X0						0
#define PROGRESS_BAR_0_Y0						110
#define PROGRESS_BAR_0_X1						480
#define PROGRESS_BAR_0_Y1						160
#define COLOR_PRIMARY								"033F"	//BLUE
#define COLOR_ASSIST								"18C3"	//GRAY
#define PROGRESS_BAR_1_X0						1
#define PROGRESS_BAR_1_Y0						140
#define PROGRESS_BAR_1_X1						388
#define PROGRESS_BAR_1_Y1						164

// the addr of page that defined in DWIN UART LCM.
#define PAGE_REG											"001F"	//31
#define PAGE_REG_INFO									"0020"	//32
#define PAGE_NORAML_SERIAL_USB				"0021"	//33
#define PAGE_NORAML_SERIAL_SD					"0022"	//34
#define PAGE_NORMAL_PREHEAT_USB				"0023"	//35
#define PAGE_NORMAL_PREHEAT_NO_USB		"0024"	//36
#define PAGE_NORMAL_COOLDOWN_USB			"0025"	//37
#define PAGE_NORMAL_COOLDOWN_NO_USB		"0026"	//38
#define PAGE_NORMAL_PREHEAT_SD				"0029"	//41
#define PAGE_NORMAL_PREHEAT_NO_SD			"002A"	//42
#define PAGE_NORMAL_COOLDOWN_SD				"002B"	//43
#define PAGE_NORMAL_COOLDOWN_NO_SD		"002C"	//44
#define PAGE_PRINT_PAUSE							"002F"	//47
#define PAGE_PRINT_RESUME							"0030"	//48
#define PAGE_STATE_NORMAL_1						"0033"	//51
#define PAGE_STATE_NORMAL_2						"0034"	//52
#define PAGE_STATE_NORMAL_3						"0035"	//53
#define PAGE_STATE_PRINT_1						"0036"	//54
#define PAGE_STATE_PRINT_2						"0037"	//55
#define PAGE_STATE_PRINT_3						"0038"	//56
#define PAGE_MOVE											"003B"	//59
#define PAGE_FILAMENT_1								"003D"	//61
#define PAGE_FILAMENT_2								"003E"	//62
#define PAGE_FILAMENT_3								"003F"	//63
#define PAGE_SHUTDOWN_HOTEND					"0041"	//65
#define PAGE_FILE_ONE									"0043"	//67
#define PAGE_FILE_NEXT								"0044"	//68
#define PAGE_FILE_LAST								"0045"	//69
#define PAGE_FILE_BOTH								"0046"	//70
#define PAGE_FILE_UP_ONE							"0047"	//71
#define PAGE_FILE_UP_NEXT							"0048"	//72
#define PAGE_FILE_UP_LAST							"0049"	//73
#define PAGE_FILE_UP_BOTH							"004A"	//74
#define PAGE_SETTINGS									"004C"	//76
#define PAGE_INFO											"004E"	//78
#define PAGE_PRINT_CONFIRM						"0050"	//80
#define PAGE_UNFINISH_CHOOSE					"0052"	//82
#define PAGE_SETTINGS_2								"005D"	//93
#define PAGE_ONLINE_INFO							"005F"	//95
#define PAGE_WIFI_INFO								"0062"	//98
#define PAGE_WIFI_CONNECT							"0064"	//100
#define PAGE_STATE_NORMAL_2_AIR				"0068"	//104
#define PAGE_STATE_PRINT_2_AIR				"0069"	//105
#define PAGE_SETTINGS_2_WIFI_OFF			"006A"	//106
#define PAGE_SETTINGS_2_WIFI_ON				"006B"	//107
#define PAGE_LEVELING_DISABLE_INVALID	"006D"	//109
#define PAGE_LEVELING_DISABLE_OFF			"006E"	//110
#define PAGE_LEVELING_DISABLE_ON			"006F"	//111
#define PAGE_LEVELING_INVALID					"0070"	//112
#define PAGE_LEVELING_VALID_ON				"0071"	//113
#define PAGE_LEVELING_VALID_OFF				"0072"	//114
#define PAGE_SETTINGS_V2							"0077"	//119
#define PAGE_SETTINGS_V2_MORE					"0079"	//121
#define PAGE_STATE_NORMAL_1_AIR				"007B"	//123
#define PAGE_STATE_PRINT_1_AIR				"007C"	//124


// the addr of varible that defined in DWIN UART LCM.
#define EX0_TAR_ADDR									"0000"	//2 bytes	(3.0)
#define EX1_TAR_ADDR									"0001"	//2 bytes	(3.0)
#define EX2_TAR_ADDR									"0002"	//2 bytes	(3.0)
#define EX0_CUR_ADDR									"0003"	//2 bytes	(3.1)
#define EX1_CUR_ADDR									"0004"	//2 bytes	(3.1)
#define EX2_CUR_ADDR									"0005"	//2 bytes	(3.1)
#define BED_TAR_ADDR									"0006"	//2 bytes	(3.0)
#define BED_CUR_ADDR									"0007"	//2 bytes	(3.1)
#define CHAMBER_TAR_ADDR							"0008"	//2 bytes (3.0)
#define CHAMBER_CUR_ADDR							"0009"	//2 bytes (3.1)
#define X_AXIS_ADDR										"0073"	//4 bytes	(4.2)
#define Y_AXIS_ADDR										"0075"	//4 bytes	(4.2)
#define Z_AXIS_ADDR										"0077"	//4 bytes	(4.2)
#define SPEED_GLOBAL_ADDR							"000B"	//2 bytes	(3.0)
#define FAN_SPEED_ADDR								"000C"	//2 bytes	(3.0)
#define EXT_MULTIPLY_ADDR							"000D"	//2 bytes	(3.0)
#define PREHEAT_FAN_SPEED_ADDR				"000E"	//2 bytes	(3.0)
#define PREHEAT_EXT_TEMP_ADDR_0				"000F"	//2 bytes	(3.0)
#define PREHEAT_EXT_TEMP_ADDR_1				"001A"	//2 bytes	(3.0)
#define PREHEAT_EXT_TEMP_ADDR_2				"001B"	//2 bytes	(3.0)
#define PREHEAT_BED_TEMP_ADDR					"0010"	//2 bytes	(3.0)
#define X_STEP_ADDR										"0011"	//4 bytes	(3.4)
#define Y_STEP_ADDR										"0013"	//4 bytes	(3.4)
#define Z_STEP_ADDR										"0015"	//4 bytes	(3.4)
#define E_STEP_ADDR										"0017"	//4 bytes	(3.4)
#define PREHEAT_CHAMBER_TEMP_ADDR			"0019"	//2 bytes	(3.0)

#define MOVE_DISTANCE_INDEX_ADDR			"0020"	//2 bytes	(1.0)  (1,2,3,4 => 0.1mm, 1mm, 10mm, 100mm)
#define FILAMENT_EXT_INDEX_ADDR				"0021"	//2 bytes	(1.0)  (1,2,3,4,5,6; 1=2=4 => Ext0; 3=5 => Ext1; 6 => Ext2)
#define FILAMENT_EXT_DISTANCE_ADDR		"0022"	//2 bytes	(3.0)
#define PERCENT_ADDR									"0023"	//2 bytes	(3.2)
#define PRINT_TIME_ADDR								"0024"	//3 bytes	(HEX) (xx:xx:xx)
#define USED_TIME_ADDR								"0026"  //4 bytes	(HEX) (xxxx:xx:xx)
#define TRIAL_PERIOD_ADDR							"0028"	//4 bytes (HEX) (xxxx:xx:xx)
#define REG_KEY_ADDR									"002A"	//2 bytes (3.2)
#define REG_STATE_INDEX_ADDR					"002B"	//2 bytes (1.0)	(0,1 => invalid, success)

#define VER_ADDR											"0030"	//32 bytes = 16 words
#define MSG_ADDR											"0040"	//32 bytes = 16 words
#define PAINT_TOOL1_ADDR							"0050"	//16 bytes = 8 words
#define PAINT_TOOL2_ADDR							"0058"	//16 bytes = 8 words
#define PAINT_TOOL3_ADDR							"0060"	//16 bytes = 8 words
#define PAINT_TOOL4_ADDR							"0068"	//16 bytes = 8 words

#define CUR_MAX_TEMP_ADDR							"0070"	//2 bytes	(3.1)
#define TEMP_FAN_SPEED_ADDR						"0071"	//2 bytes	(3.0)
#define AIR_FAN_SPEED_ADDR						"0072"	//2 bytes	(3.0)
//#define X_AXIS_ADDR										"0073"	//4 bytes	(4.2)
//#define Y_AXIS_ADDR										"0075"	//4 bytes	(4.2)
//#define Z_AXIS_ADDR										"0077"	//4 bytes	(4.2)
#define SERVO_Z_OFFSET_ADDR						"0079"	//2 bytes (2.2)
#define FADE_HEIGHT_ADDR							"007A"  //2 bytes (3.1)
#define WIFI_MODE_INDEX_ADDR					"007B"	//2 bytes	(1.0)	(0,1 => STA+AP, STA)
#define WIFI_STATE_INDEX_ADDR					"007C"	//2 btyes	(1.0)	(0,1,2 => connected, connecting, no_connect)
#define WIFI_IP_ADDR									"0080"	//32 bytes = 16 words
#define WIFI_SSID_ADDR								"0090"	//32 bytes = 16 words
#define WIFI_KEY_ADDR									"00A0"	//32 bytes = 16 words

#define SET_EX0_TAR_ADDR							"0100"	//2 bytes	(3.0)
#define SET_EX1_TAR_ADDR							"0101"	//2 bytes	(3.0)
#define SET_EX2_TAR_ADDR							"0102"	//2 bytes	(3.0)
#define SET_BED_TAR_ADDR							"0106"	//2 bytes	(3.0)
#define SET_CHAMBER_TAR_ADDR					"0108"	//2 bytes (3.0)
#define SET_SPEED_GLOBAL_ADDR					"010B"	//2 bytes	(3.0)
#define SET_FAN_SPEED_ADDR						"010C"	//2 bytes	(3.0)
#define SET_EXT_MULTIPLY_ADDR					"010D"	//2 bytes	(3.0)
#define SET_PREHEAT_FAN_SPEED_ADDR		"010E"	//2 bytes	(3.0)
#define SET_PREHEAT_EXT_TEMP_ADDR_0		"010F"	//2 bytes	(3.0)
#define SET_PREHEAT_EXT_TEMP_ADDR_1		"011A"	//2 bytes	(3.0)
#define SET_PREHEAT_EXT_TEMP_ADDR_2		"011B"	//2 bytes	(3.0)
#define SET_PREHEAT_BED_TEMP_ADDR			"0110"	//2 bytes	(3.0)
#define SET_PREHEAT_CHAMBER_TEMP_ADDR	"0119"	//2 bytes (3.0)


#define SET_X_STEP_ADDR								"0111"	//4 bytes	(3.4)
#define SET_Y_STEP_ADDR								"0113"	//4 bytes	(3.4)
#define SET_Z_STEP_ADDR								"0115"	//4 bytes	(3.4)
#define SET_E_STEP_ADDR								"0117"	//4 bytes	(3.4)

#define SET_REG_KEY_ADDR							"012A"	//2 bytes (3.2)

#define SET_TEMP_FAN_SPEED_ADDR				"0171"	//2 bytes	(3.0)
#define SET_AIR_FAN_SPEED_ADDR				"0172"	//2 bytes	(3.0)
#define SET_SERVO_Z_OFFSET_ADDR				"0179"	//2 bytes (2.2)
#define SET_FADE_HEIGHT_ADDR					"017A"  //2 bytes (3.1)

// the cmd_code defined in LCD
#define CMD_ADDR										"0700"	//2 bytes
#define CMD_EMPTY										"0000"

#define RETURN_STATE_BUTTON					0x0700
#define RETURN_DEFAULT_BUTTON				0x0701

#define PAUSE_BUTTON								0x0702
#define REUSE_BUTTON								0x0703
#define CANCEL_BUTTON								0x0704
#define PREHEAT_BUTTON							0x0705
#define COOL_DWON_BUTTON						0x0706
#define OPEN_FILE_BUTTON						0x0707
#define OPEN_FILAMENT_BUTTON				0x0708
#define CHANGE_BUTTON								0x0709
#define Z_UP_BUTTON									0x070A
#define Z_DOWN_BUTTON								0x070B
#define REG_INFO_BUTTON							0x070E

#define MOVE_HOME_ALL								0x0710
#define MOVE_HOME_X									0x0711
#define MOVE_HOME_Y									0x0712
#define MOVE_HOME_Z									0x0713
#define MOVE_X_PLUS									0x0714
#define MOVE_Y_PLUS									0x0715
#define MOVE_Z_PLUS									0x0716
#define MOVE_X_MINUS								0x0717
#define MOVE_Y_MINUS								0x0718
#define MOVE_Z_MINUS								0x0719

#define FILAMNET_EXT								0x0720
#define FILAMENT_REC								0x0721
#define FILAMNET_UNLOAD							0x0722

#define STORE_SETTINGS							0x0730
#define RESET_SETTINGS							0x0731
#define JUMP_SETGINS_2							0x0732
#define JUMP_LEVELING								0x0733
#define JUMP_SETGINS								0x0734

#define FILE_NEXT_PAGE							0x0740
#define FILE_LAST_PAGE							0x0741
#define FILE_UP_LEVEL								0x0742

#define WIFI_SWITCH_TOGGLE					0x075F
#define WIFI_SWITCH_MODE						0x0750
#define WIFI_RESET									0x0751
#define WIFI_SCAN										0x0752
#define WIFI_NEXT_PAGE							0x0753
#define WIFI_LAST_PAGE							0x0754

#define LEVELING_SWITCH_TOGGLE			0x076F
#define LEVELING_PROBE							0x0760

#define FILE_ITEM_0_ACTION					0x07A0
#define FILE_ITEM_1_ACTION					0x07A1
#define FILE_ITEM_2_ACTION					0x07A2
#define FILE_ITEM_3_ACTION					0x07A3
#define FILE_ITEM_4_ACTION					0x07A4
#define FILE_ITEM_5_ACTION					0x07A5
#define FILE_ITEM_6_ACTION					0x07A6
#define FILE_ITEM_7_ACTION					0x07A7
#define FILE_ITEM_8_ACTION					0x07A8

#define WIFI_ITEM_0_ACTION					0x07A9
#define WIFI_ITEM_1_ACTION					0x07AA
#define WIFI_ITEM_2_ACTION					0x07AB
#define WIFI_ITEM_3_ACTION					0x07AC
#define WIFI_ITEM_4_ACTION					0x07AD

//#define FILE_ACTION_OPEN						0x00B0
#define FILE_ACTION_PRINT						0x00B1
#define FILE_ACTION_CANCEL					0x00B2
#define SHUTTING_CANCEL							0x00B3
#define ACCIDENT_REUSE							0x00B4
#define ACCIDENT_CANCEL							0x00B5
#define CHANGE_DONE									0x00B6
#define ONLINE_RETURN								0x00B7
#define WIFI_CONNECT								0x00B8
#define WIFI_CANCEL									0x00B9

// about file setting
#define FILE_ITEM_0									"0300"	//32 bytes = 16 words
#define FILE_ITEM_1									"0310"	//32 bytes = 16 words
#define FILE_ITEM_2									"0320"	//32 bytes = 16 words
#define FILE_ITEM_3									"0330"	//32 bytes = 16 words
#define FILE_ITEM_4									"0340"	//32 bytes = 16 words
#define FILE_ITEM_5									"0350"	//32 bytes = 16 words
#define FILE_ITEM_6									"0360"	//32 bytes = 16 words
#define FILE_ITEM_7									"0370"	//32 bytes = 16 words
#define FILE_ITEM_8									"0380"	//32 bytes = 16 words
#define FILE_SELECT									"03F0"	//32 bytes = 16 words
#define FILE_ITEM_CENTER						FILE_ITEM_3

//#define FILE_ITEM_0_ATTR						"0500"
//#define FILE_ITEM_1_ATTR						"0510"
//#define FILE_ITEM_2_ATTR						"0520"
//#define FILE_ITEM_3_ATTR						"0530"
//#define FILE_ITEM_4_ATTR						"0540"
//#define FILE_ITEM_5_ATTR						"0550"
//#define FILE_ITEM_6_ATTR						"0560"
//#define FILE_ITEM_7_ATTR						"0570"
//#define FILE_ITEM_8_ATTR						"0580"
//#define FILE_PRINT_ARRT							"05F0"


// about WIFI setting
#define WIFI_ITEM_STR_0							"0390"	//30 bytes = 15 words
#define WIFI_ITEM_STR_1							"03A0"	//30 bytes = 15 words
#define WIFI_ITEM_STR_2							"03B0"	//30 bytes = 15 words
#define WIFI_ITEM_STR_3							"03C0"	//30 bytes = 15 words
#define WIFI_ITEM_STR_4							"03D0"	//30 bytes = 15 words
#define WIFI_ITEM_ICO_0							"039F"	//2 bytes	(1.0)  (0,1,2,3,4)
#define WIFI_ITEM_ICO_1							"03AF"	//2 bytes	(1.0)  (0,1,2,3,4)
#define WIFI_ITEM_ICO_2							"03BF"	//2 bytes	(1.0)  (0,1,2,3,4)
#define WIFI_ITEM_ICO_3							"03CF"	//2 bytes	(1.0)  (0,1,2,3,4)
#define WIFI_ITEM_ICO_4							"03DF"	//2 bytes	(1.0)  (0,1,2,3,4)

#define WIFI_SELECT									"03E0"	//32 bytes = 16 words
#define WIFI_ITEM_CENTER						WIFI_ITEM_2


// Key control Setting
#define RESET_INFO_KEY							"01"	// pop a reset info
#define STORE_INFO_KEY							"02"	// pop a store info
#define MOVE_INFO_KEY								"03"	// pop a move info
#define COLD_INFO_KEY								"04"	// pop a cold info
#define EXT_INFO_KEY								"05"	// pop a extrude info
#define REC_INFO_KEY								"06"	// pop a retract info
#define UNLOAD_INFO_KEY							"07"	// pop a unload info
//#define RESET_DONE_INFO_KEY					"08"	// pop a reset done info
//#define STORE_DONE_INFO_KEY					"09"	// pop a store done info
//#define SHUT_COLD_KEY								"0A"	// pop a window that shutting down with hot extruder
//#define ACCIDENT_KEY								"0B"	// pop a window that accident shutting down
#define CHANGE_FILAMENT_KEY					"0C"	// pop a filament change window
#define WAIT_KEY										"0D"	// pop a wait window
//#define FILE_FILE_KEY								"30"		// pop a print window
//#define FILE_DIR_KEY								"31"		// pop a open window
//#define EXT_INDEX_KEY_1							"10"	// pop a extruder chose window
//#define EXT_INDEX_KEY_2							"11"	// pop a extruder chose window
//#define EXT_INDEX_KEY_3							"12"	// pop a extruder chose window


// the DEMO addr that will init just MCU setup
/*
#define COLOR_BLACK										"0400"	//2 bytes
#define COLOR_WHITE										"0401"	//2 bytes
#define COLOR_RED											"0402"	//2 bytes
#define COLOR_ORANGE									"0403"	//2 bytes
#define COLOR_YELLOW									"0404"	//2 bytes
#define COLOR_GREEN										"0405"	//2 bytes
#define COLOR_CYAN										"0406"	//2 bytes
#define COLOR_BLUE										"0407"	//2 bytes
#define COLOR_PURPLE									"0408"	//2 bytes
#define COLOR_TEMP										"04FF"	//2 bytes
*/


#if HAS_READER
	#ifdef SDSUPPORT
		#define PAGE_NORAML_SERIAL							PAGE_NORAML_SERIAL_SD
		#define PAGE_NORMAL_PREHEAT_READER			PAGE_NORMAL_PREHEAT_SD
		#define PAGE_NORMAL_PREHEAT_NO_READER		PAGE_NORMAL_PREHEAT_NO_SD
		#define PAGE_NORMAL_COOLDOWN_READER			PAGE_NORMAL_COOLDOWN_SD
		#define PAGE_NORMAL_COOLDOWN_NO_READER	PAGE_NORMAL_COOLDOWN_NO_SD
		#define DWIN_MSG_READER_CONN						DWIN_MSG_SD_INSERTED
		#define DWIN_MSG_READER_READY						DWIN_MSG_SD_INSERTED
		#define DWIN_MSG_READER_REMOVED					DWIN_MSG_SD_REMOVED
	#elif defined(UDISKSUPPORT)
		#define PAGE_NORAML_SERIAL							PAGE_NORAML_SERIAL_USB
		#define PAGE_NORMAL_PREHEAT_READER			PAGE_NORMAL_PREHEAT_USB
		#define PAGE_NORMAL_PREHEAT_NO_READER		PAGE_NORMAL_PREHEAT_NO_USB
		#define PAGE_NORMAL_COOLDOWN_READER			PAGE_NORMAL_COOLDOWN_USB
		#define PAGE_NORMAL_COOLDOWN_NO_READER	PAGE_NORMAL_COOLDOWN_NO_USB
		#define DWIN_MSG_READER_CONN						DWIN_MSG_USB_INSERTED
		#define DWIN_MSG_READER_READY						DWIN_MSG_USB_READY
		#define DWIN_MSG_READER_REMOVED					DWIN_MSG_USB_REMOVED
	#endif
#else
	#define PAGE_NORAML_SERIAL							PAGE_NORAML_SERIAL_SD
	#define PAGE_NORMAL_PREHEAT_NO_READER		PAGE_NORMAL_PREHEAT_NO_SD
	#define PAGE_NORMAL_COOLDOWN_NO_READER	PAGE_NORMAL_COOLDOWN_NO_SD
#endif

#if EXTRUDERS > 2
	#define PAGE_STATE_NORMAL				PAGE_STATE_NORMAL_3
	#define PAGE_STATE_PRINT				PAGE_STATE_PRINT_3
	#define PAGE_FILAMENT						PAGE_FILAMENT_3
#elif EXTRUDERS > 1
	#ifdef HAS_HEATED_CHAMBER
		#define PAGE_STATE_NORMAL				PAGE_STATE_NORMAL_2_AIR
		#define PAGE_STATE_PRINT				PAGE_STATE_PRINT_2_AIR
	#else
		#define PAGE_STATE_NORMAL				PAGE_STATE_NORMAL_2
		#define PAGE_STATE_PRINT				PAGE_STATE_PRINT_2
	#endif
	#define PAGE_FILAMENT						PAGE_FILAMENT_2
#elif EXTRUDERS > 0
	#ifdef HAS_HEATED_CHAMBER
		#define PAGE_STATE_NORMAL				PAGE_STATE_NORMAL_1_AIR
		#define PAGE_STATE_PRINT				PAGE_STATE_PRINT_1_AIR
	#else
		#define PAGE_STATE_NORMAL				PAGE_STATE_NORMAL_1
		#define PAGE_STATE_PRINT				PAGE_STATE_PRINT_1
	#endif
	#define PAGE_FILAMENT						PAGE_FILAMENT_1
#endif




extern int lcd_preheat_hotend_temp[EXTRUDERS];
extern int lcd_preheat_bed_temp;
#ifdef HAS_HEATED_CHAMBER
	extern int lcd_preheat_chamber_temp;
#endif
extern int lcd_preheat_fan_speed;

extern bool cancel_heatup;

#ifdef POWER_MANAGEMENT
	extern char dwin_shutting_info[LCD_MSG_CHAR_LEN + 1];
#endif

#ifdef STATE_PAGE_AS_DEFAULT
void return_default_button_action(bool clickButton = false);
#else
void return_default_button_action();
#endif
void return_state_button_action();
void print_pause_button_action();
void print_reuse_button_action();
void print_cancel_button_action();
void jump_setting_2();
void jump_leveling_page();

enum FILE_UPDATE_MODE{ OPEN, NEXT_PAGE, LAST_PAGE, UP_LEVEL };
void dwinGetFileName(uint8_t fileItem = 0xFF);
void lcdUpdateFiles(uint8_t mode
#ifdef WIFI_SUPPORT
		, bool fromWifi = false
#endif
		);
void fileItemAciton(uint8_t index
#ifdef WIFI_SUPPORT
		, bool fromWifi = false
#endif
		);

//void setupDEMO();
void dwin_setup();
void dwin_run();

void dwin_update_msg(const char* msg);
void dwin_update_msg_P(const char* msg);

void dwin_change_page_P(const char* page);
bool dwin_is_page_P(const char* page);

#define DWIN_MSG(msg)							dwin_update_msg(msg)
#define DWIN_MSG_P(msg)						dwin_update_msg_P(PSTR(msg))

#define HAS_POPUP									(getPop() != POPUP_HIDDEN_KEY)
#define POP_WINDOW(key)						(setPop_P(PSTR(key)))
#define	HIDE_POPUP								(setPop(POPUP_HIDDEN_KEY))

#define DWIN_TOUCH								(touchState() == 1)


#define GO_PAGE(x)								dwin_change_page_P(PSTR(x))
#define DWIN_IS_PAGE(index)				(dwin_is_page_P(PSTR(index)))

#define IS_MOVE_PAGE							(DWIN_IS_PAGE(PAGE_MOVE) || DWIN_IS_PAGE(PAGE_FILAMENT))

#if HAS_READER
	#define IS_MENU_PAGE						(DWIN_IS_PAGE(PAGE_NORMAL_PREHEAT_READER) || DWIN_IS_PAGE(PAGE_NORMAL_PREHEAT_NO_READER) || \
																	 DWIN_IS_PAGE(PAGE_NORMAL_COOLDOWN_READER) || DWIN_IS_PAGE(PAGE_NORMAL_COOLDOWN_NO_READER))
#else
	#define IS_MENU_PAGE						(DWIN_IS_PAGE(PAGE_NORMAL_PREHEAT_NO_READER) || DWIN_IS_PAGE(PAGE_NORMAL_COOLDOWN_NO_READER))
#endif

#define IS_FILE_PAGE							(DWIN_IS_PAGE(PAGE_FILE_ONE) || DWIN_IS_PAGE(PAGE_FILE_NEXT) || DWIN_IS_PAGE(PAGE_FILE_BOTH) || DWIN_IS_PAGE(PAGE_FILE_LAST) || \
											 	 	 	 	 	 	 DWIN_IS_PAGE(PAGE_FILE_UP_ONE) || DWIN_IS_PAGE(PAGE_FILE_UP_NEXT) || DWIN_IS_PAGE(PAGE_FILE_UP_BOTH) || DWIN_IS_PAGE(PAGE_FILE_UP_LAST))
#define IS_STATE_PAGE							(DWIN_IS_PAGE(PAGE_STATE_NORMAL) || DWIN_IS_PAGE(PAGE_STATE_PRINT))
#define IS_WIFI_SETTING_PAGE			(DWIN_IS_PAGE(PAGE_SETTINGS_2_WIFI_OFF) || DWIN_IS_PAGE(PAGE_SETTINGS_2_WIFI_ON))
#define IS_SETTING_PAGE						(DWIN_IS_PAGE(PAGE_SETTINGS) || DWIN_IS_PAGE(PAGE_SETTINGS_2) || \
																	 DWIN_IS_PAGE(PAGE_SETTINGS_V2) || DWIN_IS_PAGE(PAGE_SETTINGS_V2_MORE) || \
																	 IS_WIFI_SETTING_PAGE)
#define IS_PRINT_DEFAULT_PAGE			(DWIN_IS_PAGE(PAGE_PRINT_PAUSE) || DWIN_IS_PAGE(PAGE_PRINT_RESUME))
#define IS_PRINT_PAGE							(IS_PRINT_DEFAULT_PAGE || DWIN_IS_PAGE(PAGE_STATE_PRINT))
#define IS_REG_PAGE								(DWIN_IS_PAGE(PAGE_REG) || DWIN_IS_PAGE(PAGE_REG_INFO))
#define IS_WIFI_INFO_PAGE					(DWIN_IS_PAGE(PAGE_WIFI_INFO))
#define IS_PROBE_DISABLE_PAGE			(DWIN_IS_PAGE(PAGE_LEVELING_DISABLE_INVALID) || DWIN_IS_PAGE(PAGE_LEVELING_DISABLE_OFF) || DWIN_IS_PAGE(PAGE_LEVELING_DISABLE_ON))
#define IS_PROBE_PAGE							(DWIN_IS_PAGE(PAGE_LEVELING_INVALID) || DWIN_IS_PAGE(PAGE_LEVELING_VALID_ON) || DWIN_IS_PAGE(PAGE_LEVELING_VALID_OFF))
#define IS_LEVELING_PAGE					(IS_PROBE_DISABLE_PAGE || IS_PROBE_PAGE)

#ifdef STATE_PAGE_AS_DEFAULT
	#define IS_DEFAULT_PAGE					(DWIN_IS_PAGE(PAGE_STATE_NORMAL) || IS_PRINT_DEFAULT_PAGE)
#else
	#define IS_DEFAULT_PAGE					(IS_MENU_PAGE || IS_PRINT_DEFAULT_PAGE)
#endif
#define IS_HOLDING_PAGE						(DWIN_IS_PAGE(PAGE_UNFINISH_CHOOSE) || DWIN_IS_PAGE(PAGE_SHUTDOWN_HOTEND) || DWIN_IS_PAGE(PAGE_WIFI_CONNECT) || DWIN_IS_PAGE(PAGE_REG))

#define IS_TIMEOUT_PAGE						(!(IS_DEFAULT_PAGE || IS_HOLDING_PAGE))
#define IS_TIMEOUT_PAGE_SERIAL		(!(DWIN_IS_PAGE(PAGE_STATE_NORMAL)))

#else	//!CREATBOT_LCD

#define DWIN_TOUCH 0
#define DWIN_MSG(msg)
#define DWIN_MSG_P(msg)

#endif //CREATBOT_LCD

#endif //DWIN_LCD_H
