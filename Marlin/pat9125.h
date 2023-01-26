#pragma once

#include <inttypes.h>

//PAT9125 configuration
//#define PAT9125_SWSPI // software SPI mode (incomplete)
#ifdef SWI2C_SCL
#define PAT9125_SWI2C   // software I2C mode
#else
#define PAT9125_I2C     // hardware I2C mode
#endif

#define PAT9125_I2C_ADDR  0x75  //ID=LO
//#define PAT9125_I2C_ADDR  0x79  //ID=HI
//#define PAT9125_I2C_ADDR  0x73  //ID=NC
#define PAT9125_XRES      0
#define PAT9125_YRES      240                   // maximum resolution (5*X cpi)
#define PAT9125_YRES_MM   (5*PAT9125_YRES/25.4) // counts per mm
#define PAT9125_INVERT_X  0 //1 means flipped
#define PAT9125_INVERT_Y  1 //1 means flipped
#define PAT9125_SWAP_XY   0 //X is Y and Y is X
#define PAT9125_12B_RES   1 //8bit or 12bit signed motion data
#define PAT9125_NEW_INIT  1 //set to 1 to use the magic sequence provided by pixart.

extern int16_t pat9125_x;
extern int16_t pat9125_y;
extern uint8_t pat9125_b;
extern uint8_t pat9125_s;

extern uint8_t pat9125_probe(void);     // Return non-zero if PAT9125 can be trivially detected
extern uint8_t pat9125_init(void);
extern uint8_t pat9125_update(void);    // update all sensor data
extern uint8_t pat9125_update_y(void);  // update _y only
extern uint8_t pat9125_update_bs(void); // update _b/_s only
