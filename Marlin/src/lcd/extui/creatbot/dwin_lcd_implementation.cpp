/*
  dwin_lcd_implementation.cpp - Hardware serial library for DWIN UART LCM.
  Copyright (c) 2015 CreatBot. All Right Reserved.

  Developer Lyn Lee, 3 June 2015.
*/

#include "dwin_lcd_implementation.h"

#ifdef CREATBOT_LCD

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <wiring.c>
#include "../../../core/serial.h"

enum recv_state {PreRevc, GetFH1, GetFH2, GetLen, RevcDone};
static bool dwinExist;
static uint8_t recvState;
static uint8_t recvLen;
static uint8_t recvCount;

static uint8_t cmdBuffer[LCD_BUF_LEN];
static uint8_t recBuffer[LCD_BUF_LEN];
static uint16_t cmdLen;

static uint16_t curPage;
static uint8_t isTouch;

static uint32_t nextUpdateTime;
static uint32_t nextInitTime;

#ifndef DWIN_SERIAL_USE_BUILT_IN
static dwin_ring_buffer dwinBuffer = { { 0 }, 0, 0 };
#endif

static pop_ring_buffer popKey = { { 0 }, 0, 0 };
static dwinVarStruct dwinVar = { 0, 0, 0, { 0 } };
static dwinStateStruct dwinState = { 0, { 0, 0, 0 } };

#define RECV_DONE				(recvState == RevcDone)
#define HAS_POP_TASK		(pop_room() != (POPUP_BUF_SIZE - 2))

#define BIT_GET_PAGE		0
#define BIT_GET_TOUCH		1
#define BIT_GET_KEY			2

#define IS_IDLE					((dwinState.dwin_state | 0) == 0)
#define IS_GET_PAGE			((dwinState.dwin_state & _BV(BIT_GET_PAGE)) == _BV(BIT_GET_PAGE))
#define IS_GET_TOUCH		((dwinState.dwin_state & _BV(BIT_GET_TOUCH)) == _BV(BIT_GET_TOUCH))
#define IS_GET_KEY			((dwinState.dwin_state & _BV(BIT_GET_KEY)) == _BV(BIT_GET_KEY))

#define SET_DWIN_STATE(x)	do{ dwinState.dwin_state |= (_BV(x)); dwinState.timeoutTime[x] = millis() + LCD_TIMEOUT; } while(0)
#define CLS_DWIN_STATE(x)	do{ dwinState.dwin_state &= ~(_BV(x)); dwinState.timeoutTime[x] = 0; } while(0)

#ifndef DWIN_SERIAL_USE_BUILT_IN
static uint8_t available(void) {
	// CRITICAL_SECTION_START;
  const uint8_t h = dwinBuffer.head,
  							t = dwinBuffer.tail;
//   CRITICAL_SECTION_END;
	return (uint8_t)(LCD_BUF_LEN + h - t) % LCD_BUF_LEN;
}

static void write(uint8_t c) {
	loop_until_bit_is_set(UCSR3A, UDRE3);
	UDR3 = c;
}

static void write(const uint8_t *buffer, size_t size) {
	while(size--) write(*buffer++);
}

__attribute__((unused)) static uint8_t peek() {
//   CRITICAL_SECTION_START;
	const uint8_t v = dwinBuffer.head == dwinBuffer.tail ? 0xFF : dwinBuffer.buffer[dwinBuffer.tail];
//   CRITICAL_SECTION_END;
  return v;
}

static uint8_t read() {
//   CRITICAL_SECTION_START;
	const uint8_t v = dwinBuffer.head == dwinBuffer.tail ? 0xFF : dwinBuffer.buffer[dwinBuffer.tail];
	dwinBuffer.tail = (uint8_t)(dwinBuffer.tail + 1) % LCD_BUF_LEN;
//   CRITICAL_SECTION_END;
  return v;
}

static void flush() {
	// CRITICAL_SECTION_START;
	dwinBuffer.head = dwinBuffer.tail;
	// CRITICAL_SECTION_END;
}

static void begin(long baud) {
	uint16_t baud_setting;

	baud_setting = (F_CPU / 4 / baud - 1) / 2;

	UCSR3A = _BV(U2X3);

	UBRR3H = baud_setting >> 8;
	UBRR3L = baud_setting;

	SBI(UCSR3B, RXEN3);
	SBI(UCSR3B, TXEN3);
	SBI(UCSR3B, RXCIE3);

	flush();
}

static void end() {
	flush();

	CBI(UCSR3B, RXEN3);
	CBI(UCSR3B, TXEN3);
	CBI(UCSR3B, RXCIE3);
}

ISR(USART3_RX_vect) {
	uint8_t c = UDR3;
	// CRITICAL_SECTION_START;
	uint8_t i = (uint8_t)(dwinBuffer.head + 1) % LCD_BUF_LEN;
	if(i != dwinBuffer.tail) {
		dwinBuffer.buffer[dwinBuffer.head] = c;
		dwinBuffer.head = i;
	}
	// CRITICAL_SECTION_END;
}

	#define DWIN_BEGIN			begin
	#define DWIN_READ				read
	#define DWIN_WRITE			write
	#define DWIN_AVAILABLE	available
	#define DWIN_FLUSH			flush
	#define DWIN_END				end
#else
	#define DWIN_SERIAL			Serial3
	#define DWIN_BEGIN			DWIN_SERIAL.begin
	#define DWIN_READ				DWIN_SERIAL.read
	#define DWIN_WRITE			DWIN_SERIAL.write
	#define DWIN_AVAILABLE	DWIN_SERIAL.available
	#define DWIN_FLUSH			DWIN_SERIAL.flush
	#define DWIN_END				DWIN_SERIAL.end
#endif



static void pop_init() {
	popKey.head = 1;
	popKey.tail = 0;
	popKey.key[0] = POPUP_HIDDEN_KEY;
}

static uint8_t pop_last() {
	uint8_t i = (POPUP_BUF_SIZE + popKey.head - 1) % POPUP_BUF_SIZE;
	return popKey.key[i];
}

static uint8_t pop_next() {
	uint8_t i = (popKey.tail + 1) % POPUP_BUF_SIZE;
	return popKey.key[i];
}

static void pop_insert(uint8_t key) {
	uint8_t i = (uint8_t)(popKey.head + 1) % POPUP_BUF_SIZE;
	if(i != popKey.tail) {
		popKey.key[popKey.head] = key;
		popKey.head = i;
	}
}

static uint8_t pop_remove() {
	uint8_t i = (uint8_t)(popKey.tail + 1) % POPUP_BUF_SIZE;
	if(i == popKey.head) {
		return 0x00;
	} else {
		uint8_t key = pop_next();
		popKey.tail = i;
		return key;
	}
}

static uint8_t pop_room() {
	return ((POPUP_BUF_SIZE + popKey.tail - popKey.head) % POPUP_BUF_SIZE) - 1;
}



/** clear the receive buffer. */
static void clearReceive() { memset(recBuffer, 0, sizeof(recBuffer)); }

/** read the data from lcd's serial. */
static void readData() {
	if(RECV_DONE) {	// the next receive.
		recvState = PreRevc;
		dwinVar.valid = false;	// set the dwinVar invalid.
	}
	while(DWIN_AVAILABLE() > 0) {
		uint8_t c = DWIN_READ();
		if(recvState == PreRevc) {
			if(recvLen)	recvLen = 0;				// receive length set 0.
			if(recvCount)	recvCount = 0;				// receive count set 0.
			if(recBuffer[0]) clearReceive();			// clear the recvBuffer.
			if(c == LCD_FH_1)	recvState = GetFH1;
		}
		else if(recvState == GetFH1) {
			if(c == LCD_FH_2)
				recvState = GetFH2;
			else
				recvState = PreRevc;					//if the value is't FH_2 , it's a dirty data. So we should throw them.
		}
		else if(recvState == GetFH2) {
			if(c < 2 || c > LCD_BUF_LEN)
				recvState = PreRevc;					//the value is the length of data. it should be a right range.
			else {
				recvState = GetLen;
				recvLen = c;
			}
		}
		else if(recvState == GetLen) {
			recBuffer[recvCount++] = c;
			if(recvCount == recvLen) {
				recvState = RevcDone;					//the transmission is complete.
				break;
			}
		}
	}

#ifdef DWIN_LCD_DEDUG
	if(RECV_DONE) {

		SERIAL_ECHOPGM("-->	");
		SERIAL_ECHO((int)recvLen);
		SERIAL_ECHOPGM("	");

		for(uint8_t i = 0; i < recvCount; i++) {
			SERIAL_ECHO_F(recBuffer[i], HEX);
			SERIAL_ECHOPGM("	");
		}
		SERIAL_EOL();
	}
#endif
}

/**
 * copy the data to cmdBuffer, the data is a HEX String.
 * return the length of the converted data.
 * return -1 if the format of data is not a HEX String.
 */
static uint16_t writeData(uint8_t* pos, const char* data) {
  uint16_t len = 0;
  unsigned char temp = 0;
  bool flag = false;

  while(*data) {
    if(*data >= 'A' && *data <= 'F')
      temp |= *data - 55;
    else if(*data >= 'a' && *data <= 'f')
      temp |= *data - 87;
    else if(*data >= '0' && *data <= '9')
      temp |= *data - 48;
    else
      return 0xFFFF;
    if(flag) {
      *pos++ = temp;
      len++;
      temp <<= 8;
    } else {
      temp <<= 4;
    }
    flag = !flag;
    data++;
  }
  return len;
}

/** the head of command is begin of "5AA5" */
static void sendStart() {
  cmdBuffer[0] = LCD_FH_1;
  cmdBuffer[1] = LCD_FH_2;
  cmdLen = 2;
}

/** calculate the length of data and send command to LCD_SERIAL, then reset the cmdLen to 0 for next command. */
static void sendEnd() {
  cmdBuffer[2] = (cmdLen > 2) ? (cmdLen - 3) : 0;

  if(cmdBuffer[2]){

#ifdef DWIN_LCD_DEDUG
  SERIAL_ECHOPGM("<--	");
	for(uint8_t i = 2; i<cmdLen; i++){
		SERIAL_ECHO_F(cmdBuffer[i], HEX);
		SERIAL_ECHOPGM("	");
	}
	SERIAL_EOL();
#endif

	DWIN_WRITE((uint8_t *)cmdBuffer, cmdLen);
  }
  cmdLen = 0;
}

/**
 * register addr is from 0x00 to 0xFF
 * addr is a constant that defined in other part of program.
 * data is a constant HEX string.
 */
static void writeRegister(const unsigned char addr, const char* data) {
  if(cmdLen != 2) return;

  uint16_t len = 0;
  cmdBuffer[3] = WRITE_REGISTER;
  cmdBuffer[4] = addr;
  len = writeData(&cmdBuffer[5], data);
  cmdLen += (len != 0xFFFF) ? (3 + len) : 0;
}

/**
 * register addr is from 0x00 to MAX_REGISTER_ADDR
 * addr is a constant that defined in other part of program.
 * data is a array pointer of unsigned char.
 * dataLen is the byte length write to register
 */
static void writeRegister(const unsigned char addr, unsigned char* data, unsigned char dataLen){
	if(cmdLen != 2) return;

	cmdBuffer[3] = WRITE_REGISTER;
	cmdBuffer[4] = addr;
	for(uint16_t i = 0; i < dataLen; i++)	cmdBuffer[5 + i] = *data++;
	cmdLen += (dataLen > 0) ? (3 + dataLen) : 0;
}

/**
 * register addr is from 0x00 to MAX_REGISTER_ADDR
 * addr is a constant that defined in other part of program.
 * dataLen is the byte length read from register, the max of dataLen is 0xFF
 */
static void readRegister(const unsigned char addr, unsigned char dataLen) {
  if(cmdLen != 2) return;

  cmdBuffer[3] = READ_REGISTER;
  cmdBuffer[4] = addr;
  cmdBuffer[5] = dataLen;
  cmdLen += 4;
}

/**
 * variable addr is from 0x0000 to MAX_VARIBLE_ADDR
 * addr is a constant HEX string (2 bytes, 4 HEX, 16 bits)
 * data is a constant HEX string
 */
static void writeVariable(const char* addr, const char* data) {
	if(cmdLen != 2) return;

	uint16_t len = 0;
	cmdBuffer[3] = WRITE_VARIABLE;
	len = writeData(&cmdBuffer[4], addr);
	if(len != 2) return; // if addr is not a 2 bytes HEX string

	len = writeData(&cmdBuffer[6], data);
	cmdLen += (len != 0xFFFF) ? (4 + len) : 0;
}

/**
 * variable addr is from 0x0000 to MAX_VARIBLE_ADDR
 * addr is a uint16_t (2 bytes)
 * data is a constant HEX string.
 */
static void writeVariable(uint16_t addr, const char* data){
	if(cmdLen != 2) return;

	uint16_t len = 0;
	cmdBuffer[3] = WRITE_VARIABLE;
	cmdBuffer[4] = addr >> 8;
	cmdBuffer[5] = addr & 0x00FF;
	len = writeData(&cmdBuffer[6], data);
	cmdLen += (len != 0xFFFF) ? (4 + len) : 0;
}

/**
 * variable addr is from 0x0000 to MAX_VARIBLE_ADDR
 * addr is a constant HEX string (2 bytes = 4 HEX = 16 bits)
 * data is a array pointer of unsigned char
 * dataLen is the byte length write to register
 */
static void writeVariable(const char* addr, unsigned char* data, uint16_t dataLen){
	if(cmdLen != 2) return;

	uint16_t len = 0;
	cmdBuffer[3] = WRITE_VARIABLE;
	len = writeData(&cmdBuffer[4], addr);
	if(len != 2) return; // if addr is not a 2 bytes HEX string

	for(uint16_t i = 0; i < dataLen; i++)	cmdBuffer[6 + i] = *data++;
	cmdLen += (dataLen > 0) ? (4 + dataLen) : 0;
}

/**
 * variable addr is from 0x0000 to MAX_VARIBLE_ADDR
 * addr is a uint16_t (2 bytes)
 * data is a array pointer of unsigned char
 * dataLen is the byte length write to register
 */
static void writeVariable(uint16_t addr, unsigned char* data, uint16_t dataLen){
	if(cmdLen != 2) return;

	cmdBuffer[3] = WRITE_VARIABLE;
	cmdBuffer[4] = addr >> 8;
	cmdBuffer[5] = addr & 0x00FF;

	for(uint16_t i = 0; i < dataLen; i++)	cmdBuffer[6 + i] = *data++;
	cmdLen += (dataLen > 0) ? (4 + dataLen) : 0;
}

/**
 * variable addr is from 0x0000 to MAX_VARIBLE_ADDR
 * addr is a constant HEX string (2 bytes = 4 HEX = 16 bits)
 * dataLen is the word length read form variable_addr, the max of dataLen is 0x7F
 */
static void readVariable(const char* addr, unsigned char dataLen) {
	if(cmdLen != 2) return;

	uint16_t len = 0;
	cmdBuffer[3] = READ_VARIABLE;
	len = writeData(&cmdBuffer[4], addr);
	if(len != 2) return; // if addr is not a 2 bytes HEX string

	cmdBuffer[6] = dataLen;
	cmdLen += 5;
}

/**
 * variable addr is from 0x0000 to MAX_VARIBLE_ADDR
 * addr is a uint16_t (2 bytes)
 * dataLen is the word length read form variable_addr, the max of dataLen is 0x7F
 */
static void readVariable(uint16_t addr, unsigned char dataLen){
	if(cmdLen != 2) return;

	cmdBuffer[3] = READ_VARIABLE;
	cmdBuffer[4] = addr >> 8;
	cmdBuffer[5] = addr & 0x00FF;
	cmdBuffer[6] = dataLen;
	cmdLen += 5;
}

// TODO write a buffer line
__attribute__((unused)) static void writeBuffer() {
}


/** initialization DWIN LCD  &  clear the serial data if exist */
void dwin_init() {
	DWIN_BEGIN(LCD_SERIAL_BAUDRATE);
	while(DWIN_AVAILABLE() > 0) { DWIN_FLUSH(); }

	dwinExist = false;
	recvState = PreRevc;
	recvLen = recvCount = 0;

	curPage = -1;				// mean is unknown
	isTouch = -1;				// mean is unknown
	nextUpdateTime = 0;

	pop_init();
	memset(&dwinState, 0, sizeof(dwinState));
	memset(&dwinVar, 0, sizeof(dwinVar));

	nextInitTime = millis() + LCD_INIT_TIMEOUT;
}



void dwin_loop() {

	//Get the page
	if(!IS_GET_PAGE && (nextUpdateTime < millis())) {
		nextUpdateTime = millis() + LCD_RUN_CYCLE;
		getPage();
	}

	//Get the touch status
	if(!IS_GET_TOUCH) {
		getTouch();
		isTouch = -1;
	}

	//Get the current key value register status
	if(!IS_GET_KEY && HAS_POP_TASK) {
		currentKEY();
	}

	//Receive data
	readData();

	//analyze data
	if(RECV_DONE) {
		if(recBuffer[0] == READ_REGISTER) {
			if((recvLen == 5) && (recBuffer[1] == REGISTER_PAGE) && IS_GET_PAGE) {
				CLS_DWIN_STATE(BIT_GET_PAGE);
				curPage = ((uint16_t)recBuffer[3] << 8) | ((uint16_t)recBuffer[4]);
			}
			if((recvLen == 4) && (recBuffer[1] == REGISTER_TOUCH_STATUS) && IS_GET_TOUCH) {
				CLS_DWIN_STATE(BIT_GET_TOUCH);
				if(recBuffer[3] == 0x5A) {
					isTouch = 1;
					sendStart();
					writeRegister(REGISTER_TOUCH_STATUS, "00");
					sendEnd();
				}else {
					isTouch = 0;
				}
			}
			if((recvLen == 4) && (recBuffer[1] == REGISTER_KEY) && IS_GET_KEY) {
				CLS_DWIN_STATE(BIT_GET_KEY);
				uint8_t key = recBuffer[3];
				if(key) {
					currentKEY();
				} else {
					if(HAS_POP_TASK) {
					#ifdef DWIN_HEX_OPERATE_USE_STR
						char tempKey[3];
						uint8_tToHex(tempKey, pop_remove());
						triggerKEY(tempKey);
					#else
						triggerKEY(pop_remove());
					#endif
					}
				}
			}
		} else if (recBuffer[0] == READ_VARIABLE){
			dwinVar.valid = true;
			dwinVar.varAddr = ((uint16_t)recBuffer[1] << 8) | ((uint16_t)recBuffer[2]);
			dwinVar.dataLen = recBuffer[3] * 2;
			memcpy(dwinVar.data, &recBuffer[4], dwinVar.dataLen);
		}

		if(!dwinExist) dwinExist = true;
	}

	//Overtime
	bool dwinTimeout = false;
	for(uint8_t i = 0; i < 3; i++){
		if(dwinState.timeoutTime[i] && millis() > dwinState.timeoutTime[i]){
			dwinTimeout = true;
			break;
		}
	}

	if(dwinTimeout) {
		CLS_DWIN_STATE(BIT_GET_PAGE);
		CLS_DWIN_STATE(BIT_GET_TOUCH);
		CLS_DWIN_STATE(BIT_GET_KEY);
		curPage = -1;
		isTouch = -1;
		dwinExist = false;
	}

	//Serial reset
	if(!dwinExist && (nextInitTime < millis())){
		DWIN_END();
		dwin_init();
	}
}

bool dwin_isExist(){
	return dwinExist;
}

dwinVarStruct* dwin_getVar(){
	return &dwinVar;
}

/** get the current page addr. */
void getPage() {
	sendStart();
	readRegister(REGISTER_PAGE, 0x02);
	sendEnd();
	SET_DWIN_STATE(BIT_GET_PAGE);
}

/**
 * jump to the specified page
 * index is a constant HEX string , the addr of specified page.
 */
void setPage(const char* index) {
  sendStart();
  writeRegister(REGISTER_PAGE, index);
  sendEnd();
}

/**
 * jump to the specified page
 * index is a uint16_t (2 bytes), the addr of specified page.
 */
void setPage(uint16_t index){
	unsigned char temp[2];
	temp[0] = index >> 8;
	temp[1] = index & 0x00FF;
	sendStart();
	writeRegister(REGISTER_PAGE, temp, 2);
	sendEnd();
}

/** return the current page. */
uint16_t currentPage(){
	return curPage;
}

/** if current page is specified page. */
bool isPage(const char* index){
#ifdef DWIN_HEX_OPERATE_USE_STR
	char page[5];
	return (strcmp(uint16_tToHex(page,curPage), index) == 0) ? true : false;
#else
	return ((uint16_t)strtoul(index, nullptr, HEX) == curPage);
#endif
}

/** if current page is specified page. */
bool isPage(uint16_t index){
	return (curPage == index);
}

/** enable dwin_lcd touch. */
void enableTouch(){
	sendStart();
	writeRegister(REGISTER_TOUCH_ENABLE, "FF");
	sendEnd();
}

/** disable dwin_lcd touch. */
void disableTouch(){
	sendStart();
	writeRegister(REGISTER_TOUCH_ENABLE, "00");
	sendEnd();
}

/** get the touch state. */
void getTouch(){
	sendStart();
	readRegister(REGISTER_TOUCH_STATUS, 0x01);
	sendEnd();
	SET_DWIN_STATE(BIT_GET_TOUCH);
}

/**
 * if dwin_lcd is touch.
 * return 1 is touch.
 * return 0 is not touch.
 * return -1 is unknown.
 */
uint8_t touchState(){
	return isTouch;
}

/** if key register is busy. */
void currentKEY(){
	sendStart();
	readRegister(REGISTER_KEY, 0x01);
	sendEnd();
	SET_DWIN_STATE(BIT_GET_KEY);
}

/** trigger a key defined in LCD. */
void triggerKEY(const char* key){
	sendStart();
	writeRegister(REGISTER_KEY, key);
	sendEnd();
	currentKEY();
}

/** trigger a key defined in LCD. */
void triggerKEY(uint8_t key){
	sendStart();
	writeRegister(REGISTER_KEY, &key, 1);
	sendEnd();
	currentKEY();
}

/** 
 * the key in dwin_lcd
 * return -1 is not a window
 * return 0 is unknown
 * return other is a window
 */
uint8_t getPop() {
	return pop_last();
}

/**
 * pop a windows
 * return true if success.
 * return false if error.
 */
bool setPop(const char* key){
	uint8_t tempKey = (uint8_t)strtol(key, nullptr, HEX);
#ifdef DWIN_HEX_OPERATE_USE_STR
	uint8_t lastKey = pop_last();
	if(tempKey == lastKey) {
		return true;
	} else {
		if(lastKey == POPUP_HIDDEN_KEY) {
			if(pop_room() > 0) {
				pop_insert(tempKey);
				return true;
			}
		}else{
			if(pop_room() > 1) {
				pop_insert(POPUP_HIDDEN_KEY);
				pop_insert(tempKey);
				return true;
			}
		}
		return false;
	}
#else
	return setPop(tempKey);
#endif
}

/**
* pop a windows
* return true if success.
* return false if error.
*/
bool setPop(uint8_t key){
#ifdef DWIN_HEX_OPERATE_USE_STR
	char tempKey[3];
	return setPop(uint8_tToHex(tempKey, key));
#else
	uint8_t lastKey = pop_last();
	if(key == lastKey) {
		return true;
	} else {
		if(lastKey == POPUP_HIDDEN_KEY) {
			if(pop_room() > 0) {
				pop_insert(key);
				return true;
			}
		}else{
			if(pop_room() > 1) {
				pop_insert(POPUP_HIDDEN_KEY);
				pop_insert(key);
				return true;
			}
		}
		return false;
	}
#endif
}






/**
 * check the validity of variable addr
 * addr is validity from MIN_VARIBLE_ADDR to MAX_VARIBLE_ADDR
 */
static inline bool checkValid(const char* addr){
#if (MIN_VARIBLE_ADDR == 0)
	if((uint32_t)strtoul(addr, nullptr, HEX) <= MAX_VARIBLE_ADDR)
#else
	if(WITHIN((uint32_t)strtoul(addr, nullptr, HEX), MIN_VARIBLE_ADDR, MAX_VARIBLE_ADDR))
#endif
	{
		sendStart();
		return true;
	} else{
		SERIAL_ECHOPGM("invalid variable address:	");
		SERIAL_ECHOLN(addr);
		return false;
	}
}

/**
 * check the validity of variable addr
 * addr is validity from MIN_VARIBLE_ADDR to MAX_VARIBLE_ADDR
 */
static inline bool checkValid(uint32_t addr){
#if (MIN_VARIBLE_ADDR == 0)
	if(addr <= MAX_VARIBLE_ADDR)
#else
	if(WITHIN(addr, MIN_VARIBLE_ADDR, MAX_VARIBLE_ADDR))
#endif
	{
		sendStart();
		return true;
	} else{
		SERIAL_ECHOPGM("invalid variable address:	");
		SERIAL_ECHOLN(addr);
		return false;
	}
}

/**
 * get a value from lcd's variable.
 * addr is a constant HEX string, the addr of the variable from LCD.
 * return a uint16_t, return -1 if any error occur.
 */
void getValueForInt(const char* addr){
	if(checkValid(addr)){
		readVariable(addr, 0x01);
		sendEnd();
	}
}

/**
 * get a value from lcd's variable.
 * addr is a uint16_t (2 bytes), the addr of the variable from LCD.
 * return a uint16_t, return -1 if any error occur.
 */
void getValueForInt(uint16_t addr){
	if(checkValid(addr)){
		readVariable(addr, 0x01);
		sendEnd();
	}
}

/**
 * get a value from lcd's variable.
 * addr is a constant HEX string, the addr of the variable from LCD.
 * return a uint32_t, return -1 if any error occur.
 */
void getValueForLong(const char* addr){
	if(checkValid(addr)){
		readVariable(addr, 0x02);
		sendEnd();
	}
}

/**
 * get a value from lcd's variable.
 * addr is a uint16_t (2 bytes), the addr of the variable from LCD.
 * return a uint32_t, return -1 if any error occur.
 */
void getValueForLong(uint16_t addr){
	if(checkValid(addr)){
		readVariable(addr, 0x02);
		sendEnd();
	}
}

/**
 * get a value from lcd's variable.
 * addr is a constant HEX string, the addr of the variable from LCD.
 * dataLen is the word length read from variable_addr, the max of dataLen is 0x7F.
 * return a char pointer (string).
 */
void getValueForString(const char* addr, unsigned char dataLen){
	if(checkValid(addr)){
		readVariable(addr, dataLen);
		sendEnd();
	}
}

/**
 * get a value from lcd's variable.
 * addr is a uint16_t (2 bytes), the addr of the variable from LCD.
 * dataLen is the word length read from variable_addr, the max of dataLen is 0x7F.
 * return a char pointer (string).
 */
void getValueForString(uint16_t addr, unsigned char dataLen){
	if(checkValid(addr)){
		readVariable(addr, dataLen);
		sendEnd();
	}
}

/**
 * get a value from lcd's variable.
 * temp is a pointer (any type), the data will return.
 * addr is a HEX string, the addr of the variable's attribute from LCD.
 * offset is the position that relative to the addr. That is the position of attribute.
 * dataLen is the length of data that get from LCD.
 */
void getValueForAttr(const char* addr, uint8_t offset, uint8_t dataLen){
	uint32_t addr_Value = (uint32_t)strtoul(addr, nullptr, HEX) + offset;
	if(checkValid(addr_Value)){
		readVariable((uint16_t)addr_Value, dataLen);
		sendEnd();
	}
}

/**
 * get a value from lcd's variable.
 * temp is a pointer (any type), the data will return.
 * addr is a uint16_t (2 bytes), the addr of the variable's attribute from LCD.
 * offset is the position that relative to the addr. That is the position of attribute.
 * dataLen is the length of data that get from LCD.
 */
void getValueForAttr(uint16_t addr, uint8_t offset, uint8_t dataLen){
	if(checkValid((uint32_t)addr + offset)){
		addr += offset;
		readVariable(addr, dataLen);
		sendEnd();
	}
}

/**
 * set a int value to lcd's variable
 * addr is a constant HEX string, the addr of the variable from LCD.
 * value is a uint16_t
 */
void setValueAsInt(const char* addr, uint16_t value){
	if(checkValid(addr)){
		unsigned char temp[2];
		temp[0] = value >> 8;
		temp[1] = value & 0x00FF;
		writeVariable(addr, temp, 2);
		sendEnd();
	}
}

/**
 * set a int value to lcd's variable
 * addr is a uint16_t (2 bytes), the addr of the variable from LCD.
 * value is a uint16_t
 */
void setValueAsInt(uint16_t addr, uint16_t value){
	if(checkValid(addr)){
		unsigned char temp[2];
		temp[0] = value >> 8;
		temp[1] = value & 0x00FF;
		writeVariable(addr, temp, 2);
		sendEnd();
	}
}

/**
 * set a int value to lcd's variable
 * addr is a constant HEX string, the addr of the variable from LCD.
 * data is a constant HEX string, if data is not 4 char, will send nothing.
 */
void setValueAsInt(const char* addr, const char* data){
	if(strlen(data) != 4) return;
	if(checkValid(addr)){
		writeVariable(addr, data);
		sendEnd();
	}
}

/**
 * set a int value to lcd's variable
 * addr is a uint16_t (2 bytes), the addr of the variable from LCD.
 * data is a constant HEX string, if data is not 4 char, will send nothing.
 */
void setValueAsInt(uint16_t addr, const char* data){
	if(strlen(data) != 4) return;
	if(checkValid(addr)){
		writeVariable(addr, data);
		sendEnd();
	}
}

/**
 * set a long value to lcd's variable
 * addr is a constant HEX string, the addr of the variable from LCD.
 * value is a uint32_t
 */
void setValueAsLong(const char* addr, uint32_t value){
	if(checkValid(addr)){
		unsigned char temp[4];
		temp[0] = value >> 24;
		temp[1] = (value & 0x00FF0000) >> 16;
		temp[2] = (value & 0x0000FF00) >> 8;
		temp[3] = (value & 0x000000FF);
		writeVariable(addr, temp, 4);
		sendEnd();
	}
}

/**
 * set a long value to lcd's variable
 * addr is a constant HEX string, the addr of the variable from LCD.
 * data is a constant HEX string, if data is not 8 char, will send nothing.
 */
void setValueAsLong(const char* addr, const char* data){
	if(strlen(data) != 8) return;
	if(checkValid(addr)){
		writeVariable(addr, data);
		sendEnd();
	}
}

/**
 * set a int value to lcd's variable
 * addr is a uint16_t (2 bytes), the addr of the variable from LCD.
 * value is a uint32_t
 */
void setValueAsLong(uint16_t addr, uint32_t value){
	if(checkValid(addr)){
		unsigned char temp[4];
		temp[0] = value >> 24;
		temp[1] = (value & 0x00FF0000) >> 16;
		temp[2] = (value & 0x0000FF00) >> 8;
		temp[3] = (value & 0x000000FF);
		writeVariable(addr, temp, 4);
		sendEnd();
	}
}

/**
 * set a int value to lcd's variable
 * addr is a uint16_t (2 bytes), the addr of the variable from LCD.
 * data is a constant HEX string, if data is not 8 char, will send nothing.
 */
void setValueAsLong(uint16_t addr, const char* data){
	if(strlen(data) != 8) return;
	if(checkValid(addr)){
		writeVariable(addr, data);
		sendEnd();
	}
}

/**
 * set a string value to lcd's variable.
 * addr is a constant HEX string, the addr of the variable from LCD.
 * space is the the space of addr.
 * data is the char pointer (sting) that you want to set.
 */
void setValueAsString(const char* addr, const uint16_t space, const char* data){
	if(checkValid(addr)){
		size_t len = strlen(data);
		unsigned char temp[LCD_BUF_LEN];
		memset(temp, 0, sizeof(temp));
		sendStart();
		writeVariable(addr, temp, space);
		sendEnd();

		memcpy(temp, data, len);
		len = (len % 2) ? len + 1 : len;		// store data per word, so if the length is odd, it should add a 0x00 behind the data.
		len = (len <= space) ? len : space;		// cuz the addr is a constant from LCD. so the space of addr is fixed, then the length of data should not large than space.

		sendStart();
		writeVariable(addr, temp, len);
		sendEnd();
	}
}

/**
 * set a string value to lcd's variable.
 * addr is a uint16_t (2 bytes), the addr of the variable from LCD.
 * space is the the space of addr.
 * data is the char pointer (sting) that you want to set.
 */
void setValueAsString(uint16_t addr, const uint16_t space, const char* data){
	if(checkValid(addr)){
		size_t len = strlen(data);
		unsigned char temp[LCD_BUF_LEN];
		memset(temp, 0, sizeof(temp));
		sendStart();
		writeVariable(addr, temp, space);
		sendEnd();

		memcpy(temp, data, len);
		len = (len % 2) ? len + 1 : len;
		len = (len <= space) ? len : space;

		sendStart();
		writeVariable(addr, temp, len);
		sendEnd();
	}
}

/**
 * set a value to lcd's variable's attribute.
 * addr is a HEX string, the addr of the variable's attribute from LCD.
 * offset is the position that relative to the addr. That is the position of attribute.
 * data is a constant HEX string. The length depend on the attribute, so user must know the lcd's describe addr
 */
void setValueAsAttr(const char* addr, uint8_t offset, char* data){
	uint32_t addr_Value = (uint32_t)strtoul(addr, nullptr, HEX) + offset;
	if(checkValid(addr_Value)){
		writeVariable((uint16_t)addr_Value, data);
		sendEnd();
	}
}

/**
 * set a value to lcd's variable's attribute.
 * addr is a uint16_t (2 bytes), the addr of the variable's attribute from LCD.
 * offset is the position that relative to the addr. That is the position of attribute.
 * data is a constant HEX string. The length depend on the attribute, so user must know the lcd's describe addr
 */
void setValueAsAttr(uint16_t addr, uint8_t offset, char* data){
	if(checkValid((uint32_t)addr + offset)){
		addr += offset;
		writeVariable(addr, data);
		sendEnd();
	}
}

/**
 * draw a rectangle in LCD.
 * addr is a constant HEX string, the addr of the variable from LCD.
 * x0, y0, x1, y1, color is the position and color of the rectangle.
 */
void drawRectangle(const char* addr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* color){
	//char data[29] = "00030001";
	char data[29];

	char temp1[5], temp2[5], temp3[5], temp4[5];
	//strcat(data, uint16_tToHex(temp1, x0));
	//strcat(data, uint16_tToHex(temp2, y0));
	//strcat(data, uint16_tToHex(temp3, x1));
	//strcat(data, uint16_tToHex(temp4, y1));
	//strcat(data, color);
	sprintf(data, "%s%s%s%s%s%s", "00030001", uint16_tToHex(temp1, x0), uint16_tToHex(temp2, y0), uint16_tToHex(temp3, x1), uint16_tToHex(temp4, y1), color);
	setValueAsAttr(addr, 0, data);
}

/**
 * fill a rectangle in LCD.
 * addr is a constant HEX string , the addr of the variable from LCD.
 * x0, y0, x1, y1, color is the position and color of the rectangle.
 */
void fillRectangle(const char* addr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char* color){
	//char data[29] = "00040001";
	char data[29];

	char temp1[5], temp2[5], temp3[5], temp4[5];
	//strcat(data, uint16_tToHex(temp1, x0));
	//strcat(data, uint16_tToHex(temp2, y0));
	//strcat(data, uint16_tToHex(temp3, x1));
	//strcat(data, uint16_tToHex(temp4, y1));
	//strcat(data, color);
	sprintf(data, "%s%s%s%s%s%s", "00040001", uint16_tToHex(temp1, x0), uint16_tToHex(temp2, y0), uint16_tToHex(temp3, x1), uint16_tToHex(temp4, y1), color);
	setValueAsAttr(addr, 0, data);
}



/**
 * convert a float to a uint32_t value that LCD can analyze.
 * x is a float value, int_num is the number of integer, dec_num is the number of decimal.
 * return a uint32_t value.
 */
uint32_t convToLCD(const float x, uint8_t int_num, uint8_t dec_num){
	//double a = pow(10, int_num);
	//double b = pow(10, dec_num);
	//// function pow() return a double value, so "%" will not be used.
	//// if force conversion to integer, it will get a strange value, cuz the value will loss of accuracy.
	//return (x - (int)(x / a) * a) * b;
	long a = 1, b = 1;
	for(uint8_t i = int_num; i--;)	a *= 10;
	for(uint8_t i = dec_num; i--;)	b *= 10;
//	return long((x) * b) % (a * b);
//	return (x - (int)(x / a) * a) * b + 5 * pow(10, -7);
//	return long((x + 5 * pow(10, -7)) * b) % (a * b);
	return long((x + (x < 0 ? (-1) : 1) * 5 * pow(10, -7)) * b) % (a * b);
}

/**
 * convert a long to a uint32_t value that LCD can analyze.
 * x is a long value, int_num is the number of integer, dec_num is the number of decimal.
 * return a uint32_t value.
 */
uint32_t convToLCD(const long x, uint8_t int_num, uint8_t dec_num){
	long a = 1, b = 1;
	for(uint8_t i = int_num; i--;)	a *= 10;
	for(uint8_t i = dec_num; i--;)	b *= 10;
	return x % a * b;
}

/**
 * convert char data to HEX string data.(the length will multiple by 2)
 * hex is the HEX string pointer. (the length is *2 +1)
 * data is the char data.
 * return hex pointer.
 */
char* charToHex(char* hex, unsigned char* data, uint16_t dataLen){
	char* temp = hex;
	while(dataLen--){
		char high_byte = (uint8_t)(*data) / 16;
		if(high_byte >= 10) *temp++ = high_byte + 55;
		else *temp++ = high_byte + 48;

		char low_byte = (uint8_t)(*data) % 16;
		if(low_byte >= 10) *temp++ = low_byte + 55;
		else *temp++ = low_byte + 48;
		data++;
	}
	*temp = 0;
	return hex;
}

/**
 * convent uint8_t data to HEX String data. (eg. 10 --> "0A")
 * hex is the HEX string pointer.(3 bytes)
 * data is a uint8_t data.
 * return hex pointer.
 */
char* uint8_tToHex(char* hex, uint8_t data) {
	sprintf(hex, "%02X", data);
	return hex;
}

/**
 * convent uint16_t data to HEX String data. (eg. 10 --> "000A")
 * hex is the HEX string pointer.(5 bytes)
 * data is a uint16_t data.
 * return hex pointer.
 */
char* uint16_tToHex(char* hex, uint16_t data){
	sprintf(hex, "%04X", data);
	return hex;
}

/*
 * Compare a uint16_t with a HEX String
 * hex is the HEX string pointer.(5 bytes)
 * data is a uint16_t data.
 * return if the uint16_t is equal to the HEX string.
 */
bool uint16_tCompHex(const char * hex, uint16_t data){
#ifdef DWIN_HEX_OPERATE_USE_STR
	char hexData[5];
	return (strcmp(uint16_tToHex(hexData, data), hex) == 0) ? true : false;
#else
	return ((uint16_t)strtoul(hex, nullptr, HEX) == data);
#endif
}



/**********************************************************************************************/
/** Below, the index or addr is constant HEX string from the SROM */
/**********************************************************************************************/

/**
* jump to the specified page
* index is a constant HEX string , the addr of specified page.
*/
void setPage_P(const char * index) {
	char rel_index[5];
	strncpy_P(rel_index, index, 5);
	setPage(rel_index);
}

/** if current page is specified page. */
bool isPage_P(const char * index) {
	char rel_index[5];
	strncpy_P(rel_index, index, 5);
	return isPage(rel_index);
}

/** trigger a key defined in LCD. */
bool setPop_P(const char * key){
	char rel_key[3];
	strncpy_P(rel_key, key, 3);
	return setPop(rel_key);
}

/**
* get a value from lcd's variable.
* addr is a constant HEX string, the addr of the variable from LCD.
* return a uint16_t, return -1 if any error occur.
*/
void getValueForInt_P(const char * addr) {
	char rel_addr[5];
	strncpy_P(rel_addr, addr, 5);
	getValueForInt(rel_addr);
}

/**
* get a value from lcd's variable.
* addr is a constant HEX string, the addr of the variable from LCD.
* return a uint32_t, return -1 if any error occur.
*/
void getValueForLong_P(const char * addr){
	char rel_addr[5];
	strncpy_P(rel_addr, addr, 5);
	getValueForLong(rel_addr);
}

/**
* get a value from lcd's variable.
* addr is a constant HEX string, the addr of the variable from LCD.
* return a char pointer (string). The length is 32 chars.
*/
void getValueForString_P(const char * addr){
	char rel_addr[5];
	strncpy_P(rel_addr, addr, 5);
	getValueForString(rel_addr);
}

/**
* get a value from lcd's variable.
* temp is a pointer (any type), the data will return.
* addr is a HEX string, the addr of the variable's attribute from LCD.
* offset is the position that relative to the addr. That is the position of attribute.
* dataLen is the length of data that get from LCD.
*/
void getValueForAttr_P(const char * addr, uint8_t offset, uint8_t len){
	char rel_addr[5];
	strncpy_P(rel_addr, addr, 5);
	getValueForAttr(rel_addr, offset, len);
}

/**
* set a int value to lcd's variable
* addr is a constant HEX string, the addr of the variable from LCD.
* value is a uint16_t
*/
void setValueAsInt_P(const char * addr, uint16_t value){
	char rel_addr[5];
	strncpy_P(rel_addr, addr, 5);
	setValueAsInt(rel_addr, value);
}

/**
* set a int value to lcd's variable
* addr is a constant HEX string, the addr of the variable from LCD.
* data is a constant HEX string, if data is not 4 char, will send nothing.
*/
void setValueAsInt_P(const char * addr, const char * data){
	char rel_addr[5], rel_data[5];
	strncpy_P(rel_addr, addr, 5);
	strncpy_P(rel_data, data, 5);
	setValueAsInt(rel_addr, rel_data);
}

/**
* set a long value to lcd's variable
* addr is a constant HEX string, the addr of the variable from LCD.
* value is a uint32_t
*/
void setValueAsLong_P(const char * addr, uint32_t value){
	char rel_addr[5];
	strncpy_P(rel_addr, addr, 5);
	setValueAsLong(rel_addr, value);
}

/**
* set a long value to lcd's variable
* addr is a constant HEX string, the addr of the variable from LCD.
* data is a constant HEX string, if data is not 8 char, will send nothing.
*/
void setValueAsLong_P(const char * addr, const char * data){
	char rel_addr[5], rel_data[9];
	strncpy_P(rel_addr, addr, 5);
	strncpy_P(rel_data, data, 9);
	setValueAsLong(rel_addr, rel_data);
}

/**
* set a string value to lcd's variable.
* addr is a constant HEX string, the addr of the variable from LCD.
* space is the the space of addr.
* data is the char pointer (sting) that you want to set.
*/
void setValueAsString_P(const char * addr, const uint16_t space, const char * data) {
	char rel_addr[5];
	strncpy_P(rel_addr, addr, 5);
	setValueAsString(rel_addr, space, data);
}


/**
* set a value to lcd's variable's attribute.
* addr is a HEX string, the addr of the variable's attribute from LCD.
* offset is the position that relative to the addr. That is the position of attribute.
* data is a constant HEX string. The length depend on the attribute, so user must know the lcd's describe addr
*/
void setValueAsAttr_P(const char * addr, uint8_t offset, char * data){
	char rel_addr[5];
	strncpy_P(rel_addr, addr, 5);
	setValueAsAttr(rel_addr,offset, data);
}

/**
* draw a rectangle in LCD.
* addr is a constant HEX string, the addr of the variable from LCD.
* x0, y0, x1, y1, color is the position and color of the rectangle.
*/
void drawRectangle_P(const char * addr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char * color){
	char rel_addr[5], rel_color[5];
	strncpy_P(rel_addr, addr, 5);
	strncpy_P(rel_color, color, 5);
	drawRectangle(rel_addr, x0, y0, x1, y1, rel_color);
}

/**
* fill a rectangle in LCD.
* addr is a constant HEX string , the addr of the variable from LCD.
* x0, y0, x1, y1, color is the position and color of the rectangle.
*/
void fillRectangle_P(const char * addr, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const char * color){
	char rel_addr[5], rel_color[5];
	strncpy_P(rel_addr, addr, 5);
	strncpy_P(rel_color, color, 5);
	fillRectangle(rel_addr, x0, y0, x1, y1, rel_color);
}

/*
 * Compare a uint16_t with a HEX String
 * hex is the HEX string pointer.(5 bytes)
 * data is a uint16_t data.
 * return if the uint16_t is equal to the HEX string.
 */
bool uint16_tCompHex_P(const char *hex, uint16_t data){
	char rel_hex[5];
	strncpy_P(rel_hex, hex, 5);
	return uint16_tCompHex(rel_hex, data);
}
#endif //CREATBOT_LCD

