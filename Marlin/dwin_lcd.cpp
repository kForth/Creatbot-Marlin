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
#ifdef HOTWIND_SYSTEM
	int lcd_preheat_chamber_temp;
#endif
int lcd_preheat_fan_speed;

enum DATA_STATE {DATA_UNINIT, DATA_INIT, DATA_GIF_START, DATA_GIF_END, DATA_READY};
static uint8_t dwin_data_state = DATA_UNINIT;
#define IS_DWIN_DATA_READY		(dwin_data_state == DATA_READY)

static float dwin_manual_feedrate[] = MANUAL_FEEDRATE;
static uint8_t manual_move_axis = 0x00;
static millis_t manual_move_start_time = 0;
static millis_t return_default_page_time = 0;
static millis_t lock_page_time = 0;
static millis_t dwin_next_update_millis = 0;

static uint8_t move_distance_index = 0;
static uint8_t extrude_index = 0;
static int extrude_distance = 0;

#if HAS_READER
static uint8_t dwinReaderState;
#endif

#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
static uint8_t dwinWifiConnIndex;
#elif defined(WIFI_MODULE_ESP8266_12)
static char dwinWifiConnSSID[LCD_WIFI_SSID_LEN + 1];
#endif
static char dwinWifiConnKey[LCD_WIFI_KEY_LEN + 1];
#endif

#ifdef POWER_MANAGEMENT
	char dwin_shutting_info[LCD_MSG_CHAR_LEN + 1] = { 0 };
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
#define GET_STR_BY_LEN(addr, len)			getValueForString_P(addr, len)
#define SET_STR(addr, len, value)			setValueAsString_P(addr, len, value);
#define SET_USTR(addr, len, value)		setValueAsUnicodeString_P(addr, len, value);
#define GET_ANY(addr, len)						getValueForAttr_P(addr, 0, len)

#define DRAW_REC(addr, x0, y0, x1, y1, color)		drawRectangle_P(addr, x0, y0, x1, y1, color)
#define FILL_REC(addr, x0, y0, x1, y1, color)		fillRectangle_P(addr, x0, y0, x1, y1, color)
#define CLEAR_CANVAS(addr)                      clearCanvas_P(addr)


void updateValue(float value, const char* valueAddr, uint8_t mode, uint8_t int_num, uint8_t dec_num){
	if(mode == INT_LCD)
		SET_INT(valueAddr, convToLCD(value, int_num, dec_num));
	else if(mode == LONG_LCD)
		SET_LONG(valueAddr, convToLCD(value, int_num, dec_num));
}

void updateValue(long value, const char* valueAddr){
	SET_LONG(valueAddr, value);
}

void updateValue(int value, const char* valueAddr){
	SET_INT(valueAddr, value);
}

#define UPDATE_LCD(value, addr)				updateValue(value, addr)
#define UPDATE_LCD_22(value, addr)		updateValue(value, addr, INT_LCD, 2, 2)
#define UPDATE_LCD_31(value, addr)		updateValue(value, addr, INT_LCD, 3, 1)
#define UPDATE_LCD_32(value, addr)		updateValue(value, addr, INT_LCD, 3, 2)
#define UPDATE_LCD_34(value, addr)		updateValue(value, addr, LONG_LCD, 3, 4)
#define UPDATE_LCD_42(value, addr)		updateValue(value, addr, LONG_LCD, 4, 2)
#define UPDATE_LCD_43(value, addr)		updateValue(value, addr, LONG_LCD, 4, 3)

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

/******************************************** Function Part ********************************************/

void dwin_cooldwon(){
	thermalManager.disable_all_heaters();
#if FAN_COUNT > 0
	for (uint8_t i = 0; i < FAN_COUNT; i++) fanSpeeds[i] = 0;
#endif
	returnDefaultButtonAction();
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


#ifdef HOTWIND_SYSTEM
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
		#ifdef HOTWIND_SYSTEM
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
		#ifdef HOTWIND_SYSTEM
			lcd_preheat_chamber_temp,
		#endif
			lcd_preheat_fan_speed
		);
	} else if(EXTRUDERS > target){
		DWIN_PREHEAT(
			target,
			lcd_preheat_hotend_temp[target],
			lcd_preheat_bed_temp,
		#ifdef HOTWIND_SYSTEM
			lcd_preheat_chamber_temp,
		#endif
			lcd_preheat_fan_speed
		);
	}
	returnDefaultButtonAction();
}

void dwin_move_home(uint8_t mode){
	if(mode == HOME_ALL){
		enqueue_and_echo_commands_P(PSTR("G28"));
	}else if(mode == HOME_X){
		enqueue_and_echo_commands_P(PSTR("G28 X"));
	}else if(mode == HOME_Y){
		enqueue_and_echo_commands_P(PSTR("G28 Y"));
	}else if(mode == HOME_Z){
	#if ENABLED(Z_SAFE_HOMING)
		enqueue_and_echo_commands_P(PSTR("G28"));
	#else
		enqueue_and_echo_commands_P(PSTR("G28 Z"));
	#endif
	}

	POP_WINDOW(POP_ICO_HOMING, POPUP_DELAY_ALWAY_DELAY);
}

void dwin_update_axis_info();

void dwin_move(int axis, float scale, bool dir){

	float min = current_position[axis] - 1000,
				max = current_position[axis] + 1000;

	#if HAS_SOFTWARE_ENDSTOPS
		if(soft_endstops_enabled){
			#if ENABLED(MIN_SOFTWARE_ENDSTOPS)
				min = soft_endstop_min[axis];
			#endif
			#if ENABLED(MAX_SOFTWARE_ENDSTOPS)
				max = soft_endstop_max[axis];
			#endif
		}
	#endif

	if(dir)
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
		manual_move_start_time = millis() + 500UL;
		SBI(manual_move_axis, axis);
	}
}

void dwin_move(int axis, bool dir){
	switch(move_distance_index){
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
			if(IS_MOVE_PAGE){
        POP_WINDOW(POP_ICO_MOVING, POPUP_DELAY_ALWAY_DELAY);
			}
			refresh_cmd_timeout();
		}
		manual_move_axis = 0;
	}
}

bool dwin_filament_active_is_valid(int hotend){
	if(hotend == 0){
		active_extruder = 0;
	}else if(hotend == 1){
#if EXTRUDERS > 1
		active_extruder = 1;
#endif
	}else if(hotend == 2){
#if EXTRUDERS > 2
		active_extruder = 2;
#endif
	}

	if (thermalManager.targetTooColdToExtrude(active_extruder)) {
		POP_WINDOW(POP_ICO_TAR_TEMP_LOW, POPUP_DELAY_500MS);
		return false;
	} else if (thermalManager.tooColdToExtrude(active_extruder)){
		POP_WINDOW(POP_ICO_CUR_TEMP_LOW, POPUP_DELAY_500MS);
		return false;
	} else {
		return true;
	}
}

void dwin_filament_test(int hotend, bool dir){
	if(dwin_filament_active_is_valid(hotend)){
		float testLength = float(extrude_distance) * (dir ? 1 : -1);
		current_position[E_AXIS] += testLength;

		planner.buffer_line_kinematic(current_position, MMM_TO_MMS(dwin_manual_feedrate[E_AXIS]), active_extruder);

		if (dir) POP_WINDOW(POP_ICO_EXTRUDE_FILAMENT, POPUP_DELAY_ALWAY_DELAY);
		else POP_WINDOW(POP_ICO_RECRACT_FILAMENT, POPUP_DELAY_ALWAY_DELAY);
	}
}

void dwin_filament_test(bool dir){
	switch(extrude_index){
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
	if(dwin_filament_active_is_valid(hotend)){
		current_position[E_AXIS] += FILAMENT_UNLOAD_EXTRUDER_LENGTH;
		planner.buffer_line_kinematic(current_position, MMM_TO_MMS(dwin_manual_feedrate[E_AXIS]), active_extruder);
		POP_WINDOW(POP_ICO_PREPARE_UNLOAD, POPUP_DELAY_ALWAY_DELAY);
		enqueue_and_echo_commands_P(PSTR("M400\nM6003"));
	}
}

void dwin_filament_unload(){
	switch(extrude_index){
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

void dwin_update_hotend(uint8_t varValue){
  extrude_index = varValue;
#if ENABLED(SWITCHING_NOZZLE)
  switch(extrude_index){
  case 1:
  case 2:
    switch_nozzle(0);
    break;
  case 3:
    switch_nozzle(1);
    break;
  default:
    switch_nozzle(0);
    break;
  }
#endif
}


void centerString(char* str, uint8_t len){
	uint8_t i = strlen(str);
	if(i >= len) return;

	uint8_t j, space;
	char raw[LCD_MAX_STR_LEN + 1];
	strcpy(raw, str);
	space = (len - i) / 2;
	for (i = 0; space--;)	str[i++] = ' ';
	for (j = 0; raw[j];)	str[i++] = raw[j++];
	str[i] = 0;
}

void centerUString(char* ustr, uint8_t len){
	uint8_t l = unicodeStrlen(ustr);
	if(l >= len) return;

	char raw[LCD_MAX_STR_LEN + 2] = {0};
	memcpy(raw, ustr, l);
	uint8_t i, space = (len - l) / 4;
	for(i = 0; space--;){
		ustr[i++] = 0x30;
		ustr[i++] = 0x00;			//double space
	}
	char *p = &ustr[i];
	memcpy(p, raw, l);
	ustr[i+l] = ustr[i+l+1] = 0;
}

#if HAS_READER
uint16_t page_num = 0;
uint16_t file_page = 1;
uint16_t up_file_page = 1;
uint16_t file_item_num = 0;
uint8_t file_item_selected = -1;

void dwinGetFileName(uint8_t fileItem){
	char *cc;
	uint8_t i;
	int16_t file_item_index;

	char name[LCD_FILE_CHAR_LEN + NULL_CHAR_LENGTH], *c;
	uint8_t end_index;
#ifdef WIFI_SUPPORT
	char wifi_name[LCD_FILE_CHAR_LEN_WIFI + 3], *wifi_c;
	uint8_t wifi_end_index;
#endif

#define GET_FILE_ITEM_INDEX(index) 				file_item_index = (file_item_num - 1) - (file_page - 1) * LCD_FILE_ITEM_PER_PAGE - index;

#ifdef DWIN_FILENAME_USE_ICO
	#define OFFSET_AS_DIR										false
	#define SET_OFFSET											1
	#define CLEAR_ICO(index)								UPDATE_LCD(0, FILE_ITEM_ICO_ ## index)
	#define SET_ICO(index)									UPDATE_LCD(CUR_FILE_IS_DIR ? 2 : 1, FILE_ITEM_ICO_ ## index)
#else
	#define OFFSET_AS_DIR										(CUR_FILE_IS_DIR)
	#define SET_OFFSET											(CUR_FILE_IS_DIR ? 0 : 1)
	#define CLEAR_ICO(index)								;
	#define SET_ICO(index)									;
#endif

#ifdef FILE_UNICODE_SUPPORT
	#define INIT_NAME_GET_END_INDEX 				end_index = (sizeof(name) - NULL_CHAR_LENGTH - (OFFSET_AS_DIR ? 4 : 0));
	#define INIT_NAME_PREFIX_NAME						if(OFFSET_AS_DIR){ name[0] = 0x00; name[1] = '['; c = &name[2]; } else { c = name; }
	#define SET_NAME_WHILE									while(cc[i*2] || cc[i*2+1])
	#define SET_NAME_IF_ABOVE								if(i*2+1 >= end_index)
	#define SET_NAME_IF_EQUAL								if((i*2+1 == end_index) || (i*2 == end_index))
	#define SET_NAME_POINTER_MOVE						*(c++) = cc[i*2]; *(c++) = cc[i*2+1];
//	#define SET_NAME_END_INDEX							name[(i-SET_OFFSET)*2] = name[(i-SET_OFFSET-1)*2] = name[(i-SET_OFFSET-2)*2] = 0x18; name[(i-SET_OFFSET)*2+1] = name[(i-SET_OFFSET-1)*2+1] = name[(i-SET_OFFSET-2)*2+1] = 0x01;
	#define SET_NAME_END_INDEX							name[(i-SET_OFFSET)*2] = 0x18; name[(i-SET_OFFSET)*2+1] = 0x01; name[(i-SET_OFFSET-1)*2] = 0x00; name[(i-SET_OFFSET-1)*2+1] = 0x20;
	#define END_NAME_SUFFIX_NAME						if(OFFSET_AS_DIR){ name[(min(i, end_index/2)+1)*2] = 0x00; name[(min(i, end_index/2)+1)*2 + 1] = ']'; }
	#define SHOW_NAME(index)								SET_USTR(FILE_ITEM_STR_ ## index, LCD_FILE_CHAR_LEN, name)
#else
	#define INIT_NAME_GET_END_INDEX 				end_index = (sizeof(name) - NULL_CHAR_LENGTH - (OFFSET_AS_DIR ? 2 : 0));
	#define INIT_NAME_PREFIX_NAME						if(OFFSET_AS_DIR){ name[0] = '['; c = &name[1]; } else { c = name; }
	#define SET_NAME_WHILE 									while(cc[i])
	#define SET_NAME_IF_ABOVE								if(i >= end_index)
	#define SET_NAME_IF_EQUAL								if(i == end_index)
	#define SET_NAME_POINTER_MOVE						*(c++) = cc[i];
	#define SET_NAME_END_INDEX							name[i-SET_OFFSET] = name[i-SET_OFFSET-1] = name[i-SET_OFFSET-2] = '.';
	#define END_NAME_SUFFIX_NAME						if(OFFSET_AS_DIR){ name[min(i, end_index) + 1] = ']'; }
	#define SHOW_NAME(index) 								SET_STR(FILE_ITEM_STR_ ## index, LCD_FILE_CHAR_LEN, name)
#endif

#ifdef WIFI_SUPPORT
	#define CLEAR_NAME											memset(name, 0, sizeof(name)); memset(wifi_name, 0, sizeof(wifi_name));
	#define INIT_NAME_GET_WIFI_END_INDEX		wifi_end_index = (sizeof(wifi_name) - 3);
	#define INIT_NAME_PREFIX_WIFI_NAME			wifi_name[0] = (CUR_FILE_IS_DIR ? '<' : ':'); wifi_c = &wifi_name[1];
	#define SET_NAME_WIFI_IF_ABOVE					if(i >= wifi_end_index)
	#define SET_NAME_WIFI_IF_EQUAL					if(i == wifi_end_index)
	#define SET_NAME_WIFI_END_INDEX					wifi_name[i] = wifi_name[i-1] = wifi_name[i-2] = '.';
	#ifdef FILE_UNICODE_SUPPORT
    #define SET_NAME_WIFI_POINTER_MOVE   *(wifi_c++) = (cc[i*2] == 0x00) ? cc[i*2+1] : '*';
		#define SET_NAME_BREAK_IF							if(i == max(end_index/2, wifi_end_index))
	#else
		#define SET_NAME_WIFI_POINTER_MOVE 		*(wifi_c++) = cc[i];
		#define SET_NAME_BREAK_IF							if(i == max(end_index, wifi_end_index))
	#endif
	#define END_NAME_SUFFIX_WIFI_NAME 			wifi_name[min(i, wifi_end_index) + 1] = (CUR_FILE_IS_DIR ? '>' : ':')
#else
	#define CLEAR_NAME											memset(name, 0, sizeof(name));
	#define INIT_NAME_GET_WIFI_END_INDEX		;
	#define INIT_NAME_PREFIX_WIFI_NAME			;
	#define SET_NAME_WIFI_IF_ABOVE					if(false)
	#define SET_NAME_WIFI_IF_EQUAL					if(false)
	#define SET_NAME_WIFI_POINTER_MOVE			;
	#define SET_NAME_WIFI_END_INDEX					;
	#define SET_NAME_BREAK_IF								SET_NAME_IF_EQUAL
	#define	END_NAME_SUFFIX_WIFI_NAME				;
#endif


#define INIT_NAME do{\
		INIT_NAME_GET_END_INDEX;\
		INIT_NAME_GET_WIFI_END_INDEX;\
		INIT_NAME_PREFIX_NAME;\
		INIT_NAME_PREFIX_WIFI_NAME;\
	}while(0)

#define SET_NAME do{\
		i = 0; cc = FILE_READER.longFilename;\
		SET_NAME_WHILE{\
			if(i > 5){\
				SET_NAME_IF_ABOVE{\
					SET_NAME_IF_EQUAL{\
						SET_NAME_END_INDEX;\
					}\
				} else {\
					SET_NAME_POINTER_MOVE;\
				}\
				SET_NAME_WIFI_IF_ABOVE{\
					SET_NAME_WIFI_IF_EQUAL{\
						SET_NAME_WIFI_END_INDEX;\
					}\
				} else {\
				  SET_NAME_WIFI_POINTER_MOVE;\
				}\
				SET_NAME_BREAK_IF{\
					break;\
				} else {\
					i++;\
				}\
			} else {\
				SET_NAME_POINTER_MOVE;\
				SET_NAME_WIFI_POINTER_MOVE;\
				i++;\
			}\
		}\
	}while(0)

#define END_NAME do{\
		END_NAME_SUFFIX_NAME;\
		END_NAME_SUFFIX_WIFI_NAME;\
	}while(0)

#define SET_FILE_ITEM(index) do {\
		GET_FILE_ITEM_INDEX(index);\
		CLEAR_NAME;\
		CLEAR_ICO(index);\
		\
		if(file_item_index >= 0){\
			TOUCH_FILE(file_item_index);\
			INIT_NAME;\
			SET_NAME;\
			END_NAME;\
			SET_ICO(index);\
		}\
		SHOW_NAME(index);\
	}while(0)

#define SET_FILE_NAME(index)  do {\
    GET_FILE_ITEM_INDEX(index);\
    CLEAR_NAME;\
    \
    if(file_item_index >= 0){\
      TOUCH_FILE(file_item_index);\
      INIT_NAME;\
      SET_NAME;\
      END_NAME;\
    }\
  }while(0)

	if(fileItem == 0xFF){
		SET_FILE_ITEM(0);
	#if LCD_FILE_ITEM_PER_PAGE > 1
		SET_FILE_ITEM(1);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 2
		SET_FILE_ITEM(2);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 3
		SET_FILE_ITEM(3);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 4
		SET_FILE_ITEM(4);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 5
		SET_FILE_ITEM(5);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 6
		SET_FILE_ITEM(6);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 7
		SET_FILE_ITEM(7);
	#endif
	#if LCD_FILE_ITEM_PER_PAGE > 8
		SET_FILE_ITEM(8);
	#endif
	}else{
	  SET_FILE_NAME(fileItem);
	#ifdef WIFI_SUPPORT
		if(file_item_index < 0){
			if(file_item_index == -1){
				myWifi.synFilesInfo(fileItem, "", true);
				TOUCH_WORKDIR;
				myWifi.endSysFile((FILE_READER.filename[0] == '/'), page_num, file_page);
			}
		}else{
			if(fileItem < LCD_FILE_ITEM_PER_PAGE){
				myWifi.synFilesInfo(fileItem, wifi_name, (fileItem == (LCD_FILE_ITEM_PER_PAGE - 1)));
			}else if(fileItem == LCD_FILE_ITEM_PER_PAGE){
				TOUCH_WORKDIR;
				myWifi.endSysFile((FILE_READER.filename[0] == '/'), page_num, file_page);
			}
		}
	#endif
	}
}

void lcdUpdateFiles(uint8_t mode){
	if(mode == UP_LEVEL){
		file_page = up_file_page;
		up_file_page = 1;
		file_item_num = 0;
		file_item_selected = -1;
		FILE_READER.updir();
	}

	file_item_num = GET_FILE_NR;

	page_num = (file_item_num + LCD_FILE_ITEM_PER_PAGE - 1) / LCD_FILE_ITEM_PER_PAGE;

	if(mode == LAST_PAGE && file_page > 1) file_page--;
	if(mode == NEXT_PAGE && file_page < page_num) file_page++;
#ifdef WIFI_SUPPORT
	if(!myWifi.isRequest() || IS_FILE_PAGE){
#endif
		dwinGetFileName();
		if(!page_num){
			char name[LCD_FILE_CHAR_LEN + NULL_CHAR_LENGTH];
		#ifdef FILE_UNICODE_SUPPORT
			memcpy_P(name, PSTR(DWIN_MSG_NO_FILE), DWIN_MSG_NO_FILE_LENGTH);
			#ifndef LCD_FILE_CHAR_MAXIMIZE
				centerUString(name, LCD_FILE_CHAR_LEN);
			#endif
			SET_USTR(FILE_ITEM_CENTER, LCD_FILE_CHAR_LEN, name);
		#else
			strcpy_P(name, PSTR(DWIN_MSG_NO_FILE));
			centerString(name, LCD_FILE_CHAR_LEN);
			SET_STR(FILE_ITEM_CENTER, LCD_FILE_CHAR_LEN, name);
		#endif
		}

		TOUCH_WORKDIR;
		if(FILE_READER.filename[0] == '/'){
			if(page_num <= 1) GO_PAGE(PAGE_FILE_ONE);
			else{
				if(file_page == 1) GO_PAGE(PAGE_FILE_NEXT);
				if(file_page == page_num) GO_PAGE(PAGE_FILE_LAST);
				if(file_page > 1 && file_page < page_num) GO_PAGE(PAGE_FILE_BOTH);
			}
		}else{
			if(page_num <= 1) GO_PAGE(PAGE_FILE_UP_ONE);
			else{
				if(file_page == 1) GO_PAGE(PAGE_FILE_UP_NEXT);
				if(file_page == page_num) GO_PAGE(PAGE_FILE_UP_LAST);
				if(file_page > 1 && file_page < page_num) GO_PAGE(PAGE_FILE_UP_BOTH);
			}
		}
#ifdef WIFI_SUPPORT
	}
  #ifdef WIFI_MODULE_ESP8266_01
    myWifi.actionSynFilesInfo(true);
  #elif defined(WIFI_MODULE_ESP8266_12)
    myWifi.actionSynFilesInfo();
  #endif  //WIFI_MODULE_ESP8266_01|12
#endif  //WIFI_SUPPORT
}

void afterFileItemAction(uint8_t mode){
	if(mode){
		int16_t file_item_index = (file_item_num - 1) - (file_page - 1) * LCD_FILE_ITEM_PER_PAGE - file_item_selected;
		TOUCH_FILE(file_item_index);

		if(mode == PRINT){
			FILE_READER.openAndPrintFile(FILE_READER.filename);
			returnDefaultButtonAction();
		}else if(mode == OPEN_DIR){
			FILE_READER.chdir(FILE_READER.filename);
			up_file_page = file_page;
			file_page = 1;
			file_item_num = 0;
			file_item_selected = -1;
			lcdUpdateFiles(OPEN);
		}
	}else{
		file_item_selected = -1;
		lcdUpdateFiles(OPEN);
	}
}

void fileItemAction(uint8_t index){
	file_item_selected = index;
	int16_t file_item_index = (file_item_num - 1) - (file_page - 1) * LCD_FILE_ITEM_PER_PAGE - index;
	TOUCH_FILE(file_item_index);

	if(file_item_index >= 0){
		if(CUR_FILE_IS_DIR){
			afterFileItemAction(OPEN_DIR);
		}else{
		#ifdef WIFI_SUPPORT
			if(myWifi.isRequest()) afterFileItemAction(PRINT);
			else
		#endif
			{
				// get the string value that chosen.
			#if LCD_FILE_CHAR_LEN == DEFAULT_STR_LEN * 2
				#define GET_FILE_NAME(addr) GET_STR(addr)
			#else
				#define GET_FILE_NAME(addr) GET_STR_BY_LEN(addr, LCD_FILE_CHAR_LEN/2)
			#endif
				switch(file_item_selected){
				case 0:
					GET_FILE_NAME(FILE_ITEM_STR_0);
					break;
			#if LCD_FILE_ITEM_PER_PAGE > 1
				case 1:
					GET_FILE_NAME(FILE_ITEM_STR_1);
					break;
			#endif
			#if LCD_FILE_ITEM_PER_PAGE > 2
				case 2:
					GET_FILE_NAME(FILE_ITEM_STR_2);
					break;
			#endif
			#if LCD_FILE_ITEM_PER_PAGE > 3
				case 3:
					GET_FILE_NAME(FILE_ITEM_STR_3);
					break;
			#endif
			#if LCD_FILE_ITEM_PER_PAGE > 4
				case 4:
					GET_FILE_NAME(FILE_ITEM_STR_4);
					break;
			#endif
			#if LCD_FILE_ITEM_PER_PAGE > 5
				case 5:
					GET_FILE_NAME(FILE_ITEM_STR_5);
					break;
			#endif
			#if LCD_FILE_ITEM_PER_PAGE > 6
				case 6:
					GET_FILE_NAME(FILE_ITEM_STR_6);
					break;
			#endif
			#if LCD_FILE_ITEM_PER_PAGE > 7
					case 7:
					GET_FILE_NAME(FILE_ITEM_STR_7);
					break;
			#endif
			#if LCD_FILE_ITEM_PER_PAGE > 8
					case 8:
					GET_FILE_NAME(FILE_ITEM_STR_8);
					break;
			#endif
				}
				GO_PAGE(PAGE_PRINT_CONFIRM);
			}
		}
	}
}
#else		// !HAS_READER
void dwinGetFileName(uint8_t fileItem){ UNUSED(fileItem); };
void lcdUpdateFiles(uint8_t mode){ UNUSED(mode); };
void afterFileItemAction(uint8_t mode){ UNUSED(mode); };
void fileItemAciton(uint8_t index){ UNUSED(index); };
#endif	//HAS_READER

/********************************************* Action Part *********************************************/

inline void returnUpLevelButtonAction(){
	if(false){}
	#ifdef REG_SN
	else if(IS_REG_PAGE){
		if(!LIMITED_USE){
			GO_PAGE(PAGE_INFO_PRINTER);
		}
	}
	#endif
}

inline void returnDefaultButtonAction(){
#if HAS_READER
	if(isSerialPrinting)
		GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_ONLINE));
	else
		if(!FILE_IS_IDLE)
			if(FILE_IS_PRINT)
				if(wait_for_user)
					GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_PRINT_RESUME));
				else
					GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_PRINT_PAUSE));
			else
				GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_PRINT_RESUME));
		else
			if(thermalManager.hasHeat())
				if(READER_VALID)
					GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_INSERT));
				else
					GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE));
			else
				if(READER_VALID)
					GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_INSERT));
				else
					GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE));
#else		// !HAS_READER
	if(isSerialPrinting)
		GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_ONLINE));
	else
		if(thermalManager.hasHeat())
			GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_COOLDOWN_DEVICE_REMOVE));
		else
			GO_PAGE(PAGE_PARSE(PAGE_DEFAULT_IDLE_PREHEAT_DEVICE_REMOVE));
#endif		// HAS_READER
}

inline void printPauseButtonAction(){
#if HAS_READER
	#if ENABLED(QUICK_PAUSE)
		if(quickPausePrintJob()){
			DWIN_MSG_P(DWIN_MSG_PRINT_PAUSED);
			returnDefaultButtonAction();
		}
	#else
		FILE_PAUSE_PRINT;
		print_job_timer.pause();
		#if ENABLED(PARK_HEAD_ON_PAUSE)
			enqueue_and_echo_commands_P(PSTR("M125"));
		#endif
		DWIN_MSG_P(DWIN_MSG_PRINT_PAUSED);
	#endif //QUICK_PAUSE
#endif
}

inline void printReuseButtonAction(){
#if HAS_READER
	#if ENABLED(QUICK_PAUSE)
		if(quickReusePrintJob()){
			DWIN_MSG_P(DWIN_MSG_PRINTING);
			returnDefaultButtonAction();
		}
	#else
		#if ENABLED(PARK_HEAD_ON_PAUSE)
			enqueue_and_echo_commands_P(PSTR("M24"));
		#else
			FILE_START_PRINT;
			print_job_timer.start();
		#endif
		returnDefaultButtonAction();
	#endif //QUICK_PAUSE
#endif
}

inline void printCancelButtonAction(){
#if HAS_READER
	#if ENABLED(QUICK_PAUSE)
		quickStopPrintJob();
		DWIN_MSG_P(DWIN_MSG_WELCOME);
		returnDefaultButtonAction();
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
		returnDefaultButtonAction();
	#endif //QUICK_PAUSE
#endif
}

inline void preheatButtonAction(){
	dwin_preheat(0);
}

inline void cooldownButtonAction(){
	dwin_cooldwon();
}

inline void openFileButtonAction(){
	lcdUpdateFiles(OPEN);
}

inline void openFilamentPageAction(){
	GO_PAGE(PAGE_PARSE(PAGE_FILAMENT));
}

inline void printChangeButtonAction(){
#ifdef FILAMENT_CHANGE
	if(!HAS_POPUP
	#ifdef ACCIDENT_DETECT
			&& !isAccidentToPrinting
	#endif
			){
		if(pauseToUnloadFilament()){
			SERIAL_ECHOLNPGM(MSG_FILAMENT_NOT_READY);
			DWIN_MSG_P(DWIN_MSG_FILAMENT_NOT_READY);
		}
	}
#endif
}

inline void zUpButtonAction(){
//	dwin_move(Z_AXIS, DWIN_Z_MOVE_SCALE, false);
	dwin_move(Z_AXIS, 1, false);
}

inline void zDownButtonAction(){
//	dwin_move(Z_AXIS, DWIN_Z_MOVE_SCALE, true);
	dwin_move(Z_AXIS, 1, true);
}

inline void settingButtonAction(){
#if HAS_LEVELING
	#if ENABLED(WIFI_SUPPORT)
		GO_PAGE(PAGE_SETTING_WIFI_LEVELING);
	#else
		GO_PAGE(PAGE_SETTING_LEVELING);
	#endif
#else
	#if ENABLED(WIFI_SUPPORT)
		GO_PAGE(PAGE_SETTING_WIFI);
	#else
		GO_PAGE(PAGE_SETTING);
	#endif
#endif
}

inline void adjustButtonAction(){
	GO_PAGE(PAGE_ADJUST);
}

inline void regButtonAction(){
#ifdef REG_SN
	GO_PAGE(PAGE_REG);
#endif
}

inline void HomeAllAction(){
	dwin_move_home(HOME_ALL);
}

inline void moveHomeXAction(){
	dwin_move_home(HOME_X);
}

inline void moveHomeYAction(){
	dwin_move_home(HOME_Y);
}

inline void moveHomeZAction(){
	dwin_move_home(HOME_Z);
}

inline void moveXPlusAction(){
	dwin_move(X_AXIS, true);
}

inline void moveYPlusAction(){
	dwin_move(Y_AXIS, true);
}

inline void moveZPlusAction(){
	dwin_move(Z_AXIS, true);
}

inline void moveXMinusAction(){
	dwin_move(X_AXIS, false);
}

inline void moveYMinusAction(){
	dwin_move(Y_AXIS, false);
}

inline void moveZMinusAction(){
	dwin_move(Z_AXIS, false);
}

inline void filamentExtrudeAction(){
	dwin_filament_test(true);
}

inline void filamentRetractAction(){
	dwin_filament_test(false);
}

inline void filamentUnloadAction(){
	dwin_filament_unload();
}

inline void storeSettingsAction(){
  POP_WINDOW(POP_ICO_SETTING_SAVING, POPUP_DELAY_500MS);
  settings.save();
}

inline void resetSettingsAction(){
	POP_WINDOW(POP_ICO_SETTING_RESTORING, POPUP_DELAY_500MS);
	settings.reset();
#ifdef WIFI_MODULE_ESP8266_12
	myWifi.resetWifiInfo();
#endif
}

inline void jumpSettingMotionAction(){
  GO_PAGE(PAGE_MOTION_SETTING);
}

inline void jumpSettingFanAction(){
#ifdef HAS_AIR_FAN
	GO_PAGE(PAGE_FANSPEED_SETTING_FILTER);
#else
	GO_PAGE(PAGE_FANSPEED_SETTING);
#endif
}

inline void jumpSettingPreheatAction(){
#ifdef HOTWIND_SYSTEM
	GO_PAGE(PAGE_PREHEAT_SETTING_CHAMBER);
#else
	GO_PAGE(PAGE_PREHEAT_SETTING);
#endif
}

inline void jumpSettingWifiAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	if(myWifi.isOpen()){
		GO_PAGE(PAGE_WIFI_SETTING_ON);
	}else{
		GO_PAGE(PAGE_WIFI_SETTING_OFF);
	}
	if(myWifi.isAction(ACTION_INIT) && !HAS_POPUP){
    POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_ALWAY_DELAY);
	}
#elif defined(WIFI_MODULE_ESP8266_12)
  GO_PAGE(PAGE_CONNECT_WIFI_12);
#endif
#endif
}

inline void jumpSettingLevelingAction(){
#if HAS_LEVELING
	#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
		if(leveling_is_active()){
			if(leveling_is_valid()){
				if(FILE_IS_IDLE && !planner.blocks_queued() && !isSerialPrinting){		// no move.
					GO_PAGE(PAGE_LEVELING_SETTING_ON);
				}else{
					GO_PAGE(PAGE_LEVELING_SETTING_DISABLE);
				}
			}
		}else{
			GO_PAGE(PAGE_LEVELING_SETTING_OFF);
		}
	#else
		// Other leveling
	#endif
#endif
}

inline void fileNextPageAction(){
	lcdUpdateFiles(NEXT_PAGE);
}

inline void fileLastPageAction(){
	lcdUpdateFiles(LAST_PAGE);
}

inline void fileUpLevelAction(){
	lcdUpdateFiles(UP_LEVEL);
}

void dwin_update_wifi_icon();

inline void wifiSwitchToggleAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
  if(myWifi.isOpen()){
    POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_200MS);
    myWifi.ON_OFF(false);
  } else{
    POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_ALWAY_DELAY);
    myWifi.ON_OFF(true);
  }
#endif
#endif
}

inline void wifiModeStaAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
  if(myWifi.getWifiMode() != MODE_STA){
    myWifi.actionSetWifiMode(MODE_STA);
    POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_ALWAY_DELAY);
  }
#endif
#endif
}

inline void wifiModeApAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
  if(myWifi.getWifiMode() != MODE_AP){
    myWifi.actionSetWifiMode(MODE_AP);
    POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_200MS);
  }
#endif
#endif
}

inline void wifiSwitchModeAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	myWifi.actionSwitchWifiMode();
  POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_200MS);
#endif
#endif
}

inline void wifiResetAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	myWifi.actionWifiReset();
  POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_ALWAY_DELAY);
#endif
#endif
}

inline void wifiScanAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	myWifi.actionScanAP(SCAN_0);
  POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_ALWAY_DELAY);
#endif
#endif
}

inline void wifiNextPageAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	myWifi.actionScanAP(SCAN_NEXT);
  POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_ALWAY_DELAY);
#endif
#endif
}

inline void wifiLastPageAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	myWifi.actionScanAP(SCAN_LAST);
  POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_ALWAY_DELAY);
#endif
#endif
}

inline void wifiSettingEntryAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	if(myWifi.getWifiMode() == MODE_AP){
		GO_PAGE(PAGE_WIFI_INFO_AP);
	}else if(myWifi.getWifiMode() == MODE_STA){
		GO_PAGE(PAGE_WIFI_INFO_STATION);
//	}else if(myWifi.getWifiMode() == MODE_BOTH){
//		//TODO WIFI
//		SERIAL_ECHOLN("TO DO");
	}else{
		//TODO WIFI
		SERIAL_ECHOLN("TODO");
	}
#endif
#endif
}

inline void levelingEnableAction(){
#if HAS_LEVELING
	set_bed_leveling_enabled(true);
	jumpSettingLevelingAction();
#endif
}

inline void levelingSwitchToggleAction(){
#if HAS_LEVELING
#ifdef DWIN_USE_OS
	if(leveling_is_active()){
		set_bed_leveling_enabled(false);
	} else {
		if(leveling_is_valid()){
			if(FILE_IS_IDLE && !planner.blocks_queued() && !isSerialPrinting){		// no move.
				POP_WINDOW(POP_KEY_PROBE_CONFIRM, POPUP_DELAY_ALWAY_DELAY);
			} else {
				levelingEnableAction();
			}
		} else {
			POP_WINDOW(POP_ICO_INVALID_LEVLEL_DATA, POPUP_DELAY_1S);
			if(FILE_IS_IDLE && !planner.blocks_queued() && !isSerialPrinting){		// no move.
				enqueue_and_echo_commands_P(PSTR("M6014"));
			} else {
				POP_WINDOW(POP_ICO_PROBER_UNAVAILABLE, POPUP_DELAY_500MS);
			}
		}
	}
	jumpSettingLevelingAction();
#else //!DWIN_USE_OS
	if(leveling_is_valid()){
		bool allowLeveling = leveling_is_active();
		set_bed_leveling_enabled(!allowLeveling);
		EEPROM_STORE(planner.abl_enabled, mesh_bilinear_ubl_status);
		jumpSettingLevelingAction();
	}
#endif //DWIN_USE_OS
#endif //HAS_LEVELING
}

inline void levelingProbeAction(){
#if HAS_BED_PROBE
	if(FILE_IS_IDLE && !planner.blocks_queued() && !isSerialPrinting){
		enqueue_and_echo_commands_P(PSTR("M6014"));
	}
#endif
}

inline void wifiItemAction(uint8_t index){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
  //#define CENTER_WIFI_SELECT
  #ifdef CENTER_WIFI_SELECT
    char name[WIFI_SSID_SIZE] = { 0 };
    strcpy(name, myWifi.getWifiInfo(index)->ssid);

    if(name[0]){
      dwinWifiConnIndex = index;

      // show the string in the center of LCD.
      centerString(name, LCD_WIFI_CHAR_LEN);
      SET_STR(WIFI_SELECT, LCD_WIFI_CHAR_LEN, name);

      GO_PAGE(PAGE_CONNECT_WIFI_01);
    }
  #else //!CENTER_WIFI_SELECT
    char *name = myWifi.getWifiInfo(index)->ssid;
    if(name[0]){
      dwinWifiConnIndex = index;
      SET_STR(WIFI_SELECT, LCD_WIFI_CHAR_LEN, name);
      GO_PAGE(PAGE_CONNECT_WIFI_01);
    }
  #endif //CENTER_WIFI_SELECT
#else
  UNUSED(index);
#endif
#else
	UNUSED(index);
#endif
}

inline void fileOpenAction(){
	afterFileItemAction(OPEN);
}

inline void filePrintAction(){
	afterFileItemAction(PRINT);
}

inline void fileCancelAction(){
	afterFileItemAction(CANCEL);
}

inline void shuttingCancelAction(){
#if FAN_COUNT > 0
	for(uint8_t i = 0; i < FAN_COUNT; i++)
		fanSpeeds[i] = 0;
#endif
	returnDefaultButtonAction();
}

inline void accidentReuseAction(){
#ifdef ACCIDENT_DETECT
//	returnDefaultButtonAction();
	accidentToResume_Home();
#endif
}

inline void accidentCancelAction(){
#ifdef ACCIDENT_DETECT
	returnDefaultButtonAction();
	if(isAccident){
		isAccident = false;
		STORE_SETTING(isAccident);
	}
#endif
}

inline void changeDoneAction(){
#ifdef FILAMENT_CHANGE
	HIDE_POPUP;	// dummy hide popup, as the popup is disappear by touch the "resume" button in dwin lcd.
	printReuseButtonAction();
#endif
}

inline void onlineReturnAction(){

}

inline void wifiConnectAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
  myWifi.actionJoinAP(dwinWifiConnIndex, dwinWifiConnKey);
  POP_WINDOW(POP_ICO_WAITING, POPUP_DELAY_ALWAY_DELAY);
	wifiSettingEntryAction();
#elif defined(WIFI_MODULE_ESP8266_12)
	if(strlen(dwinWifiConnSSID)){
	  myWifi.actionJoinAP(dwinWifiConnSSID, dwinWifiConnKey);
	}
#endif
#endif
}

inline void wifiCancelAction(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	wifiSettingEntryAction();
#elif defined(WIFI_MODULE_ESP8266_12)
	settingButtonAction();
#endif
#endif
}

/*******************************************************************************************************/

/*
void setupDEMO(){
	SET_INT(COLOR_BLACK,	"0000");
	SET_INT(COLOR_WHITE,	"FFFF");
	SET_INT(COLOR_RED,		"F800");
	SET_INT(COLOR_ORANGE,	"FC00");
	SET_INT(COLOR_YELLOW,	"FFE0");
	SET_INT(COLOR_GREEN,	"07E0");
	SET_INT(COLOR_CYAN,		"07FF");
	SET_INT(COLOR_BLUE,		"001F");
	SET_INT(COLOR_PURPLE,	"F81F");
}
*/

/********************************************* View Part ***********************************************/

void dwin_update_msg(const char* msg){
  if(powerState <= POWER_COOLING) return;
#ifdef DWIN_CENTER_MSG
	char msg_center[LCD_MSG_CHAR_LEN + 1];
	strcpy(msg_center, msg);
	centerString(msg_center, LCD_MSG_CHAR_LEN);
	SET_STR(MSG_ADDR, LCD_MSG_CHAR_LEN, msg_center);
#else
	SET_STR(MSG_ADDR, LCD_MSG_CHAR_LEN, msg);
#endif
	SET_STR(MSG_ADDR_SHORT, LCD_MSG_CHAR_LEN, msg);
}

void dwin_update_msg_P(const char* msg){
	char real_msg[LCD_MSG_CHAR_LEN + 1] = { 0 };
	strncpy_P(real_msg, msg, LCD_MSG_CHAR_LEN);
	dwin_update_msg(real_msg);
}

void updateFanSpeed(uint8_t mode){
	#define _FAN_SPEED_UPDATE(fan, addr) do{\
			SET_INT(addr, (fan + 1) * 100 / 255);\
		} while(0);
	#define FAN_SPEED_UPDATE(speed, addr)	_FAN_SPEED_UPDATE(speed, addr)

	if(mode == GLOBAL_FAN){
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
//	else if (mode == PREHEAT_FAN) {
//		FAN_SPEED_UPDATE(lcd_preheat_fan_speed[PLA], PREHEAT_FAN_SPEED_ADDR);
//	}
	else if(mode == TEMP_FAN){
		FAN_SPEED_UPDATE(extruder_auto_fan_speed, TEMP_FAN_SPEED_ADDR);
	}
#ifdef HAS_AIR_FAN
	else if(mode == AIR_FAN){
		FAN_SPEED_UPDATE(air_fan_speed, AIR_FAN_SPEED_ADDR);
	}
#endif
}

void dwin_update_version(){
	char *ver = &versionFW[1];
	SET_STR(VER_ADDR, LCD_VER_CHAR_LEN, ver);
	SET_STR(UI_ADDR, LCD_UI_CHAR_LEN, DWIN_VER);
}

void dwin_update_reg_icon(){
#ifdef REG_SN
  uint8_t regState;
  regState = REG_PASS ? 1 : 0;
  UPDATE_LCD(regState, REG_STATE_INDEX_ADDR);
#endif
}

void dwin_update_wifi_home_icon(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
  UPDATE_LCD(myWifi.getWifiMode(), WIFI_HOME_ICO);
#elif defined(WIFI_MODULE_ESP8266_12)
  UPDATE_LCD(myWifi.getWifiCode(), WIFI_HOME_ICO);
#endif
#endif
}

void dwin_update_wifi_icon(){
#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	UPDATE_LCD(myWifi.getWifiMode(), WIFI_MODE_INDEX_ADDR);
#elif defined(WIFI_MODULE_ESP8266_12)
	uint8_t wifiState = 0;
	switch(myWifi.getWifiCode()){
	  case WifiNoConnect:
	    wifiState = 0;
	    break;
	  case WifiConnecting:
      wifiState = 1;
	    break;
	  case WifiRssi1:
	  case WifiRssi2:
	  case WifiRssi3:
	  case WifiRssi4:
      wifiState = 2;
	    break;
	  case WifiSsidNoFind:
      wifiState = 4;
	    break;
	  case WifiKeyErr:
      wifiState = 3;
	    break;
	  default:
	    wifiState = 0;
	}
  UPDATE_LCD(wifiState, WIFI_STATE_INDEX_ADDR);
#endif
#endif 
}

void dwin_update_wifi_uuid(const char* uuid){
  SET_STR(WIFI_UUID_ADDR, LCD_WIFI_UUID_LEN, uuid);
}

void dwin_update_index(){
	move_distance_index = 4;	// 100mm
#if EXTRUDERS > 2
	extrude_index = 4;
#elif EXTRUDERS > 1
	extrude_index = 2;
#elif EXTRUDERS > 0
	extrude_index = 1;
#endif
	extrude_distance = 30;		// 30mm

	UPDATE_LCD(move_distance_index, MOVE_DISTANCE_INDEX_ADDR);
	UPDATE_LCD(extrude_index, FILAMENT_EXT_INDEX_ADDR);
	UPDATE_LCD(extrude_distance, FILAMENT_EXT_DISTANCE_ADDR);

	dwin_update_reg_icon();
	dwin_update_wifi_icon();
	dwin_update_wifi_home_icon();
}

void dwin_update_strings(){
	SET_STR(X_STR_ADDR, 2, "X:");
	SET_STR(Y_STR_ADDR, 2, "Y:");
	SET_STR(Z_STR_ADDR, 2, "Z:");
  SET_STR(WIFI_KEY_MASK_ADDR, 16, "****************");
	updateStateStrings();
}

void dwin_update_temp_info(){
	//target_temperature
	UPDATE_LCD(thermalManager.degTargetHotend(0), EX0_TAR_ADDR);
#if EXTRUDERS > 1
	UPDATE_LCD(thermalManager.degTargetHotend(1), EX1_TAR_ADDR);
#endif
#if EXTRUDERS > 2
	UPDATE_LCD(thermalManager.degTargetHotend(2), EX2_TAR_ADDR);
#endif
	UPDATE_LCD(thermalManager.degTargetBed(), BED_TAR_ADDR);
#ifdef HOTWIND_SYSTEM
	UPDATE_LCD(thermalManager.degTargetChamber(), CHAMBER_TAR_ADDR);
#endif

	//current_temperature
	UPDATE_LCD((int16_t )thermalManager.degHotend(0), EX0_CUR_ADDR);
#if EXTRUDERS > 1
	UPDATE_LCD((int16_t)thermalManager.degHotend(1), EX1_CUR_ADDR);
#endif
#if EXTRUDERS > 2
	UPDATE_LCD((int16_t)thermalManager.degHotend(2), EX2_CUR_ADDR);
#endif
	UPDATE_LCD((int16_t )thermalManager.degBed(), BED_CUR_ADDR);
#ifdef HOTWIND_SYSTEM
	UPDATE_LCD((int16_t)thermalManager.degChamber(), CHAMBER_CUR_ADDR);
#endif
}

void dwin_update_adjust_info(){
	//global_speed  & fan_speed & extrude_multiply
	UPDATE_LCD(feedrate_percentage, SPEED_GLOBAL_ADDR);
	UPDATE_LCD(flow_percentage[active_extruder], EXT_MULTIPLY_ADDR);
	updateFanSpeed(GLOBAL_FAN);
}

void dwin_update_axis_info(){
	//current_position
	UPDATE_LCD_42(current_position[X_AXIS], X_AXIS_ADDR);
	UPDATE_LCD_42(current_position[Y_AXIS], Y_AXIS_ADDR);
	UPDATE_LCD_42(current_position[Z_AXIS], Z_AXIS_ADDR);
}

void dwin_update_max_temp_info(){
	//current_max_temperature
	UPDATE_LCD(thermalManager.maxDegHotend(), CUR_MAX_TEMP_ADDR);
}

void dwin_update_setting_motion_info(){
	UPDATE_LCD_34(planner.axis_steps_per_mm[X_AXIS], X_STEP_ADDR);
	UPDATE_LCD_34(planner.axis_steps_per_mm[Y_AXIS], Y_STEP_ADDR);
	UPDATE_LCD_43(planner.axis_steps_per_mm[Z_AXIS], Z_STEP_ADDR);
	UPDATE_LCD_34(planner.axis_steps_per_mm[E_AXIS], E_STEP_ADDR);
}

void dwin_update_setting_fan_info(){
	updateFanSpeed(GLOBAL_FAN);
	updateFanSpeed(TEMP_FAN);
#ifdef HAS_AIR_FAN
	updateFanSpeed(AIR_FAN);
#endif
}

void dwin_update_setting_preheat_info(){
	updateFanSpeed(PREHEAT_FAN);
	UPDATE_LCD(lcd_preheat_hotend_temp[0], PREHEAT_EXT_TEMP_ADDR_0);
#if EXTRUDERS > 1
	UPDATE_LCD(lcd_preheat_hotend_temp[1], PREHEAT_EXT_TEMP_ADDR_1);
#endif
#if EXTRUDERS > 2
	UPDATE_LCD(lcd_preheat_hotend_temp[2], PREHEAT_EXT_TEMP_ADDR_2);
#endif
	UPDATE_LCD(lcd_preheat_bed_temp, PREHEAT_BED_TEMP_ADDR);
#ifdef HOTWIND_SYSTEM
	UPDATE_LCD(lcd_preheat_chamber_temp, PREHEAT_CHAMBER_TEMP_ADDR);
#endif
}

#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
void dwin_update_wifi_list_info(){
  if(myWifi.getWifiMode() == MODE_STA){
    #define WIFI_INFO(index) do{\
      SET_STR(WIFI_ITEM_STR_ ## index, LCD_WIFI_CHAR_LEN, myWifi.getWifiInfo(index)->ssid);\
      UPDATE_LCD(myWifi.getWifiInfo(index)->level, WIFI_ITEM_ICO_ ## index);\
    }while(0);

    WIFI_INFO(0);
    WIFI_INFO(1);
    WIFI_INFO(2);
    WIFI_INFO(3);
    WIFI_INFO(4);
  }
}
#endif

void dwin_update_station_info(const char* ssid, const char* key){
#ifdef WIFI_MODULE_ESP8266_12
  strcpy(dwinWifiConnSSID, ssid);
  strcpy(dwinWifiConnKey, key);
  SET_STR(WIFI_SSID_SET_ADDR, LCD_HOST_CHAR_LEN, ssid);
  SET_STR(WIFI_KEY_ADDR, LCD_HOST_CHAR_LEN, key);
#endif
}

void dwin_update_setting_wifi_info(){
  char ip_str[16] = { 0 };
  uint32_t ip_num = myWifi.getIP();
  if(ip_num){
    sprintf_P(ip_str, PSTR("%d.%d.%d.%d"), (int(ip_num >> 24)) & 0x00FF, (int(ip_num >> 16)) & 0x00FF,
               (int(ip_num >> 8)) & 0x00FF, (int(ip_num)) & 0x00FF);
  }else{
    strcpy_P(ip_str, PSTR("000.000.000.000"));
  }
  SET_STR(WIFI_IP_ADDR, 32, ip_str);
#ifdef WIFI_MODULE_ESP8266_01
  SET_STR(WIFI_SSID_ADDR, LCD_HOST_CHAR_LEN, myWifi.getSSID());
#endif
}
#endif

#if HAS_LEVELING
void dwin_update_setting_leveling_info(){
	#if HAS_BED_PROBE
		UPDATE_LCD_22(-zprobe_zoffset, SERVO_Z_OFFSET_ADDR);
	#endif

	#if	ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
		UPDATE_LCD_31(planner.z_fade_height, FADE_HEIGHT_ADDR);
	#endif
}
#endif

void dwin_update_time_info(uint8_t mode){
	char time_str[11] = { 0 };

	if(mode == TIME_USED){
		(duration_t(usedTime)).toTimeDWIN(time_str, 4);
		setValueAsAttr_P(USED_TIME_ADDR, 0, time_str);
	}else if(mode == TIME_PRINT){
		(duration_t(print_job_timer.duration())).toTimeDWIN(time_str, 3);
		setValueAsAttr_P(PRINT_TIME_ADDR, 0, time_str);
	}
#ifdef REG_SN
	else if(mode == TIME_TRIAL) {
		float trialTime = TOTAL_TIME_LIMIT - usedTime;
		trialTime = (trialTime <= 0) ? 0 : trialTime;
		(duration_t(trialTime)).toTimeDWIN(time_str, 4);
		setValueAsAttr_P(TRIAL_PERIOD_ADDR, 0, time_str);
	}
#endif
}

#ifdef REG_SN
void dwin_update_reg_info(){
	dwin_update_time_info(TIME_TRIAL);
#ifdef REG_USE_HARDWARE
	SET_STR(SN_CAHR_ADDR, LCD_SN_CHAR_LEN, reg_c);
#endif
	UPDATE_LCD_32(regSN, REG_KEY_ADDR);
	dwin_update_reg_icon();
}
#endif

void dwin_update_progress_info(){
#if HAS_READER
	float percent = FILE_READER.percentDoneF();
	UPDATE_LCD_31(percent, PERCENT_ADDR);

	FILL_REC(PAINT_TOOL1_ADDR, PROGRESS_BAR_X0, PROGRESS_BAR_Y0, PROGRESS_BAR_X1, PROGRESS_BAR_Y1, COLOR_ASSIST);
	uint16_t p0 = PROGRESS_BAR_X0 + uint32_t((PROGRESS_BAR_X1 - PROGRESS_BAR_X0) * percent) / 100;

	if(!FILE_IS_IDLE){
	  if(p0){ //如果右下角的x坐标为0,T5L屏幕将显示异常,画面铺满整个屏幕
	    FILL_REC(PAINT_TOOL2_ADDR, PROGRESS_BAR_X0, PROGRESS_BAR_Y0, p0, PROGRESS_BAR_Y1, COLOR_PRIMARY);
	  }else{
	    CLEAR_CANVAS(PAINT_TOOL2_ADDR);
	  }
		dwin_update_time_info(TIME_PRINT);
	}else{
		DRAW_REC(PAINT_TOOL2_ADDR, PROGRESS_BAR_X0, PROGRESS_BAR_Y0, PROGRESS_BAR_X1, PROGRESS_BAR_Y1, COLOR_ASSIST);
	}
#endif //HAS_READER
}

/*******************************************************************************************************/

/********************************************* Other Part **********************************************/

static void updateData();

void dwin_setup(){
#if HAS_READER
	#if ENABLED(SDSUPPORT) && PIN_EXISTS(SD_DETECT)
		SET_INPUT_PULLUP(SD_DETECT_PIN);
	#endif
	dwinReaderState = 0xFF;					// unknown Reader state.
#endif

	dwin_data_state = DATA_UNINIT;	// dwin lcd uninit
	dwin_next_update_millis = 0;		// next data update time
	return_default_page_time = 0;		// return the default page time.
	lock_page_time = 0;							// lock page time.

	dwin_init();										// the implement init.
}

bool dwin_is_page_P(const char* page){
  return isPage_P(page);
}

void dwin_change_page_P(const char* page){
	if(HAS_POPUP)		HIDE_POPUP;

  // update the data of page in lcd before goto the page.
	uint16_t prePage = currentPage(); char rel_page[5];
	forceSetPage((uint16_t)strtoul(strncpy_P(rel_page, page, 5), nullptr, HEX));
	updateData();
	forceSetPage(prePage);

	setPage_P(page);
}

// execute the cmd from dwin lcd.
void updateCmd(uint16_t cmdValue){
	if(cmdValue){
		SET_INT(CMD_ADDR, CMD_EMPTY);

		switch(cmdValue){
		case RETURN_UP_LEVEL_BUTTON:
			returnUpLevelButtonAction();
			break;
		case RETURN_DEFAULT_BUTTON:
			returnDefaultButtonAction();
			break;

		case PAUSE_BUTTON:
			printPauseButtonAction();
			break;
		case REUSE_BUTTON:
			printReuseButtonAction();
			break;
		case CANCEL_BUTTON:
		case STOP_PRINT_CONFIRM:
			printCancelButtonAction();
			break;
		case PREHEAT_BUTTON:
			preheatButtonAction();
			break;
		case COOLDOWN_BUTTON:
			cooldownButtonAction();
			break;
		case OPEN_FILE_BUTTON:
			openFileButtonAction();
			break;
		case OPEN_FILAMENT_BUTTON:
			openFilamentPageAction();
			break;
		case CHANGE_BUTTON:
			printChangeButtonAction();
			break;
		case Z_UP_BUTTON:
			zUpButtonAction();
			break;
		case Z_DOWN_BUTTON:
			zDownButtonAction();
			break;
		case SETTING_BUTTON:
			settingButtonAction();
			break;
		case ADJUST_BUTTON:
			adjustButtonAction();
			break;

		case REG_INFO_BUTTON:
			regButtonAction();
			break;

		case MOVE_HOME_ALL:
			HomeAllAction();
			break;
		case MOVE_HOME_X:
			moveHomeXAction();
			break;
		case MOVE_HOME_Y:
			moveHomeYAction();
			break;
		case MOVE_HOME_Z:
			moveHomeZAction();
			break;
		case MOVE_X_PLUS:
			moveXPlusAction();
			break;
		case MOVE_Y_PLUS:
			moveYPlusAction();
			break;
		case MOVE_Z_PLUS:
			moveZPlusAction();
			break;
		case MOVE_X_MINUS:
			moveXMinusAction();
			break;
		case MOVE_Y_MINUS:
			moveYMinusAction();
			break;
		case MOVE_Z_MINUS:
			moveZMinusAction();
			break;

		case FILAMENT_EXT:
			filamentExtrudeAction();
			break;
		case FILAMENT_REC:
			filamentRetractAction();
			break;
		case FILAMENT_UNLOAD:
			filamentUnloadAction();
			break;

		case STORE_SETTINGS:
			storeSettingsAction();
			break;
		case RESET_SETTINGS:
		case RESET_SETTINGS_CONFIRM:
			resetSettingsAction();
			break;
		case JUMP_SETGING_MOTION:
			jumpSettingMotionAction();
			break;
		case JUMP_SETTING_FAN:
			jumpSettingFanAction();
			break;
		case JUMP_SETTING_PREHEAT:
			jumpSettingPreheatAction();
			break;
		case JUMP_SETTING_WIFI:
			jumpSettingWifiAction();
			break;
		case JUMP_SETTING_LEVELING:
			jumpSettingLevelingAction();
			break;

		case FILE_NEXT_PAGE:
			fileNextPageAction();
			break;
		case FILE_LAST_PAGE:
			fileLastPageAction();
			break;
		case FILE_UP_LEVEL:
			fileUpLevelAction();
			break;

		case WIFI_SWITCH_TOGGLE:
			wifiSwitchToggleAction();
			break;
//		case WIFI_SWITCH_MODE:
//			wifiSwitchModeAction();
//			break;
		case WIFI_RESET:
			wifiResetAction();
			break;
		case WIFI_SCAN:
			wifiScanAction();
			break;
		case WIFI_NEXT_PAGE:
			wifiNextPageAction();
			break;
		case WIFI_LAST_PAGE:
			wifiLastPageAction();
			break;
		case WIFI_MODE_STA:
			wifiModeStaAction();
			break;
		case WIFI_MODE_AP:
			wifiModeApAction();
			break;
		case WIFI_SETTING_ENTRY:
			wifiSettingEntryAction();
			break;

		case LEVELING_SWITCH_TOGGLE:
			levelingSwitchToggleAction();
			break;
		case LEVELING_PROBE:
		case LEVELING_AGAIN_CONFIRM:
			levelingProbeAction();
			break;
		case LEVELING_SKIP_CONFIRM:
			levelingEnableAction();
			break;


		case FILE_ITEM_0_ACTION:
			fileItemAction(0);
			break;
		case FILE_ITEM_1_ACTION:
			fileItemAction(1);
			break;
		case FILE_ITEM_2_ACTION:
			fileItemAction(2);
			break;
		case FILE_ITEM_3_ACTION:
			fileItemAction(3);
			break;
		case FILE_ITEM_4_ACTION:
			fileItemAction(4);
			break;
		case FILE_ITEM_5_ACTION:
			fileItemAction(5);
			break;
		case FILE_ITEM_6_ACTION:
			fileItemAction(6);
			break;
		case FILE_ITEM_7_ACTION:
			fileItemAction(7);
			break;
		case FILE_ITEM_8_ACTION:
			fileItemAction(8);
			break;

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

//		case FILE_ACTION_OPEN:
//			fileOpenAction();
//			break;
		case FILE_ACTION_PRINT:
			filePrintAction();
			break;
		case FILE_ACTION_CANCEL:
			fileCancelAction();
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
//		case ONLINE_RETURN:
//			onlineReturnAction();
//			break;
		case WIFI_CONNECT:
			wifiConnectAction();
			break;
		case WIFI_CANCEL:
			wifiCancelAction();
			break;

		default:
			//SERIAL_ECHOLN(cmdValue);
			break;
		}
	}
}

void resetTimeout(){
	unsigned long curTime = millis();
	return_default_page_time = curTime + LCD_TIMEOUT_TO_STATUS;
	lock_page_time = curTime + LCD_TIMEOUT_TO_LOCK;
}

#ifdef DWIN_USE_OS
void dwin_hide_popup(){
	setPop((uint16_t)POPUP_INDEX_HIDE_POP, POPUP_DELAY_NO_DELAY);
	resetTimeout();
}
#endif
/*******************************************************************************************************/


// scan the dwin lcd to receive the possible BackData and check if connect ready.
static void dwin_scan(){
	dwin_loop();

	if(dwin_isExist()){
	  uint16_t curPage = currentPage();
		if((curPage + 1)){		// current page is valid.
      millis_t ms = millis();

		  //页面跳转和数据更新
			switch(dwin_data_state){
			  case DATA_UNINIT:
          if(curPage > LCD_SETUP_LOOP_END){
            if(ms > dwin_next_update_millis){
              dwin_next_update_millis = ms + LCD_RUN_CYCLE * 2;
              GO_PAGE(LCD_SETUP_FIRST);
            }
          }
			    break;
			  case DATA_INIT:
          if(ms > dwin_next_update_millis){
            dwin_next_update_millis = ms + LCD_RUN_CYCLE * LCD_SETUP_NUM;

          #ifdef WIFI_MODULE_ESP8266_12
            myWifi.getStationInfo();
          #endif

            enableTouch();
            dwin_update_version();
            dwin_update_index();
            dwin_update_strings();
          }
          break;
			  case DATA_GIF_START:
			    break;
			  case DATA_GIF_END:
          if(ms > dwin_next_update_millis){
            dwin_next_update_millis = ms + LCD_RUN_CYCLE * 2;
            returnDefaultButtonAction();
          }
			    break;
			  case DATA_READY:
          break;
			  default:
			    break;
			}

			//状态切换
			switch(dwin_data_state){
			  case DATA_UNINIT:
			    if(curPage <= LCD_SETUP_LOOP_END){
	          dwin_next_update_millis = 0;
	          dwin_data_state = DATA_INIT;
			    }
          break;
			  case DATA_INIT:
			    if(curPage == LCD_SETUP_LOOP_STRAT){
            dwin_next_update_millis = 0;
            dwin_data_state = DATA_GIF_START;
			    }
			  case DATA_GIF_START:
          if(curPage == LCD_SETUP_LOOP_END){
            dwin_next_update_millis = 0;
            dwin_data_state = DATA_GIF_END;
          }
			  case DATA_GIF_END:
          if(curPage > LCD_SETUP_LOOP_END){
            dwin_next_update_millis = ms + 1000L;
            dwin_data_state = DATA_READY;
            resetTimeout();
          }
          break;
        case DATA_READY:
          if(curPage <= LCD_SETUP_LOOP_END){
            dwin_next_update_millis = 0;
            dwin_data_state = DATA_UNINIT;
          }
          break;
        default:
          break;
			}
		}
	}else{
		dwin_data_state = DATA_UNINIT;
	}
}

// resolve the back data.
static void resolveVar(){

#define VAR_IS_ADDR(addr) (uint16_tCompHex_P(addr, dwin_getVar()->varAddr))
#define VAR_IS_LEN(len)   (dwin_getVar()->dataLen == len)
#define IS_VAR(len, addr) (VAR_IS_LEN(len) && VAR_IS_ADDR(addr))

#ifdef WIFI_SUPPORT
  if(VAR_IS_ADDR(WIFI_KEY_ADDR) || VAR_IS_ADDR(WIFI_SSID_SET_ADDR) ){
    uint8_t i;
    for(i = 0; dwin_getVar()->data[i] != 0xFF; i++);
    dwin_getVar()->data[i] = 0x00;
    if(VAR_IS_ADDR(WIFI_KEY_ADDR)){
      strcpy(dwinWifiConnKey, (char *)dwin_getVar()->data);
    }
  #ifdef WIFI_MODULE_ESP8266_12
    else {
      strcpy(dwinWifiConnSSID, (char *)dwin_getVar()->data);
    }
  #endif
  } else
#endif //WIFI_SUPPORT
	if(VAR_IS_LEN(2)){
		uint16_t varValue = ((uint16_t)dwin_getVar()->data[0] << 8) | ((uint16_t)dwin_getVar()->data[1]);

		if(VAR_IS_ADDR(CMD_ADDR)){								// dwin_700_cmd
			updateCmd(varValue);
		}else if(VAR_IS_ADDR(MOVE_DISTANCE_INDEX_ADDR)){			// axis_move_distance
			move_distance_index = (uint8_t)varValue;
		}else if(VAR_IS_ADDR(FILAMENT_EXT_DISTANCE_ADDR)){		// filament_test_distance
			extrude_distance = varValue;
		}else if(VAR_IS_ADDR(FILAMENT_EXT_INDEX_ADDR)){				// extruder index
		  dwin_update_hotend(varValue);
		}else if(VAR_IS_ADDR(SET_EX0_TAR_ADDR)){							// extruder 0 target temp.
			NOMORE(varValue, HEATER_0_MAXTEMP - 15);
			thermalManager.setTargetHotend(varValue, 0);
			returnDefaultButtonAction();
		}
	#if EXTRUDERS > 1
		else if (VAR_IS_ADDR(SET_EX1_TAR_ADDR)){							// extruder 1 target temp.
			NOMORE(varValue, HEATER_1_MAXTEMP - 15);
			thermalManager.setTargetHotend(varValue, 1);
			returnDefaultButtonAction();
		}
	#endif
	#if EXTRUDERS > 2
		else if (VAR_IS_ADDR(SET_EX2_TAR_ADDR)){							// extruder 2 target temp.
			NOMORE(varValue, HEATER_2_MAXTEMP - 15);
			thermalManager.setTargetHotend(varValue, 2);
			returnDefaultButtonAction();
		}
	#endif
		else if(VAR_IS_ADDR(SET_BED_TAR_ADDR)){							// bed target temp.
			NOMORE(varValue, BED_MAXTEMP - 15);
			thermalManager.setTargetBed(varValue);
			returnDefaultButtonAction();
		}
	#ifdef HOTWIND_SYSTEM
		else if(VAR_IS_ADDR(SET_CHAMBER_TAR_ADDR)){						// chamber target temp.
			NOMORE(varValue, CHAMBER_MAXTEMP - 5);
			thermalManager.setTargetChamber(varValue);
			returnDefaultButtonAction();
		}
	#endif
		else if(VAR_IS_ADDR(SET_SPEED_GLOBAL_ADDR)){					// print speed
			feedrate_percentage = varValue;
		}
	#if FAN_COUNT > 0
		else if(VAR_IS_ADDR(SET_FAN_SPEED_ADDR)){							// fan speed
		#if FAN_COUNT > 1
			fanSpeeds[active_extruder < FAN_COUNT ? active_extruder : 0]
		#else
			fanSpeeds[0]
		#endif
			= varValue * 2.55;
		}
	#endif
		else if(VAR_IS_ADDR(SET_EXT_MULTIPLY_ADDR)){					// flow
			flow_percentage[active_extruder] = varValue;
		}else if (VAR_IS_ADDR(SET_PREHEAT_FAN_SPEED_ADDR)) {	// preheat fan speed
			lcd_preheat_fan_speed = varValue * 2.55;
		}else if (VAR_IS_ADDR(SET_PREHEAT_EXT_TEMP_ADDR_0)) {	// preheat extruder 0 temp.
			NOMORE(varValue, HEATER_0_MAXTEMP - 15);
			lcd_preheat_hotend_temp[0] = varValue;
		}
		#if EXTRUDERS > 1
		else if(VAR_IS_ADDR(SET_PREHEAT_EXT_TEMP_ADDR_1)) {		// preheat extruder 1 temp.
			NOMORE(varValue, HEATER_1_MAXTEMP - 15);
			lcd_preheat_hotend_temp[1] = varValue;
		}
		#endif
		#if EXTRUDERS > 2
		else if(VAR_IS_ADDR(SET_PREHEAT_EXT_TEMP_ADDR_2)) {		// preheat extruder 2 temp.
			NOMORE(varValue, HEATER_2_MAXTEMP - 15);
			lcd_preheat_hotend_temp[2] = varValue;
		}
		#endif
		else if(VAR_IS_ADDR(SET_PREHEAT_BED_TEMP_ADDR)){			// preheat bed temp.
			NOMORE(varValue, BED_MAXTEMP - 15);
			lcd_preheat_bed_temp = varValue;
		}
	#ifdef HOTWIND_SYSTEM
		else if (VAR_IS_ADDR(SET_PREHEAT_CHAMBER_TEMP_ADDR)){	// perheat chamber temp.
			NOMORE(varValue, CHAMBER_MAXTEMP - 5);
			lcd_preheat_chamber_temp = varValue;
		}
	#endif
	#ifdef REG_SN
	else if (VAR_IS_ADDR(SET_REG_KEY_ADDR)) {								// register key
		regSN = (float)varValue / 100;
		dwin_update_reg_info();
		if(REG_PASS){
			updateStateStrings();
			returnDefaultButtonAction();
		}
		STORE_SETTING(regSN);
	}
	#endif
	#if HAS_AUTO_FAN
		else if(VAR_IS_ADDR(SET_TEMP_FAN_SPEED_ADDR)){				// temp. fan speed
			extruder_auto_fan_speed = varValue * 2.55;
		}
	#endif
	#ifdef HAS_AIR_FAN
		else if(VAR_IS_ADDR(SET_AIR_FAN_SPEED_ADDR)){					// air fan speed
			air_fan_speed = varValue * 2.55;
		}
	#endif
	#if HAS_BED_PROBE
		else if(VAR_IS_ADDR(SET_SERVO_Z_OFFSET_ADDR)){				// servo Z offset
			zprobe_zoffset = -(float)(int16_t)varValue / 100;
			refresh_zprobe_zoffset();
			STORE_SETTING(zprobe_zoffset);
		}
	#endif
	#if	ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
		else if(VAR_IS_ADDR(SET_FADE_HEIGHT_ADDR)){						// fade height
			if(leveling_is_active()){
				float fade_height = (float)varValue / 10;
				set_z_fade_height(fade_height);
				STORE_PLAN(z_fade_height);
			}
		}
	#endif
		else if(false){
		}else{
			SERIAL_ECHO_F(dwin_getVar()->varAddr, HEX);
			SERIAL_EOL();
		}
	}else if(VAR_IS_LEN(4)){
		uint32_t varValue = ((uint32_t)dwin_getVar()->data[0] << 24) | ((uint32_t)dwin_getVar()->data[1] << 16)
				| ((uint32_t)dwin_getVar()->data[2] << 8) | ((uint32_t)dwin_getVar()->data[3]);

		if(VAR_IS_ADDR(SET_X_STEP_ADDR)){
			planner.axis_steps_per_mm[X_AXIS] = (float)varValue / 10000;						// axis X motor step
			planner.refresh_positioning();
		}else if(VAR_IS_ADDR(SET_Y_STEP_ADDR)){
			planner.axis_steps_per_mm[Y_AXIS] = (float)varValue / 10000;						// axis Y motor step
			planner.refresh_positioning();
		}else if(VAR_IS_ADDR(SET_Z_STEP_ADDR)){
			planner.axis_steps_per_mm[Z_AXIS] = (float)varValue / 1000;						// axis Z motor step
			planner.refresh_positioning();
		}else if(VAR_IS_ADDR(SET_E_STEP_ADDR)){
			planner.axis_steps_per_mm[E_AXIS] = (float)varValue / 10000;						// axis E motor step
			planner.refresh_positioning();
		}else{
			SERIAL_ECHO_F(dwin_getVar()->varAddr, HEX);
			SERIAL_EOL();
		}
	}
#if HAS_READER
	else if(VAR_IS_LEN(LCD_FILE_CHAR_LEN)){
		if(VAR_IS_ADDR(FILE_ITEM_STR_0)
		#if LCD_FILE_ITEM_PER_PAGE > 1
			|| VAR_IS_ADDR(FILE_ITEM_STR_1)
		#endif
		#if LCD_FILE_ITEM_PER_PAGE > 2
			|| VAR_IS_ADDR(FILE_ITEM_STR_2)
		#endif
		#if LCD_FILE_ITEM_PER_PAGE > 3
			|| VAR_IS_ADDR(FILE_ITEM_STR_3)
		#endif
		#if LCD_FILE_ITEM_PER_PAGE > 4
			|| VAR_IS_ADDR(FILE_ITEM_STR_4)
		#endif
		#if LCD_FILE_ITEM_PER_PAGE > 5
			|| VAR_IS_ADDR(FILE_ITEM_STR_5)
		#endif
		#if LCD_FILE_ITEM_PER_PAGE > 6
			|| VAR_IS_ADDR(FILE_ITEM_STR_6)
		#endif
		#if LCD_FILE_ITEM_PER_PAGE > 7
			|| VAR_IS_ADDR(FILE_ITEM_STR_7)
		#endif
		#if LCD_FILE_ITEM_PER_PAGE > 8
			|| VAR_IS_ADDR(FILE_ITEM_STR_8)
		#endif
		){
			char name[LCD_FILE_CHAR_LEN + NULL_CHAR_LENGTH];
			// show the string in the center of LCD.
		#ifdef FILE_UNICODE_SUPPORT
			ZERO(name);
			char* ustr = (char *)dwin_getVar()->data;
			memcpy(name, ustr, unicodeStrlen(ustr));
			#ifndef LCD_FILE_CHAR_MAXIMIZE
				centerUString(name, LCD_FILE_CHAR_LEN);
			#endif
			SET_USTR(FILE_SELECT, LCD_FILE_CHAR_LEN, name);
		#else
			strcpy(name, (char *)dwin_getVar()->data);
			centerString(name, LCD_FILE_CHAR_LEN);
			SET_STR(FILE_SELECT, LCD_FILE_CHAR_LEN, name);
		#endif
		}
	}
#endif //HAS_READER
	else if(VAR_IS_LEN((DEFAULT_STR_LEN * 2))){
		if(false){
			;
		} else{
			SERIAL_ECHOLNPGM("unknown var");
			SERIAL_ECHO_F(dwin_getVar()->varAddr, HEX);
			SERIAL_EOL();
		}
	}else{
		SERIAL_ECHOLNPGM("unknown var_len");
		SERIAL_ECHO_F(dwin_getVar()->varAddr, HEX);
		SERIAL_ECHOLNPAIR(":	", (int)dwin_getVar()->dataLen);
	}
}

// control the dwin lcd page.
static void pageControl(){
	bool touchLCD = DWIN_TOUCH;

	if(touchLCD) resetTimeout();

#ifdef ACCIDENT_DETECT
	if(isAccident && (powerState > POWER_COOLING) && !DWIN_IS_PAGE(PAGE_UNFINISH_CHOOSE)){
		GO_PAGE(PAGE_UNFINISH_CHOOSE);
	}
#endif //ACCIDENT_DETECT

#if HAS_BED_PROBE
	if(probeDone && IS_POPUP(POP_ICO_PROBING)){
		HIDE_POPUP;
		jumpSettingLevelingAction();
	}
#endif

	if(!planner.blocks_queued()){
		if(IS_FILAMENT_PAGE && (IS_POPUP(POP_ICO_EXTRUDE_FILAMENT) || IS_POPUP(POP_ICO_RECRACT_FILAMENT))){
			HIDE_POPUP;
		}else if(IS_MOVE_PAGE && IS_POPUP(POP_ICO_MOVING)){			// Moving
			HIDE_POPUP;
		}else if(IS_PRINT_PAGE && IS_POPUP(POP_ICO_PARKING)){		// Parking
			HIDE_POPUP;
		}
	}

#ifdef WIFI_SUPPORT
#ifdef WIFI_MODULE_ESP8266_01
	if(IS_WIFI_PAGE && IS_POPUP(POP_ICO_WAITING) && myWifi.isAction(ACTION_NULL)){
    updateData();
//    HIDE_POPUP;
	}
#endif
#endif

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
		}else{
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
		if(IS_DEFAULT_PAGE || IS_FILE_PAGE){
			returnDefaultButtonAction();
		}

		dwinReaderState = READER_STATE;
	}
#endif	//HAS_READER


	if(return_default_page_time && !HAS_POPUP && !IS_HOLDING_PAGE){
		unsigned long curTime = millis();

		if(ELAPSED(curTime, return_default_page_time) & (curTime > dwin_next_update_millis)){
      if(!IS_DEFAULT_PAGE) returnDefaultButtonAction();
		}
	}

}

// update the dwin lcd data.
static void updateData(){
	if(IS_DEFAULT_PAGE){
		dwin_update_temp_info();
  #ifdef WIFI_SUPPORT
		dwin_update_wifi_home_icon();
  #endif
	}

	if(DWIN_IS_PAGE(PAGE_SHUTDOWN_HOTTEMP)){
		dwin_update_max_temp_info();
	}

	if(IS_AXIS_INFO_PAGE){
		dwin_update_axis_info();
	}

	if(DWIN_IS_PAGE(PAGE_ADJUST)){
		dwin_update_adjust_info();
	}

	if(DWIN_IS_PAGE(PAGE_MOTION_SETTING)){
		dwin_update_setting_motion_info();
	}

#ifdef HAS_AIR_FAN
	if(DWIN_IS_PAGE(PAGE_FANSPEED_SETTING_FILTER))
#else
	if(DWIN_IS_PAGE(PAGE_FANSPEED_SETTING))
#endif
	{
		dwin_update_setting_fan_info();
	}

#ifdef HOTWIND_SYSTEM
	if(DWIN_IS_PAGE(PAGE_PREHEAT_SETTING_CHAMBER))
#else
	if(DWIN_IS_PAGE(PAGE_PREHEAT_SETTING))
#endif
	{
		dwin_update_setting_preheat_info();
	}

#ifdef WIFI_SUPPORT
  if(IS_WIFI_SETTING_PAGE){
    dwin_update_wifi_icon();
  }
	if(IS_WIFI_INFO_PAGE){
		dwin_update_setting_wifi_info();
	}
#endif

#if HAS_LEVELING
	if(IS_LEVELING_PAGE){
		dwin_update_setting_leveling_info();
	}
#endif

	if(IS_PRINT_PAGE){
		dwin_update_progress_info();
	}

	if(DWIN_IS_PAGE(PAGE_INFO_PRINTER)){
		dwin_update_time_info(TIME_USED);
	}
	
#ifdef REG_SN
	if(IS_REG_PAGE) {
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

	if(IS_DWIN_DATA_READY){
		if(dwin_getVar()->valid){
			resolveVar();
		}

		pageControl();

		if(millis() > dwin_next_update_millis){
			dwin_next_update_millis = millis() + 1000L;	// update the lcd every one second.
			updateData();
		}
	}
}

#endif // DWIN_LCD
