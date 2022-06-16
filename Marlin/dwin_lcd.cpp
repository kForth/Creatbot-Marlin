/*
dwin_lcd.cpp - Hardware serial library for DWIN UART LCM.
Copyright (c) 2015 CreatBot. All Right Reserved.

Developer Lyn Lee, 3 June 2015.
*/


#include "dwin_lcd.h"
#ifdef DWIN_LCD
#include "duration_t.h"
#include "buzzer.h"
#include "language.h"
#include "temperature.h"
#include "planner.h"
#include "configuration_store.h"
#include "cardReader.h"
#include "UDiskReader.h"
#include "WiFiSocket.h"


int lcd_preheat_hotend_temp[EXTRUDERS];
int lcd_preheat_bed_temp;
#ifdef HEATED_CHAMBER
	int lcd_preheat_chamber_temp;
#endif
int lcd_preheat_fan_speed;

enum DATA_STATE {DATA_UNINIT, DATA_INIT, DATA_READY};
static uint8_t dwin_data_state = DATA_UNINIT;
#define IS_DWIN_DATA_READY		(dwin_data_state == DATA_READY)

static float dwin_manual_feedrate[] = MANUAL_FEEDRATE;
static uint8_t manual_move_axis = 0x00;
static millis_t manual_move_start_time = 0;
static millis_t return_default_page_time = 0;
static millis_t dwin_next_update_millis = 0;

static uint8_t move_distance_index = 0;
static uint8_t extrude_index = 0;
static int extrude_distance = 0;

static const char* dwin_next_page_P = nullptr;		// next dwin page from PSTR.


#if HAS_READER
static uint8_t dwinReaderState;
#endif

#ifdef WIFI_SUPPORT
static uint8_t dwinWifiConnIndex;
static char dwinWifiConnKey[LCD_MSG_CHAR_LEN + 1];
#endif

#ifdef POWER_MANAGEMENT
	char dwin_shutting_info[LCD_MSG_CHAR_LEN + 1] = {0};
#endif

enum DATA_MODE{ INT_LCD, LONG_LCD };
enum FAN_MODE{ GLOBAL_FAN, PREHEAT_FAN, TEMP_FAN, AIR_FAN };
//enum COLOR_MODE{ BLACK, WHITE, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE };
enum FILE_ACITON_MODE{ CANCEL, PRINT, OPEN_DIR };
enum WIFI_ACTION_MODE{ WIFI_ACTION_SWITCH_TOGGLE, WIFI_ACTION_CONNECT, WIFI_ACTION_CANCEL, WIFI_ACTION_SWITCH_MODE, WIFI_ACTION_RESET, WIFI_ACTION_SCAN, WIFI_ACTION_NEXT, WIFI_ACTION_LAST, WIFI_ACTION_JOIN};
enum HOME_MODE{ HOME_ALL, HOME_X, HOME_Y, HOME_Z };
enum TIME_MODE { TIME_USED, TIME_PRINT, TIME_TRIAL };

#define GET_INT(addr)									getValueForInt_P(addr)
#define SET_INT(addr, value)					setValueAsInt_P(addr, value)
#define GET_LONG(addr)								getValueForLong_P(addr)
#define SET_LONG(addr, value)					setValueAsLong_P(addr, value)
#define GET_STR(addr)									getValueForString_P(addr)
#define SET_STR(addr, len, value)			setValueAsString_P(addr, len, value);

#define GET_INT_P(addr)								getValueForInt_P(PSTR(addr))
#define SET_INT_P(addr, value)				setValueAsInt_P(PSTR(addr), PSTR(value))
#define GET_LONG_P(addr)							getValueForLong_P(PSTR(addr))
#define SET_LONG_P(addr, value)				setValueAsLong_P(PSTR(addr), PSTR(value))
#define GET_STR_P(addr)								getValueForString_P(PSTR(addr))
#define SET_STR_P(addr, len, value)		setValueAsString_P(PSTR(addr), len, value)

#define DRAW_REC(addr, x0, y0, x1, y1, color)		drawRectangle_P(PSTR(addr), x0, y0, x1, y1, PSTR(color))
#define FILL_REC(addr, x0, y0, x1, y1, color)		fillRectangle_P(PSTR(addr), x0, y0, x1, y1, PSTR(color))


void updateValue(float value, const char* valueAddr, uint8_t mode, uint8_t int_num, uint8_t dec_num){
	if (mode == INT_LCD)
		SET_INT(valueAddr, convToLCD(value, int_num, dec_num));
	else if (mode == LONG_LCD)
		SET_LONG(valueAddr, convToLCD(value, int_num, dec_num));
}

void updateValue(long value, const char* valueAddr){
	SET_LONG(valueAddr, value);
}

void updateValue(int value, const char* valueAddr) {
	SET_INT(valueAddr, value);
}

#define UPDATE_LCD(value, addr)				updateValue(value, PSTR(addr))
#define UPDATE_LCD_22(value, addr)		updateValue(value, PSTR(addr), INT_LCD, 2, 2)
#define UPDATE_LCD_31(value, addr)		updateValue(value, PSTR(addr), INT_LCD, 3, 1)
#define UPDATE_LCD_32(value, addr)		updateValue(value, PSTR(addr), INT_LCD, 3, 2)
#define UPDATE_LCD_34(value, addr)		updateValue(value, PSTR(addr), LONG_LCD, 3, 4)
#define UPDATE_LCD_42(value, addr)		updateValue(value, PSTR(addr), LONG_LCD, 4, 2)

/*
uint8_t tempColorMode;
void setColor(const char* addr, char* color){
	setValueAsAttr(addr, 3, color);
}

void setColor(const char* addr, uint8_t mode){
	switch (mode){
	case BLACK:		setColor(addr, "0000"); break;
	case WHITE:		setColor(addr, "FFFF"); break;
	case RED:		setColor(addr, "F800"); break;
	case ORANGE:	setColor(addr, "FC00"); break;
	case YELLOW:	setColor(addr, "FFE0"); break;
	case GREEN:		setColor(addr, "07E0"); break;
	case CYAN:		setColor(addr, "07FF"); break;
	case BLUE:		setColor(addr, "001F"); break;
	case PURPLE:	setColor(addr, "F81F"); break;
	default:		setColor(addr, "0000"); break;
	}
}

uint8_t getColor(const char* addr){
	unsigned char temp[2];
	char tempColor[5];
	getValueForAttr(temp, addr, 3, 1);
	charToHex(tempColor, temp, 2);
	if (!strcmp(tempColor, "0000")) return BLACK;
	else if (!strcmp(tempColor, "FFFF")) return WHITE;
	else if (!strcmp(tempColor, "F800")) return RED;
	else if (!strcmp(tempColor, "FC00")) return ORANGE;
	else if (!strcmp(tempColor, "FFE0")) return YELLOW;
	else if (!strcmp(tempColor, "07E0")) return GREEN;
	else if (!strcmp(tempColor, "07FF")) return CYAN;
	else if (!strcmp(tempColor, "001F")) return BLUE;
	else if (!strcmp(tempColor, "F81F")) return PURPLE;
	else								 return BLACK;
}

void resetColor(){
	setColor(FILE_ITEM_0_ATTR, BLACK);
	setColor(FILE_ITEM_1_ATTR, BLACK);
	setColor(FILE_ITEM_2_ATTR, BLACK);
	setColor(FILE_ITEM_3_ATTR, BLACK);
	setColor(FILE_ITEM_4_ATTR, BLACK);
	setColor(FILE_ITEM_5_ATTR, BLACK);
	setColor(FILE_ITEM_6_ATTR, BLACK);
	setColor(FILE_ITEM_7_ATTR, BLACK);
	setColor(FILE_ITEM_8_ATTR, BLACK);
}
*/

/******************************************** Funciton Part ********************************************/

void dwin_cooldwon(){
	thermalManager.disable_all_heaters();
#if FAN_COUNT > 0
	for (uint8_t i = 0; i < FAN_COUNT; i++) fanSpeeds[i] = 0;
#endif
	return_state_button_action();
}

void dwin_preheat(uint8_t target){

#if FAN_COUNT > 0
	#if FAN_COUNT > 1
		#define FAN_SPEED(x) fanSpeeds[active_extruder < FAN_COUNT ? active_extruder : 0] = x;
	#else
		#define FAN_SPEED(x) fanSpeeds[0] = x;
	#endif
#else
	#define FAN_SPEED(x) UNUSED(x);
#endif


#ifdef HEATED_CHAMBER
	#define DWIN_PREHEAT(endnum, temph, tempb, tempc, fan) do{\
		if(temph > 0) thermalManager.setTargetHotend(temph, endnum);\
		thermalManager.setTargetBed(tempb);\
		thermalManager.setTargetChamber(tempc);\
		FAN_SPEED(fan);\
	} while(0);
#else
	#define DWIN_PREHEAT(endnum, temph, tempb, fan) do{\
		if(temph > 0) thermalManager.setTargetHotend(temph, endnum);\
		thermalManager.setTargetBed(tempb);\
		FAN_SPEED(fan);\
	} while(0);
#endif

	if(target == 0xFF){						// Only Bed
		HOTEND_LOOP() thermalManager.setTargetHotend(0, e);
		DWIN_PREHEAT(
			0,
			0,
			lcd_preheat_bed_temp,
		#ifdef HEATED_CHAMBER
			0,
		#endif
			lcd_preheat_fan_speed
		);
	} else if(target == 0xFE){		// All of heater.
		HOTEND_LOOP() thermalManager.setTargetHotend(lcd_preheat_hotend_temp[e], e);

		DWIN_PREHEAT(
			0,
			0,
			lcd_preheat_bed_temp,
		#ifdef HEATED_CHAMBER
			lcd_preheat_chamber_temp,
		#endif
			lcd_preheat_fan_speed
		);
	} else if(EXTRUDERS > target){
		DWIN_PREHEAT(
			target,
			lcd_preheat_hotend_temp[target],
			lcd_preheat_bed_temp,
		#ifdef HEATED_CHAMBER
			lcd_preheat_chamber_temp,
		#endif
			lcd_preheat_fan_speed
		);
	}
	return_state_button_action();
}

void dwin_move_home(uint8_t mode){
	if (mode == HOME_ALL){
		enqueue_and_echo_commands_P(PSTR("G28"));
	} else if (mode == HOME_X){
		enqueue_and_echo_commands_P(PSTR("G28 X"));
	} else if (mode == HOME_Y){
		enqueue_and_echo_commands_P(PSTR("G28 Y"));
	} else if (mode == HOME_Z){
	#if ENABLED(Z_SAFE_HOMING)
		enqueue_and_echo_commands_P(PSTR("G28"));
	#else
		enqueue_and_echo_commands_P(PSTR("G28 Z"));
	#endif
	}

	POP_WINDOW(MOVE_INFO_KEY);
}

void dwin_update_axis_info();

void dwin_move(int axis, float scale, bool dir){

	float min = current_position[axis] - 1000,
				max = current_position[axis] + 1000;

	#if HAS_SOFTWARE_ENDSTOPS
		if (soft_endstops_enabled) {
			#if ENABLED(MIN_SOFTWARE_ENDSTOPS)
				min = soft_endstop_min[axis];
			#endif
			#if ENABLED(MAX_SOFTWARE_ENDSTOPS)
				max = soft_endstop_max[axis];
			#endif
		}
	#endif

	if (dir)
		current_position[axis] += scale;
	else
		current_position[axis] -= scale;

	NOLESS(current_position[axis], min);
	NOMORE(current_position[axis], max);

	dwin_update_axis_info();

	if(((axis == X_AXIS || axis == Y_AXIS) && scale == 10) || (axis == Z_AXIS && scale == 1)){
		planner.buffer_line_kinematic(current_position, MMM_TO_MMS(dwin_manual_feedrate[axis]), active_extruder);
		refresh_cmd_timeout();
	} else {
		manual_move_start_time = millis() + 300UL;
		SBI(manual_move_axis, axis);
	}
}

void dwin_move(int axis, bool dir){
	switch (move_distance_index){
	case 1:
		dwin_move(axis, 0.1, dir);
		break;
	case 2:
		dwin_move(axis, 1, dir);
		break;
	case 3:
		dwin_move(axis, 10, dir);
		break;
	case 4:
		dwin_move(axis, 100, dir);
		break;
	default:
		dwin_move(axis, 10, dir);
	}
}

void manual_move_delay(){
	if((manual_move_axis != 0) && ELAPSED(millis(), manual_move_start_time) && !planner.is_full()) {
		float manual_feedrate = 100000;

		LOOP_XYZE(i){
			if(TEST(manual_move_axis, i))
				manual_feedrate = min(manual_feedrate, dwin_manual_feedrate[i]);
		}

		if(manual_feedrate < 60000){
			planner.buffer_line_kinematic(current_position, MMM_TO_MMS(manual_feedrate), active_extruder);
	    refresh_cmd_timeout();
		}
		manual_move_axis = 0;
	}
}

void dwin_filament_test(int hotend, bool dir){
	if (hotend == 0){
		active_extruder = 0;
	} else if (hotend == 1){
	#if EXTRUDERS > 1
		active_extruder = 1;
	#endif
	} else if (hotend == 2){
	#if EXTRUDERS > 2
		active_extruder = 2;
	#endif
	}

	if (min(thermalManager.degTargetHotend(active_extruder), thermalManager.degHotend(active_extruder)) < EXTRUDE_MINTEMP) {
		POP_WINDOW(COLD_INFO_KEY);
		delay(500);
		HIDE_POPUP;
		return;
	}

	float testLength = float(extrude_distance) * (dir ? 1 : -1);
	current_position[E_AXIS] += testLength;

	planner.buffer_line_kinematic(current_position, MMM_TO_MMS(dwin_manual_feedrate[E_AXIS]), active_extruder);

	if (dir) POP_WINDOW(EXT_INFO_KEY);
	else POP_WINDOW(REC_INFO_KEY);
}

void dwin_filament_test(bool dir){
	switch (extrude_index){
	case 1:
	case 2:
	case 4:
		dwin_filament_test(0, dir);
		break;
	case 3:
	case 5:
		dwin_filament_test(1, dir);
		break;
	case 6:
		dwin_filament_test(2, dir);
		break;
	default:
		dwin_filament_test(0, dir);
		break;
	}
}

void dwin_filament_unload(int hotend){
	if (hotend == 0){
		active_extruder = 0;
	} else if (hotend == 1){
	#if EXTRUDERS > 1
		active_extruder = 1;
	#endif
	} else if (hotend == 2){
	#if EXTRUDERS > 2
		active_extruder = 2;
	#endif
	}

	if (min(thermalManager.degTargetHotend(active_extruder), thermalManager.degHotend(active_extruder)) < EXTRUDE_MINTEMP) {
		POP_WINDOW(COLD_INFO_KEY);
		delay(500);		// TODO
		HIDE_POPUP;
		return;
	}

	current_position[E_AXIS] += FILAMENT_UNLOAD_EXTRUDER_LENGTH;
	planner.buffer_line_kinematic(current_position, MMM_TO_MMS(dwin_manual_feedrate[E_AXIS]), active_extruder);
	enqueue_and_echo_commands_P(PSTR("M6003"));
	POP_WINDOW(UNLOAD_INFO_KEY);
}

void dwin_filament_unload(){
	switch (extrude_index){
	case 1:
	case 2:
	case 4:
		dwin_filament_unload(0);
		break;
	case 3:
	case 5:
		dwin_filament_unload(1);
		break;
	case 6:
		dwin_filament_unload(2);
		break;
	default:
		dwin_filament_unload(0);
		break;
	}
}


void centerString(char* str, uint8_t len) {
	uint8_t i = strlen(str);
	if (i >= len) return;

	uint8_t j, space; char raw[LCD_FILE_CHAR_LEN];
	strcpy(raw, str);
	space = (len - i) / 2;
	for (i = 0; space--;)	str[i++] = ' ';
	for (j = 0; raw[j];)	str[i++] = raw[j++];
	str[i] = 0;
}

#if HAS_READER
uint16_t page_num = 0;
uint16_t file_page = 1;
uint16_t up_file_page = 1;
uint16_t file_item_num = 0;
uint8_t file_item_selected = -1;

void dwinGetFileName(uint8_t fileItem){
	char name[FILE_NAME_LEN + 1]; char *c, *cc; uint8_t i, end_index; int16_t file_item_index;
#ifdef WIFI_SUPPORT
	char wifi_name[FILE_NAME_LEN + 3]; char *wifi_c; uint8_t wifi_end_index;
#endif


#ifdef WIFI_SUPPORT
	#define MEMSET_NAME do{\
			memset(name, 0, sizeof(name));\
			memset(wifi_name, 0, sizeof(wifi_name));\
		}while(0)
	#define INIT_NAME do{\
			end_index = (sizeof(name) - (CUR_FILE_IS_DIR ? 2 : 1));\
			wifi_end_index = (sizeof(wifi_name) - 3);\
			if(CUR_FILE_IS_DIR){\
				name[0] = '['; c = &name[1];\
				wifi_name[0] = '<'; wifi_c = &wifi_name[1];\
			} else {\
				c = name;\
				wifi_name[0] = ':'; wifi_c = &wifi_name[1];\
			}\
		}while(0)
	#define SET_NAME do{\
			while(cc[i]){\
				if (i > 5){\
					if(i >= end_index){\
						if(i == end_index){\
							name[i - 1] = name[i - 2] = name[i - 3] = '.';\
						}\
					} else {\
						*(c++) = cc[i];\
					}\
					if(i >= wifi_end_index){\
						if(i == wifi_end_index){\
							wifi_name[i] = wifi_name[i - 1] = wifi_name[i - 2] = '.';\
						}\
					} else {\
						*(wifi_c++) = cc[i];\
					}\
					if(i == (end_index > wifi_end_index ? end_index : wifi_end_index)){\
						break;\
					} else {\
						i++;\
					}\
				} else {\
					*(c++) = cc[i];\
					*(wifi_c++) = cc[i];\
					i++;\
				}\
			}\
		}while(0)
	#define END_NAME do{\
			if (CUR_FILE_IS_DIR){\
				if(i < end_index){\
					name[i + 1] = ']';\
				} else {\
					name[end_index] = ']';\
				}\
			}\
			if(i < wifi_end_index){\
				wifi_name[i + 1] = (CUR_FILE_IS_DIR ? '>' : ':');\
			} else {\
				wifi_name[wifi_end_index + 1] = (CUR_FILE_IS_DIR ? '>' : ':');\
			}\
		}while(0)
#else
	#define MEMSET_NAME		memset(name, 0, sizeof(name))
	#define INIT_NAME do{\
			end_index = (sizeof(name) - (CUR_FILE_IS_DIR ? 2 : 1));\
			if(CUR_FILE_IS_DIR){\
				name[0] = '['; c = &name[1];\
			} else {\
				c = name;\
			}\
		}while(0)
	#define SET_NAME do{\
			while(cc[i]){\
				if (i > 5){\
					if(i == end_index){\
						name[i - 1] = name[i - 2] = name[i - 3] = '.';\
						break;\
					}\
				}\
				*(c++) = cc[i++];\
			}\
		}while(0)
	#define END_NAME do{\
			if (CUR_FILE_IS_DIR){\
				if(i < end_index){\
					name[i + 1] = ']';\
				} else {\
					name[end_index] = ']';\
				}\
			}\
		}while(0)
#endif

#define GET_ITEM(index) do {\
		file_item_index = (file_item_num - 1) - (file_page - 1) * LCD_FILE_ITEM_PER_PAGE - index;\
		MEMSET_NAME;\
		\
		if(file_item_index >= 0){\
			TOUCH_FILE(file_item_index);\
			INIT_NAME;\
			i = 0;\
			if(FILE_READER.longFilename[0])\
				cc = FILE_READER.longFilename;\
			else\
				cc = FILE_READER.filename;\
			\
			SET_NAME;\
			END_NAME;\
		}\
	}while(0)

#define _SET_FILE_ITEM(item, index) do {\
		GET_ITEM(index);\
		SET_STR(item, LCD_FILE_CHAR_LEN, name);\
	}while (0)

#define SET_FILE_ITEM(item, index)	_SET_FILE_ITEM(PSTR(item), index)

	if(fileItem == 0xFF){
		SET_FILE_ITEM(FILE_ITEM_0, 0);
	#if LCD_FILE_ITEM_PER_PAGE > 1
		SET_FILE_ITEM(FILE_ITEM_1, 1);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 2
		SET_FILE_ITEM(FILE_ITEM_2, 2);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 3
		SET_FILE_ITEM(FILE_ITEM_3, 3);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 4
		SET_FILE_ITEM(FILE_ITEM_4, 4);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 5
		SET_FILE_ITEM(FILE_ITEM_5, 5);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 6
		SET_FILE_ITEM(FILE_ITEM_6, 6);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 7
		SET_FILE_ITEM(FILE_ITEM_7, 7);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 8
		SET_FILE_ITEM(FILE_ITEM_8, 8);
	#endif
	}else{
		GET_ITEM(fileItem);
	#ifdef WIFI_SUPPORT
		if(file_item_index < 0){
			if(file_item_index == -1){
				myWifi.synFilesInfo(fileItem, "", true);
			} else if (file_item_index == -2){
				TOUCH_WORKDIR;
				myWifi.endSysFile((FILE_READER.filename[0] == '/'), page_num, file_page);
			}
		} else {
			if(fileItem < LCD_FILE_ITEM_PER_PAGE){
				myWifi.synFilesInfo(fileItem, wifi_name, (fileItem == (LCD_FILE_ITEM_PER_PAGE - 1)));
			}
			else if(fileItem == LCD_FILE_ITEM_PER_PAGE){
				TOUCH_WORKDIR;
				myWifi.endSysFile((FILE_READER.filename[0] == '/'), page_num, file_page);
			}
		}
	#endif
	}
}

void lcdUpdateFiles(uint8_t mode
#ifdef WIFI_SUPPORT
		, bool fromWifi
#endif
		){
	if (mode == UP_LEVEL) {
		file_page = up_file_page;
		up_file_page = 1;
		file_item_num = 0;
		file_item_selected = -1;
		FILE_READER.updir();
	}

	file_item_num =	GET_FILE_NR;

	page_num = (file_item_num + LCD_FILE_ITEM_PER_PAGE - 1) / LCD_FILE_ITEM_PER_PAGE;

	if (mode == LAST_PAGE && file_page > 1)	file_page--;
	if (mode == NEXT_PAGE && file_page < page_num) file_page++;
#ifdef WIFI_SUPPORT
	if(!fromWifi || IS_FILE_PAGE){
#endif
		dwinGetFileName();
		if (!page_num) {
			char name[LCD_FILE_CHAR_LEN];
			strcpy_P(name, PSTR(DWIN_MSG_NO_FILE));
			centerString(name, LCD_FILE_CHAR_LEN);
			SET_STR_P(FILE_ITEM_CENTER, LCD_FILE_CHAR_LEN, name);
		}

		TOUCH_WORKDIR;
		if (FILE_READER.filename[0] == '/') {
			if (page_num <= 1) { GO_PAGE(PAGE_FILE_ONE); }
			else {
				if (file_page == 1) GO_PAGE(PAGE_FILE_NEXT);
				if (file_page == page_num) GO_PAGE(PAGE_FILE_LAST);
				if (file_page > 1 && file_page < page_num) GO_PAGE(PAGE_FILE_BOTH);
			}
		}
		else {
			if (page_num <= 1) { GO_PAGE(PAGE_FILE_UP_ONE); }
			else {
				if (file_page == 1) GO_PAGE(PAGE_FILE_UP_NEXT);
				if (file_page == page_num) GO_PAGE(PAGE_FILE_UP_LAST);
				if (file_page > 1 && file_page < page_num) GO_PAGE(PAGE_FILE_UP_BOTH);
			}
		}
#ifdef WIFI_SUPPORT
	}
	myWifi.actionSynFilesInfo(true);
#endif
}

void afterFileItemAction(uint8_t mode
#ifdef WIFI_SUPPORT
		, bool fromWifi = false
#endif
		){
	if (mode) {
		int16_t file_item_index = (file_item_num - 1) - (file_page - 1) * LCD_FILE_ITEM_PER_PAGE - file_item_selected;
		TOUCH_FILE(file_item_index);

		if (mode == PRINT) {
			FILE_READER.openAndPrintFile(FILE_READER.filename);
			return_default_button_action();
		}
		else if (mode == OPEN_DIR) {
			FILE_READER.chdir(FILE_READER.filename);
			up_file_page = file_page;
			file_page = 1;
			file_item_num = 0;
			file_item_selected = -1;
			lcdUpdateFiles(OPEN
			#ifdef WIFI_SUPPORT
				, fromWifi
			#endif
			);
		}
	} else {
		file_item_selected = -1;
		lcdUpdateFiles(OPEN
		#ifdef WIFI_SUPPORT
			, fromWifi
		#endif
		);
	}
}

void fileItemAciton(uint8_t index
#ifdef WIFI_SUPPORT
		, bool fromWifi
#endif
		){
	file_item_selected = index;
	int16_t file_item_index = (file_item_num - 1) - (file_page - 1) * LCD_FILE_ITEM_PER_PAGE - index;
	TOUCH_FILE(file_item_index);

	if (file_item_index >= 0){
		if (CUR_FILE_IS_DIR) {
			afterFileItemAction(OPEN_DIR
			#ifdef WIFI_SUPPORT
					, fromWifi
			#endif
					);
		} else {
		#ifdef WIFI_SUPPORT
			if(fromWifi)
				afterFileItemAction(PRINT);
			else
		#endif
			{
				// get the string value that chosen.
				switch (file_item_selected) {
				case 0:
					GET_STR_P(FILE_ITEM_0);
					break;
				case 1:
					GET_STR_P(FILE_ITEM_1);
					break;
				case 2:
					GET_STR_P(FILE_ITEM_2);
					break;
				case 3:
					GET_STR_P(FILE_ITEM_3);
					break;
				case 4:
					GET_STR_P(FILE_ITEM_4);
					break;
				case 5:
					GET_STR_P(FILE_ITEM_5);
					break;
				case 6:
					GET_STR_P(FILE_ITEM_6);
					break;
				case 7:
					GET_STR_P(FILE_ITEM_7);
					break;
				case 8:
					GET_STR_P(FILE_ITEM_8);
					break;
				}
				GO_PAGE(PAGE_PRINT_CONFIRM);
			}
		}
	}
}
#else		// !HAS_READER
void lcdUpdateFiles(uint8_t mode) { UNUSED(mode); };
void afterFileItemAction(uint8_t mode) { UNUSED(mode); };
void fileItemAciton(uint8_t index) { UNUSED(index); };
#endif	//HAS_READER

#ifdef WIFI_SUPPORT
void wifiItemAction(uint8_t index){
	char name[WIFI_SSID_SIZE + 1] = {0};
	strcpy(name, myWifi.getWifiInfo(index).ssid);

	if(name[0]){
		dwinWifiConnIndex = index;

		// show the string in the center of LCD.
		centerString(name, LCD_MSG_CHAR_LEN);
		SET_STR_P(WIFI_SELECT, LCD_MSG_CHAR_LEN, name);

		GO_PAGE(PAGE_WIFI_CONNECT);
	}
}

void wifiFuncAction(uint8_t mode){
	switch (mode){
	case WIFI_ACTION_SWITCH_TOGGLE:
		myWifi.ON_OFF(!myWifi.enable);
		break;
	case WIFI_ACTION_CONNECT:
		GET_STR_P(WIFI_KEY_ADDR);
		GO_PAGE(PAGE_WIFI_INFO);
		break;
	case WIFI_ACTION_CANCEL:
		GO_PAGE(PAGE_WIFI_INFO);
		break;
	case WIFI_ACTION_SWITCH_MODE:
		myWifi.actionSwitchWifiMode();
		break;
	case WIFI_ACTION_RESET:
		myWifi.actionWifiReset();
		break;
	case WIFI_ACTION_SCAN:
		myWifi.actionScanAP(SCAN_0);
		break;
	case WIFI_ACTION_NEXT:
		myWifi.actionScanAP(SCAN_NEXT);
		break;
	case WIFI_ACTION_LAST:
		myWifi.actionScanAP(SCAN_LAST);
		break;
	case WIFI_ACTION_JOIN:
		myWifi.actionJoinAP(dwinWifiConnIndex, dwinWifiConnKey);
		break;
	default:
		break;
	}
}
#endif	// WIFI_SUPPORT

#ifdef STATE_PAGE_AS_DEFAULT
void return_default_button_action(bool clickButton){
#else
void return_default_button_action(){
#endif
#if HAS_READER
	if(isSerialPrinting)
		GO_PAGE(PAGE_NORAML_SERIAL);
	else
		if (!FILE_IS_IDLE)
			if (FILE_IS_PRINT)
				GO_PAGE(PAGE_PRINT_PAUSE);
			else
				GO_PAGE(PAGE_PRINT_RESUME);
		else
	#ifdef STATE_PAGE_AS_DEFAULT
			if(!clickButton){
				GO_PAGE(PAGE_STATE_NORMAL);
			} else
	#endif
			{
			if (thermalManager.hasHeat())
				if (READER_VALID)
					GO_PAGE(PAGE_NORMAL_COOLDOWN_READER);
				else
					GO_PAGE(PAGE_NORMAL_COOLDOWN_NO_READER);
			else
				if (READER_VALID)
					GO_PAGE(PAGE_NORMAL_PREHEAT_READER);
				else
					GO_PAGE(PAGE_NORMAL_PREHEAT_NO_READER);
			}
#else		// !HAS_READER
	if(isSerialPrinting)
		GO_PAGE(PAGE_NORAML_SERIAL);
	else
		if(thermalManager.hasHeat())
			GO_PAGE(PAGE_NORMAL_COOLDOWN_NO_READER);
		else
			GO_PAGE(PAGE_NORMAL_PREHEAT_NO_READER);
#endif		// HAS_READER
}

void return_state_button_action(){
#if HAS_READER
	if (FILE_IS_IDLE)
		GO_PAGE(PAGE_STATE_NORMAL);
	else
		GO_PAGE(PAGE_STATE_PRINT);
#else
	GO_PAGE(PAGE_STATE_NORMAL);
#endif
}


void jump_settting(){
#if defined(WIFI_SUPPORT) || HAS_LEVELING
	#ifdef WIFI_SUPPORT
		GO_PAGE(PAGE_SETTINGS);
	#else
		GO_PAGE(PAGE_SETTINGS_V2);
	#endif
#else
	GO_PAGE(PAGE_SETTINGS);
#endif
}

void jump_setting_2(){
#if defined(WIFI_SUPPORT) || HAS_LEVELING
	#ifdef WIFI_SUPPORT
		if(myWifi.enable)
			GO_PAGE(PAGE_SETTINGS_2_WIFI_ON);
		else
			GO_PAGE(PAGE_SETTINGS_2_WIFI_OFF);
	#else
		GO_PAGE(PAGE_SETTINGS_V2_MORE);
	#endif
#else
	GO_PAGE(PAGE_SETTINGS_2);
#endif
}

#if HAS_LEVELING
void jump_leveling_page(){
#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
	if(FILE_IS_IDLE && !planner.blocks_queued() && !isSerialPrinting){
		if(leveling_is_valid()){
			if(leveling_is_active()){
				GO_PAGE(PAGE_LEVELING_VALID_ON);
			} else{
				GO_PAGE(PAGE_LEVELING_VALID_OFF);
			}
		} else{
			GO_PAGE(PAGE_LEVELING_INVALID);
		}
	} else{
		if(leveling_is_valid()){
			if(leveling_is_active()){
				GO_PAGE(PAGE_LEVELING_DISABLE_ON);
			} else{
				GO_PAGE(PAGE_LEVELING_DISABLE_OFF);
			}
		} else{
			GO_PAGE(PAGE_LEVELING_DISABLE_INVALID);
		}
	}
#else
	// TODO Other leveling
#endif
}

#if HAS_BED_PROBE
void leveling_probe(){
	if(FILE_IS_IDLE && !planner.blocks_queued() && !isSerialPrinting){
  	probeDone = false;
  	enqueue_and_echo_commands_P(PSTR("G28\nG29"));
		jump_leveling_page();
		POP_WINDOW(WAIT_KEY);
	}
}
#else
void leveling_probe(){};
#endif

void leveling_toggle(){
	if(leveling_is_valid()){
		bool allowLeveling = leveling_is_active();
		set_bed_leveling_enabled(!allowLeveling);
		EEPROM_STORE(planner.abl_enabled, setting_mesh_bilinear_ubl_status);
		jump_leveling_page();
	}
}
#else
void jump_leveling_page(){};
void leveling_probe(){};
void leveling_toggle(){};
#endif


void preheat_button_action(){
	dwin_preheat(0);
}

#if HAS_READER
void print_pause_button_action() {
#if ENABLED(QUICK_PAUSE)
	if(quickPausePrintJob()){
		DWIN_MSG_P(DWIN_MSG_PRINT_PAUSED);
		return_default_button_action();
	}
#else
	FILE_PAUSE_PRINT;
  print_job_timer.pause();
  #if ENABLED(PARK_HEAD_ON_PAUSE)
    enqueue_and_echo_commands_P(PSTR("M125"));
  #endif
	DWIN_MSG_P(DWIN_MSG_PRINT_PAUSED);
#endif //QUICK_PAUSE
}

void print_reuse_button_action() {
#if ENABLED(QUICK_PAUSE)
	if(quickReusePrintJob()){
		DWIN_MSG_P(DWIN_MSG_PRINTING);
		return_default_button_action();
	}
#else
  #if ENABLED(PARK_HEAD_ON_PAUSE)
    enqueue_and_echo_commands_P(PSTR("M24"));
  #else
    FILE_START_PRINT;
    print_job_timer.start();
  #endif
    return_default_button_action();
#endif //QUICK_PAUSE
}

void print_cancel_button_action() {
#if ENABLED(QUICK_PAUSE)
	quickStopPrintJob();
	DWIN_MSG_P(DWIN_MSG_WELCOME);
	return_default_button_action();
#else
	FILE_STOP_PRINT;
  clear_command_queue();
  quickstop_stepper();
  print_job_timer.stop();
  thermalManager.disable_all_heaters();
  #if FAN_COUNT > 0
    for (uint8_t i = 0; i < FAN_COUNT; i++) fanSpeeds[i] = 0;
  #endif
  wait_for_heatup = false;
	DWIN_MSG_P(DWIN_MSG_WELCOME);
	return_default_button_action();
#endif //QUICK_PAUSE
}
#else		//!HAS_READER
void print_pause_button_action() {}
void print_reuse_button_action() {}
void print_cancel_button_action() {}
#endif	//HAS_READER

#ifdef FILAMENT_CHANGE
void print_change_button_action() {
	if (!HAS_POPUP
#ifdef ACCIDENT_DETECT
	&& !isAccidentToPrinting
#endif
	) {
		if(pauseToUnloadFilament()){
			POP_WINDOW(WAIT_KEY);
		}
	}
}
#else
void print_change_button_action() {}
#endif

void open_filament_page_action(){
	GO_PAGE(PAGE_FILAMENT);
}


void store_setting_action(){
	POP_WINDOW(STORE_INFO_KEY);
#ifdef DEBUG_FREE
	SERIAL_ECHOLNPAIR("before save: ", freeMemory());
#endif
	settings.save();
#ifdef DEBUG_FREE
	SERIAL_ECHOLNPAIR("after save: ", freeMemory());
#endif
	HIDE_POPUP;
}

void reset_setting_action(){
	POP_WINDOW(RESET_INFO_KEY);
	settings.reset();
	HIDE_POPUP;
}

void jump_reg_action(){
#ifdef REG_SN
	GO_PAGE(PAGE_REG_INFO);
#endif
}

void shuttingCancelAction(){
#if FAN_COUNT > 0
	for (uint8_t i = 0; i < FAN_COUNT; i++) fanSpeeds[i] = 0;
#endif
	return_default_button_action();
}

#ifdef ACCIDENT_DETECT

void accidentReuseAction(){
	//return_default_button_action();
	accidentToResume_Home();
}

void accidentCancelAction(){
	return_default_button_action();
	if (isAccident) {
		isAccident = false;
		STORE_SETTING(isAccident);
	}
}	
#else
void accidentReuseAction() {};
void accidentCancelAction() {};
#endif

#ifdef FILAMENT_CHANGE
void changeDoneAction() {
	HIDE_POPUP;		// dummy hide popup, as the popup is disappear by touch the "resume" button in dwin lcd.
	print_reuse_button_action();
};
#else
void changeDoneAction() {};
#endif

/*******************************************************************************************************/

/*
void setupDEMO(){
	SET_INT_P(COLOR_BLACK,	"0000");
	SET_INT_P(COLOR_WHITE,	"FFFF");
	SET_INT_P(COLOR_RED,		"F800");
	SET_INT_P(COLOR_ORANGE,	"FC00");
	SET_INT_P(COLOR_YELLOW,	"FFE0");
	SET_INT_P(COLOR_GREEN,	"07E0");
	SET_INT_P(COLOR_CYAN,		"07FF");
	SET_INT_P(COLOR_BLUE,		"001F");
	SET_INT_P(COLOR_PURPLE,	"F81F");
}
*/

/********************************************* View Part ***********************************************/

void dwin_update_msg(const char* msg) {
  if(powerState <= POWER_COOLING) return;
	// center the str.
	//char msg_ram[30] = { 0 };
	//strcpy(msg_ram, msg);
	//centerString(msg_ram, 30);
	//SET_STR_P(MSG_ADDR, LCD_MSG_CHAR_LEN, msg_ram);
	SET_STR_P(MSG_ADDR, LCD_MSG_CHAR_LEN, msg);

}

void dwin_update_msg_P(const char* msg){
	char real_msg[LCD_MSG_CHAR_LEN + 1] = { 0 };
	strncpy_P(real_msg, msg, LCD_MSG_CHAR_LEN);
	dwin_update_msg(real_msg);
//	SERIAL_ECHO("dwin:	");
//	SERIAL_ECHOLN(real_msg);
}

void updateFanSpeed(uint8_t mode) {
#define _FAN_SPEED_UPDATE(fan, addr) do{\
			SET_INT(addr, (fan + 1) * 100 / 255);\
		} while(0);
#define FAN_SPEED_UPDATE(speed, addr)	_FAN_SPEED_UPDATE(speed, PSTR(addr))


	if (mode == GLOBAL_FAN) {
		int fanSpeed =
		#if FAN_COUNT > 0
			#if FAN_COUNT > 1
				fanSpeeds[active_extruder < FAN_COUNT ? active_extruder : 0];
			#else
				fanSpeeds[0];
			#endif
		#else
				0;
		#endif
		FAN_SPEED_UPDATE(fanSpeed, FAN_SPEED_ADDR);
	}
	else if (mode == PREHEAT_FAN) {
		FAN_SPEED_UPDATE(lcd_preheat_fan_speed, PREHEAT_FAN_SPEED_ADDR);
	}
	else if (mode == TEMP_FAN) {
		FAN_SPEED_UPDATE(extruder_auto_fan_speed, TEMP_FAN_SPEED_ADDR);
	}
	else if (mode == AIR_FAN) {
#ifdef CHAMBER_FAN
		FAN_SPEED_UPDATE(air_fan_speed, AIR_FAN_SPEED_ADDR);
#endif
	}
}

void dwin_update_version(){
	char *ver = &versionFW[1];
	SET_STR_P(VER_ADDR, LCD_VER_CHAR_LEN, ver);
}

void dwin_update_icon() {
#ifdef REG_SN
	uint8_t regState;
	regState = REG_PASS ? 1 : 0;
	UPDATE_LCD(regState, REG_STATE_INDEX_ADDR);
#else
	UPDATE_LCD(1, REG_STATE_INDEX_ADDR);
#endif
#ifdef WIFI_SUPPORT
	uint8_t wifi_mode, wifi_state;
	if (myWifi.getWifiMode() == MODE_BOTH) {
		wifi_mode = 0;
	} else wifi_mode = 1;

	if (myWifi.getIP()) {
		wifi_state = 0;
	} else {
		if (myWifi.isAction(ACTION_JOIN_AP))
			wifi_state = 1;
		else
			wifi_state = 2;
	}

	UPDATE_LCD(wifi_mode, WIFI_MODE_INDEX_ADDR);
	UPDATE_LCD(wifi_state, WIFI_STATE_INDEX_ADDR);
#endif 
}

void dwin_update_index() {
	move_distance_index = 3;
#if EXTRUDERS > 2
	extrude_index = 4;
#elif EXTRUDERS > 1
	extrude_index = 2;
#elif EXTRUDERS > 0
	extrude_index = 1;
#endif
	extrude_distance = 10;

	UPDATE_LCD(move_distance_index, MOVE_DISTANCE_INDEX_ADDR);
	UPDATE_LCD(extrude_index, FILAMENT_EXT_INDEX_ADDR);
	UPDATE_LCD(extrude_distance, FILAMENT_EXT_DISTANCE_ADDR);

	dwin_update_icon();
}

void dwin_update_state_info() {
	//target_temperature
	UPDATE_LCD(thermalManager.degTargetHotend(0), EX0_TAR_ADDR);
#if EXTRUDERS > 1
	UPDATE_LCD(thermalManager.degTargetHotend(1), EX1_TAR_ADDR);
#endif
#if EXTRUDERS > 2
	UPDATE_LCD(thermalManager.degTargetHotend(2), EX2_TAR_ADDR);
#endif
	UPDATE_LCD(thermalManager.degTargetBed(), BED_TAR_ADDR);
#ifdef HEATED_CHAMBER
	UPDATE_LCD(thermalManager.degTargetChamber(), CHAMBER_TAR_ADDR);
#endif

	//current_temperature
	UPDATE_LCD_31(thermalManager.degHotend(0), EX0_CUR_ADDR);
#if EXTRUDERS > 1
	UPDATE_LCD_31(thermalManager.degHotend(1), EX1_CUR_ADDR);
#endif
#if EXTRUDERS > 2
	UPDATE_LCD_31(thermalManager.degHotend(2), EX2_CUR_ADDR);
#endif
	UPDATE_LCD_31(thermalManager.degBed(), BED_CUR_ADDR);
#ifdef HEATED_CHAMBER
	UPDATE_LCD_31(thermalManager.degChamber(), CHAMBER_CUR_ADDR);
#endif

	//global_speed  & fan_speed & extrude_multiply
	UPDATE_LCD(feedrate_percentage, SPEED_GLOBAL_ADDR);
	UPDATE_LCD(flow_percentage[active_extruder], EXT_MULTIPLY_ADDR);
	updateFanSpeed(GLOBAL_FAN);
}

void dwin_update_axis_info() {
	//current_position
	UPDATE_LCD_42(current_position[X_AXIS], X_AXIS_ADDR);
	UPDATE_LCD_42(current_position[Y_AXIS], Y_AXIS_ADDR);
	UPDATE_LCD_42(current_position[Z_AXIS], Z_AXIS_ADDR);
}

void dwin_update_max_temp_info() {
	//current_max_temperature
	UPDATE_LCD_31(thermalManager.maxDegHotend(), CUR_MAX_TEMP_ADDR);
}

void dwin_update_setting_info() {
	//machine_setting
	updateFanSpeed(PREHEAT_FAN);
	UPDATE_LCD(lcd_preheat_hotend_temp[0], PREHEAT_EXT_TEMP_ADDR_0);
#if EXTRUDERS > 1
	UPDATE_LCD(lcd_preheat_hotend_temp[1], PREHEAT_EXT_TEMP_ADDR_1);
#endif
#if EXTRUDERS > 2
	UPDATE_LCD(lcd_preheat_hotend_temp[2], PREHEAT_EXT_TEMP_ADDR_2);
#endif
	UPDATE_LCD(lcd_preheat_bed_temp, PREHEAT_BED_TEMP_ADDR);
#ifdef HEATED_CHAMBER
	UPDATE_LCD(lcd_preheat_chamber_temp, PREHEAT_CHAMBER_TEMP_ADDR);
#endif

	UPDATE_LCD_34(planner.axis_steps_per_mm[X_AXIS], X_STEP_ADDR);
	UPDATE_LCD_34(planner.axis_steps_per_mm[Y_AXIS], Y_STEP_ADDR);
	UPDATE_LCD_34(planner.axis_steps_per_mm[Z_AXIS], Z_STEP_ADDR);
	UPDATE_LCD_34(planner.axis_steps_per_mm[E_AXIS], E_STEP_ADDR);

	updateFanSpeed(TEMP_FAN);
	updateFanSpeed(AIR_FAN);
}

#ifdef WIFI_SUPPORT
void dwin_update_wifi_info() {
	// wifi settings
	dwin_update_icon();

	char ip_str[16] = {0};
	uint32_t ip_num = myWifi.getIP();

	if(ip_num){
		sprintf_P(ip_str, PSTR("%d.%d.%d.%d"), (int(ip_num>>24)) & 0x00FF, (int(ip_num>>16)) & 0x00FF, (int(ip_num>>8)) & 0x00FF, (int(ip_num)) & 0x00FF);
	} else {
		strcpy_P(ip_str, PSTR("000.000.000.000"));
	}

	SET_STR_P(WIFI_IP_ADDR, 32, ip_str);
	SET_STR_P(WIFI_SSID_ADDR, LCD_HOST_CHAR_LEN, hostName);
	
	#define WIFI_INFO(index) do{\
		SET_STR_P(WIFI_ITEM_STR_ ## index, LCD_WIFI_CHAR_LEN, myWifi.getWifiInfo(index).ssid);\
		UPDATE_LCD(myWifi.getWifiInfo(index).level, WIFI_ITEM_ICO_ ## index);\
	}while(0);

	WIFI_INFO(0);
	WIFI_INFO(1);
	WIFI_INFO(2);
	WIFI_INFO(3);
	WIFI_INFO(4);

}
#endif

#if HAS_LEVELING
void dwin_update_leveling_info(){
	#if HAS_BED_PROBE
//		float offset = zprobe_zoffset;
//		UPDATE_LCD_22(-offset, SERVO_Z_OFFSET_ADDR);
		UPDATE_LCD_22(-zprobe_zoffset, SERVO_Z_OFFSET_ADDR);
	#endif

	#if	ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
//		float fade_height = 0.0;
//		if(leveling_is_active()){
//			fade_height = planner.z_fade_height;
//		}
//		UPDATE_LCD_31(fade_height, FADE_HEIGHT_ADDR);
			UPDATE_LCD_31(planner.z_fade_height, FADE_HEIGHT_ADDR);
	#endif
}
#endif


void dwin_update_time_info(uint8_t mode) {
	char time_str[9] = { 0 };

	if (mode == TIME_USED) {
		(duration_t(usedTime)).toTimeDWIN(time_str, 4);
		setValueAsAttr_P(PSTR(USED_TIME_ADDR), 0, time_str);
	}
	else if (mode == TIME_PRINT) {
		(duration_t(print_job_timer.duration()).toTimeDWIN(time_str, 2));
		setValueAsAttr_P(PSTR(PRINT_TIME_ADDR), 0, time_str);
	}
#ifdef REG_SN
	else if (mode == TIME_TRIAL) {
		float trialTime = TOTAL_TIME_LIMIT - usedTime;
		trialTime = (trialTime <= 0) ? 0 : trialTime;
		(duration_t(trialTime)).toTimeDWIN(time_str, 4);
		setValueAsAttr_P(PSTR(TRIAL_PERIOD_ADDR), 0, time_str);
	}
#endif
}

void dwin_update_progress_info() {
#if HAS_READER
	float percent = FILE_READER.percentDoneF();
	UPDATE_LCD_32(percent, PERCENT_ADDR);

	FILL_REC(PAINT_TOOL1_ADDR, PROGRESS_BAR_0_X0, PROGRESS_BAR_0_Y0, PROGRESS_BAR_0_X1, PROGRESS_BAR_0_Y1, COLOR_ASSIST);
	FILL_REC(PAINT_TOOL3_ADDR, PROGRESS_BAR_1_X0, PROGRESS_BAR_1_Y0, PROGRESS_BAR_1_X1, PROGRESS_BAR_1_Y1, COLOR_ASSIST);
	uint16_t p0 = PROGRESS_BAR_0_X0 + uint16_t((PROGRESS_BAR_0_X1 - PROGRESS_BAR_0_X0) * percent) / 100;
	uint16_t p1 = PROGRESS_BAR_1_X0 + uint16_t((PROGRESS_BAR_1_X1 - PROGRESS_BAR_1_X0) * percent) / 100;

	if(!FILE_IS_IDLE){
		FILL_REC(PAINT_TOOL2_ADDR, PROGRESS_BAR_0_X0, PROGRESS_BAR_0_Y0, p0, PROGRESS_BAR_0_Y1, COLOR_PRIMARY);
		FILL_REC(PAINT_TOOL4_ADDR, PROGRESS_BAR_1_X0, PROGRESS_BAR_1_Y0, p1, PROGRESS_BAR_1_Y1, COLOR_PRIMARY);
		dwin_update_time_info(TIME_PRINT);
	} else {
		DRAW_REC(PAINT_TOOL2_ADDR, PROGRESS_BAR_0_X0, PROGRESS_BAR_0_Y0, PROGRESS_BAR_0_X1, PROGRESS_BAR_0_Y1, COLOR_ASSIST);
		DRAW_REC(PAINT_TOOL4_ADDR, PROGRESS_BAR_1_X0, PROGRESS_BAR_1_Y0, PROGRESS_BAR_0_X1, PROGRESS_BAR_1_Y1, COLOR_ASSIST);
	}
#endif //HAS_READER
}

#ifdef REG_SN
void dwin_update_reg_info(){
	dwin_update_time_info(TIME_TRIAL);
	UPDATE_LCD_32(regSN, REG_KEY_ADDR);
	dwin_update_icon();
}
#endif


/*******************************************************************************************************/

/********************************************* Other Part **********************************************/

void dwin_setup() {
#if HAS_READER
	#if ENABLED(SDSUPPORT) && PIN_EXISTS(SD_DETECT)
		SET_INPUT_PULLUP(SD_DETECT_PIN);
	#endif
	dwinReaderState = 0xFF;					// unknown Reader state.
#endif

	dwin_data_state = DATA_UNINIT;	// dwin lcd uninit
	dwin_next_update_millis = 0;		// next data update time
	return_default_page_time = 0;		// return the default page time.
	dwin_next_page_P = nullptr;			// next dwin page is unknow.

	dwin_init();										// the implement init.
}

void updateData();

bool dwin_is_page_P(const char* page) {
	if (dwin_next_page_P) {
		char c[5] = { 0 };
		strcpy_P(c, page);

		if (strcmp_P(c, dwin_next_page_P))
			return false;
		else
			return true;
	} else {
		return isPage_P(page);
	}
}

void dwin_change_page_P(const char* page) {
	if(HAS_POPUP)		HIDE_POPUP;

	dwin_next_page_P = page;
	updateData();					// update the data of page in lcd before goto the page.
	dwin_next_page_P = nullptr;
	setPage_P(page);
}

// execute the cmd from dwin lcd.
void updateCmd(uint16_t cmdValue){
	if (cmdValue) {
		SET_INT_P(CMD_ADDR, CMD_EMPTY);

		switch (cmdValue) {
		case RETURN_DEFAULT_BUTTON:
		#ifdef STATE_PAGE_AS_DEFAULT
			return_default_button_action(true);
		#else
			return_default_button_action();
		#endif
			break;
		case RETURN_STATE_BUTTON:
			return_state_button_action();
			break;
		case PAUSE_BUTTON:
			print_pause_button_action();
			break;
		case REUSE_BUTTON:
			print_reuse_button_action();
			break;
		case CANCEL_BUTTON:
			print_cancel_button_action();
			break;
		case PREHEAT_BUTTON:
			preheat_button_action();
			break;
		case COOL_DWON_BUTTON:
			dwin_cooldwon();
			break;
		case OPEN_FILE_BUTTON:
			lcdUpdateFiles(OPEN);
			break;
		case OPEN_FILAMENT_BUTTON:
			open_filament_page_action();
			break;
		case CHANGE_BUTTON:
			print_change_button_action();
			break;
		case Z_UP_BUTTON:
			dwin_move(Z_AXIS, DWIN_Z_MOVE_SCALE, false);
			break;
		case Z_DOWN_BUTTON:
			dwin_move(Z_AXIS, DWIN_Z_MOVE_SCALE, true);
			break;
		case REG_INFO_BUTTON:
			jump_reg_action();
			break;

		case MOVE_HOME_ALL:
			dwin_move_home(HOME_ALL);
			break;
		case MOVE_HOME_X:
			dwin_move_home(HOME_X);
			break;
		case MOVE_HOME_Y:
			dwin_move_home(HOME_Y);
			break;
		case MOVE_HOME_Z:
			dwin_move_home(HOME_Z);
			break;
		case MOVE_X_PLUS:
			dwin_move(X_AXIS, true);
			break;
		case MOVE_Y_PLUS:
			dwin_move(Y_AXIS, true);
			break;
		case MOVE_Z_PLUS:
			dwin_move(Z_AXIS, true);
			break;
		case MOVE_X_MINUS:
			dwin_move(X_AXIS, false);
			break;
		case MOVE_Y_MINUS:
			dwin_move(Y_AXIS, false);
			break;
		case MOVE_Z_MINUS:
			dwin_move(Z_AXIS, false);
			break;

		case FILAMNET_EXT:
			dwin_filament_test(true);
			break;
		case FILAMENT_REC:
			dwin_filament_test(false);
			break;
		case FILAMNET_UNLOAD:
			dwin_filament_unload();
			break;

		case STORE_SETTINGS:
			store_setting_action();
			break;
		case RESET_SETTINGS:
			reset_setting_action();
			break;

		case FILE_NEXT_PAGE:
			lcdUpdateFiles(NEXT_PAGE);
			break;
		case FILE_LAST_PAGE:
			lcdUpdateFiles(LAST_PAGE);
			break;
		case FILE_UP_LEVEL:
			lcdUpdateFiles(UP_LEVEL);
			break;


		case FILE_ITEM_0_ACTION:
			fileItemAciton(0);
			break;
		case FILE_ITEM_1_ACTION:
			fileItemAciton(1);
			break;
		case FILE_ITEM_2_ACTION:
			fileItemAciton(2);
			break;
		case FILE_ITEM_3_ACTION:
			fileItemAciton(3);
			break;
		case FILE_ITEM_4_ACTION:
			fileItemAciton(4);
			break;
		case FILE_ITEM_5_ACTION:
			fileItemAciton(5);
			break;
		case FILE_ITEM_6_ACTION:
			fileItemAciton(6);
			break;
		case FILE_ITEM_7_ACTION:
			fileItemAciton(7);
			break;
		case FILE_ITEM_8_ACTION:
			fileItemAciton(8);
			break;


		case FILE_ACTION_PRINT:
			afterFileItemAction(PRINT);
			break;
		//case FILE_ACTION_OPEN:
		//	afterSdItemAction(OPEN_DIR);
		//	break;
		case FILE_ACTION_CANCEL:
			afterFileItemAction(CANCEL);
			break;
		case SHUTTING_CANCEL:
			shuttingCancelAction();
			break;
		case ACCIDENT_REUSE:
			accidentReuseAction();
			break;
		case ACCIDENT_CANCEL:
			accidentCancelAction();
			break;
		case CHANGE_DONE:
			changeDoneAction();
			break;

		case JUMP_SETGINS:
			jump_settting();
			break;
		case JUMP_SETGINS_2:
			jump_setting_2();
			break;
		case JUMP_LEVELING:
			jump_leveling_page();
			break;

	#ifdef WIFI_SUPPORT
		case WIFI_ITEM_0_ACTION:
			wifiItemAction(0);
			break;
		case WIFI_ITEM_1_ACTION:
			wifiItemAction(1);
			break;
		case WIFI_ITEM_2_ACTION:
			wifiItemAction(2);
			break;
		case WIFI_ITEM_3_ACTION:
			wifiItemAction(3);
			break;
		case WIFI_ITEM_4_ACTION:
			wifiItemAction(4);
			break;

		case WIFI_SWITCH_TOGGLE:
			wifiFuncAction(WIFI_ACTION_SWITCH_TOGGLE);
			break;
		case WIFI_CONNECT:
			wifiFuncAction(WIFI_ACTION_CONNECT);
			break;
		case WIFI_CANCEL:
			wifiFuncAction(WIFI_ACTION_CANCEL);
			break;
		case WIFI_SWITCH_MODE:
			wifiFuncAction(WIFI_ACTION_SWITCH_MODE);
			break;
		case WIFI_RESET:
			wifiFuncAction(WIFI_ACTION_RESET);
			break;
		case WIFI_SCAN:
			wifiFuncAction(WIFI_ACTION_SCAN);
			break;
		case WIFI_NEXT_PAGE:
			wifiFuncAction(WIFI_ACTION_NEXT);
			break;
		case WIFI_LAST_PAGE:
			wifiFuncAction(WIFI_ACTION_LAST);
			break;
	#endif


		case LEVELING_SWITCH_TOGGLE:
			leveling_toggle();
			break;
		case LEVELING_PROBE:
			leveling_probe();
			break;


		default:
			//SERIAL_ECHOLN(cmdValue);
			break;
		}
	}
}

/*******************************************************************************************************/




// scan the dwin lcd to receive the possible BackData and check if connect ready.
void dwin_scan() {
	dwin_loop();

	if (dwin_isExist()) {
		if ((currentPage() + 1)) {		// current page is valid.
			if(!IS_DWIN_DATA_READY){
				millis_t ms = millis();
				if (currentPage() > LCD_SETUP_LOOP_END) {
					if (dwin_data_state == DATA_UNINIT) {
						if (ms > dwin_next_update_millis) {
							dwin_next_update_millis = ms + LCD_RUN_CYCLE;
							GO_PAGE(LCD_SETUP_FIRST);
						}
					} else if (dwin_data_state == DATA_INIT) {
						dwin_data_state = DATA_READY;
					}
				} else {
					if (ms > dwin_next_update_millis) {
						dwin_next_update_millis = ms + LCD_RUN_CYCLE;

						dwin_update_version();
						dwin_update_index();
						updateStateStrings();
						return_default_button_action();
						dwin_data_state = DATA_INIT;
					}
				}
			} else{
				if(currentPage() <= LCD_SETUP_LOOP_END) {
					dwin_data_state = DATA_UNINIT;
				}
			}
		}
	} else {
		dwin_data_state = DATA_UNINIT;
	}
}

// resolve the back data.
void resolveVar() {


#ifdef DEBUG_FREE
	SERIAL_ECHOLNPAIR("resolver: ", freeMemory());
#endif

#define VAR_IS_ADDR(addr)	(uint16_tCompHex_P(PSTR(addr), dwin_getVar()->varAddr))
#define VAR_IS_LEN(len)		(dwin_getVar()->dataLen == len)
#define IS_VAR(len, addr)	(VAR_IS_LEN(len) && VAR_IS_ADDR(addr))

	if (VAR_IS_LEN(2)) {
		uint16_t varValue = ((uint16_t)dwin_getVar()->data[0] << 8) | ((uint16_t)dwin_getVar()->data[1]);

		if (VAR_IS_ADDR(CMD_ADDR)) {								// dwin_700_cmd
			updateCmd(varValue);
		} else if (VAR_IS_ADDR(MOVE_DISTANCE_INDEX_ADDR)) {			// axis_move_distance
			move_distance_index = (uint8_t)varValue;
		} else if (VAR_IS_ADDR(FILAMENT_EXT_DISTANCE_ADDR)) {		// filament_test_distance
			extrude_distance = varValue;
		} else if (VAR_IS_ADDR(FILAMENT_EXT_INDEX_ADDR)) {			// extruder index
			extrude_index = (uint8_t)varValue;
		} else if (VAR_IS_ADDR(SET_EX0_TAR_ADDR)) {							// extruder 0 target temp.
			thermalManager.setTargetHotend(varValue, 0);
		}
		#if EXTRUDERS > 1
		else if (VAR_IS_ADDR(SET_EX1_TAR_ADDR)) {								// extruder 1 target temp.
			thermalManager.setTargetHotend(varValue, 1);
		}
		#endif
		#if EXTRUDERS > 2
		else if (VAR_IS_ADDR(SET_EX2_TAR_ADDR)) {								// extruder 2 target temp.
			thermalManager.setTargetHotend(varValue, 2);
		}
		#endif
		else if (VAR_IS_ADDR(SET_CHAMBER_TAR_ADDR)){						// chamber target temp.
			thermalManager.setTargetChamber(varValue);
		}
		else if (VAR_IS_ADDR(SET_BED_TAR_ADDR)) {								// bed target temp.
			thermalManager.setTargetBed(varValue);
		} else if (VAR_IS_ADDR(SET_SPEED_GLOBAL_ADDR)) {				// print speed
			feedrate_percentage = varValue;
		} else if (VAR_IS_ADDR(SET_FAN_SPEED_ADDR)) {						// fan speed
			#if FAN_COUNT > 0
				#if FAN_COUNT > 1
					fanSpeeds[active_extruder < FAN_COUNT ? active_extruder : 0]
				#else
					fanSpeeds[0]
				#endif
										 = varValue * 2.55;
			#endif
		} else if (VAR_IS_ADDR(SET_EXT_MULTIPLY_ADDR)) {				// flow
			flow_percentage[active_extruder] = varValue;
		} else if (VAR_IS_ADDR(SET_PREHEAT_FAN_SPEED_ADDR)) {		// preheat fan speed
			lcd_preheat_fan_speed = varValue * 2.55;
		} else if (VAR_IS_ADDR(SET_PREHEAT_EXT_TEMP_ADDR_0)) {	// preheat extruder 0 temp.
			lcd_preheat_hotend_temp[0] = varValue;
		}
		#if EXTRUDERS > 1
		else if (VAR_IS_ADDR(SET_PREHEAT_EXT_TEMP_ADDR_1)) {		// preheat extruder 1 temp.
			lcd_preheat_hotend_temp[1] = varValue;
		}
		#endif
		#if EXTRUDERS > 2
		else if (VAR_IS_ADDR(SET_PREHEAT_EXT_TEMP_ADDR_2)) {		// preheat extruder 2 temp.
			lcd_preheat_hotend_temp[2] = varValue;
		}
		#endif
		else if (VAR_IS_ADDR(SET_PREHEAT_BED_TEMP_ADDR)) {			// preheat bed temp.
			lcd_preheat_bed_temp = varValue;
		}
		#ifdef HEATED_CHAMBER
		else if (VAR_IS_ADDR(SET_PREHEAT_CHAMBER_TEMP_ADDR)){		// perheat chamber temp.
			lcd_preheat_chamber_temp = varValue;
		}
		#endif
		#ifdef REG_SN
		else if (VAR_IS_ADDR(SET_REG_KEY_ADDR)) {								// register key
			regSN = (float)varValue / 100;
			dwin_update_reg_info();
			dwin_update_icon();
			if(REG_PASS){
				updateStateStrings();
				return_default_button_action();
			}
			STORE_SETTING(regSN);
		}
		#endif
		#if HAS_AUTO_FAN
		else if (VAR_IS_ADDR(SET_TEMP_FAN_SPEED_ADDR)) {			// temp. fan speed
			extruder_auto_fan_speed = varValue * 2.55;
		}
		#endif
		else if (VAR_IS_ADDR(SET_AIR_FAN_SPEED_ADDR)) {				// air fan speed
			#ifdef CHAMBER_FAN
			air_fan_speed = varValue * 2.55;
			#endif
		}
	#if HAS_BED_PROBE
		else if (VAR_IS_ADDR(SET_SERVO_Z_OFFSET_ADDR)) {				// servo Z offset
			zprobe_zoffset = -(float)(int16_t)varValue / 100;
			refresh_zprobe_zoffset();
			STORE_SETTING(zprobe_zoffset);
		}
	#endif
	#if	ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
		else if (VAR_IS_ADDR(SET_FADE_HEIGHT_ADDR)) {						// fade height
			if(leveling_is_active()){
				float fade_height = (float)varValue / 10;
				set_z_fade_height(fade_height);
				EEPROM_STORE(planner.z_fade_height, setting_z_fade_height);
			}
		}
	#endif
		else if (false) {
		} else {
			SERIAL_PROTOCOL_F(dwin_getVar()->varAddr, HEX);
			SERIAL_EOL();
		}
	} else if (VAR_IS_LEN(4)) {
		uint32_t varValue = ((uint32_t)dwin_getVar()->data[0] << 24) | ((uint32_t)dwin_getVar()->data[1] << 16) | ((uint32_t)dwin_getVar()->data[2] << 8) | ((uint32_t)dwin_getVar()->data[3]);

		if (VAR_IS_ADDR(SET_X_STEP_ADDR)) {
			planner.axis_steps_per_mm[X_AXIS] = (float)varValue / 10000;	// axis X motor step
			planner.refresh_positioning();
		} else if (VAR_IS_ADDR(SET_Y_STEP_ADDR)) {
			planner.axis_steps_per_mm[Y_AXIS] = (float)varValue / 10000;	// axis Y motor step
			planner.refresh_positioning();
		} else if (VAR_IS_ADDR(SET_Z_STEP_ADDR)) {
			planner.axis_steps_per_mm[Z_AXIS] = (float)varValue / 10000;	// axis Z motor step
			planner.refresh_positioning();
		} else if (VAR_IS_ADDR(SET_E_STEP_ADDR)) {
			planner.axis_steps_per_mm[E_AXIS] = (float)varValue / 10000;	// axis E motor step
			planner.refresh_positioning();
		} else {
			SERIAL_PROTOCOL_F(dwin_getVar()->varAddr, HEX);
			SERIAL_EOL();
		}
	} else if (VAR_IS_LEN((DEFAULT_STR_LEN * 2))) {
		if(false){
			;
		}
	#if HAS_READER
		else if (VAR_IS_ADDR(FILE_ITEM_0) || VAR_IS_ADDR(FILE_ITEM_1) || VAR_IS_ADDR(FILE_ITEM_2) || VAR_IS_ADDR(FILE_ITEM_3) || VAR_IS_ADDR(FILE_ITEM_4) ||
			VAR_IS_ADDR(FILE_ITEM_5) || VAR_IS_ADDR(FILE_ITEM_6) || VAR_IS_ADDR(FILE_ITEM_7) || VAR_IS_ADDR(FILE_ITEM_8)) {
				char name[FILE_NAME_LEN + 1];
				strcpy(name, (char *)dwin_getVar()->data);

				// show the string in the center of LCD.
				centerString(name, LCD_FILE_CHAR_LEN);
				SET_STR_P(FILE_SELECT, LCD_FILE_CHAR_LEN, name);
		}
	#endif
	#ifdef WIFI_SUPPORT
		else if(VAR_IS_ADDR(WIFI_KEY_ADDR)){
				strcpy(dwinWifiConnKey, (char *)dwin_getVar()->data);
				wifiFuncAction(WIFI_ACTION_JOIN);
		}
	#endif
		else {
			SERIAL_ECHOLNPGM("unknown var");
			SERIAL_PROTOCOL_F(dwin_getVar()->varAddr, HEX);
			SERIAL_EOL();
		}
	}
	else {
		SERIAL_ECHOLNPGM("unknown var_len");
		SERIAL_PROTOCOL_F(dwin_getVar()->varAddr, HEX);
		SERIAL_ECHOLNPAIR(":	", (int)dwin_getVar()->dataLen);
	}
}

// control the dwin lcd page.
void pageControl() {
	bool touchLCD = DWIN_TOUCH;

	if (touchLCD) return_default_page_time = millis() + LCD_TIMEOUT_TO_STATUS;

#ifdef ACCIDENT_DETECT
	if (isAccident && (powerState > POWER_COOLING) && !DWIN_IS_PAGE(PAGE_UNFINISH_CHOOSE)){
		GO_PAGE(PAGE_UNFINISH_CHOOSE);
	}
#endif //ACCIDENT_DETECT

#if HAS_BED_PROBE
	if (IS_PROBE_PAGE && HAS_POPUP && probeDone){
		HIDE_POPUP;
		jump_leveling_page();
		return_default_page_time = millis() + LCD_TIMEOUT_TO_STATUS;
	}
#endif

	if (DWIN_IS_PAGE(PAGE_FILAMENT) && HAS_POPUP && !planner.blocks_queued())	HIDE_POPUP;

#if HAS_READER
	if(dwinReaderState != READER_STATE){
		if(READER_CONN){
			file_page = 1;
			up_file_page = 1;
			file_item_num = 0;
			file_item_selected = -1;

		#ifdef SDSUPPORT
			card.initsd();
		#endif

			if(dwinReaderState != 0xFF){
				if(READER_VALID){
					DWIN_MSG_P(DWIN_MSG_USB_READY);
				#ifdef WIFI_SUPPORT
					myWifi.actionSynPrinterInfo();
				#endif
				} else {
					DWIN_MSG_P(DWIN_MSG_READER_CONN);
				}
			}
		} else {
		#if ENABLED(QUICK_PAUSE)
			quickPausePrintJob();
		#endif //QUICK_PAUSE
			FILE_READER.release();

			if(dwinReaderState != 0xFF){
				DWIN_MSG_P(DWIN_MSG_READER_REMOVED);
			#ifdef WIFI_SUPPORT
				myWifi.actionSynPrinterInfo();
			#endif
			}
		}
		if (IS_MENU_PAGE || IS_PRINT_DEFAULT_PAGE || IS_FILE_PAGE) {
			return_default_button_action();
		}

		dwinReaderState = READER_STATE;
	}
#endif	//HAS_READER

	if(isSerialPrinting){		// the default page is state page while printing from serial.
		if (return_default_page_time && (millis() > return_default_page_time) && !HAS_POPUP && IS_TIMEOUT_PAGE_SERIAL) return_state_button_action();
	}else{
		if (return_default_page_time && (millis() > return_default_page_time) && !HAS_POPUP && IS_TIMEOUT_PAGE)	return_default_button_action();
	}

}

// update the dwin lcd data.
void updateData() {
	if (IS_STATE_PAGE) {
		dwin_update_state_info();
	}

	if (DWIN_IS_PAGE(PAGE_SHUTDOWN_HOTEND)) {
		dwin_update_max_temp_info();
	}

	if (DWIN_IS_PAGE(PAGE_MOVE) || IS_STATE_PAGE) {
		dwin_update_axis_info();
	}

	if (IS_SETTING_PAGE) {
		dwin_update_setting_info();
	}

#ifdef WIFI_SUPPORT
	if (IS_WIFI_INFO_PAGE) {
		dwin_update_wifi_info();
	}
#endif

#if HAS_LEVELING
	if (IS_LEVELING_PAGE){
		dwin_update_leveling_info();
	}
#endif

	if (IS_PRINT_PAGE) {
		dwin_update_progress_info();
	}

	if (DWIN_IS_PAGE(PAGE_INFO)) {
		dwin_update_time_info(TIME_USED);
	}
	
#ifdef REG_SN
	if (IS_REG_PAGE) {
		dwin_update_reg_info();
	}
#endif

#ifdef POWER_MANAGEMENT
	if(dwin_shutting_info[0]){
		DWIN_MSG(dwin_shutting_info);
	#ifdef AUTO_SHUTDOWN_DEBUG
		SERIAL_ECHOLN(dwin_shutting_info);
	#endif
	}
#endif
}


// dwin loop run.
void dwin_run(){
	manual_move_delay();
	dwin_scan();		// must at the first.
	
	if (IS_DWIN_DATA_READY) {
		if (dwin_getVar()->valid) {
		#ifdef DEBUG_FREE
			SERIAL_ECHOLNPAIR("dwin_run: ", freeMemory());
		#endif
			resolveVar();
		}

		pageControl();

		if (millis() > dwin_next_update_millis) {
			dwin_next_update_millis = millis() + 1000L;		// update the lcd every one second.
			updateData();
		}
	}
}

#endif // DWIN_LCD
