/*
dwin_lcd_implementation.h - Hardware serial library for DWIN UART LCM.
Copyright (c) 2015 CreatBot. All Right Reserved.

Developer Lyn Lee, 3 June 2015.
*/

#ifndef DWIN_LCD_IMPLEMENTATION_H
#define DWIN_LCD_IMPLEMENTATION_H

#include "Marlin.h"

#ifdef DWIN_LCD



#define LCD_SERIAL_BAUDRATE		115200
#define LCD_FH_1					0x5A
#define LCD_FH_2					0xA5
#define LCD_BUF_LEN				64		//char
#define LCD_INIT_TIMEOUT 	5000		//ms
#define LCD_TIMEOUT				2000	//ms
#define LCD_RUN_CYCLE			100		//ms

#define WRITE_REGISTER			0x80
#define READ_REGISTER				0x81
#define WRITE_VARIABLE			0x82
#define READ_VARIABLE				0x83
#define WRITE_BUFFER				0x84
#define MIN_VARIBLE_ADDR		0x0000
#define MAX_VARIBLE_ADDR		0x07FF
#define MIN_REGISTER_ADDR		0x00
#define MAX_REGISTER_ADDR		0xFF

#define REGISTER_PAGE						0x03
#define REGISTER_TOUCH_STATUS		0x05
#define REGISTER_TOUCH_ENABLE		0x0B
#define REGISTER_KEY						0x4F

#define POPUP_BUF_SIZE			8
#define POPUP_HIDDEN_KEY		0xFF

#define DEFAULT_STR_LEN			0x10

#ifndef DWIN_SERIAL_USE_BUILT_IN
struct dwin_ring_buffer {
	uint8_t buffer[LCD_BUF_LEN];
	uint8_t head;
	uint8_t tail;
};
#endif

struct pop_ring_buffer {
	uint8_t	key[POPUP_BUF_SIZE];
	uint8_t head;
	uint8_t tail;
};

struct dwinVarStruct{
	boolean valid;
	uint16_t varAddr;
	uint8_t dataLen;
	uint8_t data[LCD_BUF_LEN - 7];	// [5A A5 recvLen 83 addr_H addr_L dataLen data]
};

struct dwinStateStruct{
	uint8_t dwin_state;
	uint32_t timeoutTime[3];
};

void dwin_init();
void dwin_loop();

boolean dwin_isExist();

dwinVarStruct* dwin_getVar();

void getPage();
void setPage(const char* index);
void setPage(uint16_t index);
uint16_t currentPage();
boolean isPage(const char* index);
boolean isPage(uint16_t index);
void enableTouch();
void disableTouch();
void getTouch();
uint8_t touchState();
void currentKEY();
void triggerKEY(const char* key);
void triggerKEY(uint8_t key);
uint8_t getPop();
boolean setPop(const char* key);
boolean setPop(uint8_t key);

void getValueForInt(const char* addr);
void getValueForInt(uint16_t addr);
void getValueForLong(const char* addr);
void getValueForLong(uint16_t addr);
void getValueForString(const char* addr, unsigned char dataLen = DEFAULT_STR_LEN);
void getValueForString(uint16_t addr, unsigned char dataLen = DEFAULT_STR_LEN);
void getValueForAttr(const char* addr, uint8_t offset, uint8_t dataLen);
void getValueForAttr(uint16_t addr, uint8_t offset, uint8_t dataLen);

void setValueAsInt(const char* addr, uint16_t value);
void setValueAsInt(const char* addr, const char* data);
void setValueAsInt(uint16_t addr, uint16_t value);
void setValueAsInt(uint16_t addr, const char* data);
void setValueAsLong(const char* addr, uint32_t value);
void setValueAsLong(const char* addr, const char* data);
void setValueAsLong(uint16_t addr, uint32_t value);
void setValueAsLong(uint16_t addr, const char* data);
void setValueAsString(const char* addr, const uint16_t space, const char* data);
void setValueAsString(uint16_t addr, const uint16_t space, const char* data);
void setValueAsAttr(const char* addr, uint8_t offset, char* data);
void setValueAsAttr(uint16_t addr, uint8_t offset, char* data);

void drawRectangle(const char* addr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* color);
void fillRectangle(const char* addr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* color);

uint32_t convToLCD(const float x, uint8_t int_num, uint8_t dec_num);
uint32_t convToLCD(const long x, uint8_t int_num, uint8_t dec_num);
char* charToHex(char* hex, unsigned char* data, uint16_t dataLen);
char* uint8_tToHex(char* hex, uint8_t data);
char* uint16_tToHex(char* hex, uint16_t data);
boolean uint16_tCompHex(const char *hex, uint16_t data);


// Below, the args is from SROM
void setPage_P(const char* index);
bool isPage_P(const char* index);
bool setPop_P(const char* key);

void getValueForInt_P(const char* addr);
void getValueForLong_P(const char* addr);
void getValueForString_P(const char* addr);
void getValueForAttr_P(const char* addr, uint8_t offset, uint8_t len);

void setValueAsInt_P(const char* addr, uint16_t value);
void setValueAsInt_P(const char* addr, const char* data);
void setValueAsLong_P(const char* addr, uint32_t value);
void setValueAsLong_P(const char* addr, const char* data);
void setValueAsString_P(const char* addr, const uint16_t space, const char* data);
void setValueAsAttr_P(const char* addr, uint8_t offset, char* data);

void drawRectangle_P(const char* addr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* color);
void fillRectangle_P(const char* addr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* color);

boolean uint16_tCompHex_P(const char *hex, uint16_t data);

#endif //DWIN_LCD

#endif //DWIN_LCD_H
