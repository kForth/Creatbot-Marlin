#ifndef OEM_INFO_H
#define OEM_INFO_H

#define OEM_NAME "CreatBot"

#define Other_MODEL   -1
#define DM_Mini       1
#define DM            2
#define DM_Plus       3
#define DX            4
#define DX_Plus       5
#define DE            6
#define DE_Plus       7
#define D600          8
#define D600_Mini     9
#define D600_SE       10
#define D600_Pro      11
#define F160          12
#define F200          13
#define F220          14
#define F260          15
#define F300          16
#define F430          17
#define F1000         18
#define PEEK300       19

#define en    0
#define cn    1
#define fr    2
#define es    3
#define it    4
#define pl    5

#define VER_POS_1    {2, 63}    // left-bottom
#define VER_POS_2    {104, 7}   // right-top

// Thermistor Types
#define KTC -1
#define NTC 1

#if MODEL == DM_Mini
  #define MODEL_NAME "DM-"
#elif MODEL == DM
  #define MODEL_NAME "DM"
#elif MODEL == DM_Plus
  #define MODEL_NAME "DM+"
#elif MODEL == DX
  #define MODEL_NAME "DX"
#elif MODEL == DX_Plus
  #define MODEL_NAME "DX+"
#elif MODEL == DE
  #define MODEL_NAME "DE"
#elif MODEL == DE_Plus
  #define MODEL_NAME "DE+"
#elif MODEL == D600
  #define MODEL_NAME "D600"
#elif MODEL == D600_Mini
  #define MODEL_NAME "D600 Mini"
#elif MODEL == D600_SE
  #define MODEL_NAME "D600 SE"
#elif MODEL == D600_Pro
  #define MODEL_NAME "D600 Pro"
#elif MODEL == F160
  #define MODEL_NAME "F160"
#elif MODEL == F200
  #define MODEL_NAME "F200"
#elif MODEL == F220
  #define MODEL_NAME "F220"
#elif MODEL == F260
  #define MODEL_NAME "F260"
#elif MODEL == F300
  #define MODEL_NAME "F300"
#elif MODEL == F430
  #define MODEL_NAME "F430"
#elif MODEL == F1000
  #define MODEL_NAME "F1000"
#elif MODEL == PEEK300
  #define MODEL_NAME "PEEK-300"
#elif MODEL == Other_MODEL
  #define MODEL_NAME ""
#else
  #error "We don't have this MODEL."
#endif

#if LANGUAGE == en
  #define LANGUAGE_NUM 0
#elif LANGUAGE == cn
  #define LANGUAGE_NUM 1
#elif LANGUAGE == fr
  #define LANGUAGE_NUM 2
#elif LANGUAGE == es
  #define LANGUAGE_NUM 3
#elif LANGUAGE == it
  #define LANGUAGE_NUM 4
#elif LANGUAGE == pl
  #define LANGUAGE_NUM 5
#else
  #define LANGUAGE_NUM 0
#endif

// LANGUAGE will be used as string, so we should undef them
#undef en
#undef cn
#undef fr
#undef es
#undef it
#undef pl

#if MODEL == D600 || MODEL == D600_SE || MODEL == D600_Pro || MODEL == F300 || MODEL == F160 || MODEL == F200 || MODEL == F220 || MODEL == F260 || MODEL == F430 || MODEL == F1000|| MODEL == PEEK300
  #define CREATBOT_MODEL OEM_NAME " " MODEL_NAME
#else
  #define CREATBOT_MODEL OEM_NAME " " MODEL_NAME "0" STRINGIFY(EXTRUDERS)
#endif

#define VER_POS VER_POS_1

#endif	// OEM_INFO_H
