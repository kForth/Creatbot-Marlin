#ifndef OEM_INFO_H
#define OEM_INFO_H

#define DMMini        1
#define DM            2
#define DMPlus        3
#define DX            4
#define DXPlus        5
#define DE            6
#define DEPlus        7
#define D600          8
#define D600Mini      9
#define D600SE        10
#define D600Pro       11
#define F160          12
#define F160PEEK      13
#define F200          14
#define F220          15
#define F260          16
#define F300          17
#define F430          18
#define F430Plus      19
#define F1000         20
#define PEEK300       21

#define en    0
#define cn    1

#define OEM_NAME "CreatBot"

#if MODEL == DMMini
  #define MODEL_NAME "DM Mini"
#elif MODEL == DM
  #define MODEL_NAME "DM"
#elif MODEL == DMPlus
  #define MODEL_NAME "DM Plus"
#elif MODEL == DX
  #define MODEL_NAME "DX"
#elif MODEL == DXPlus
  #define MODEL_NAME "DX Plus"
#elif MODEL == DE
  #define MODEL_NAME "DE"
#elif MODEL == DEPlus
  #define MODEL_NAME "DE Plus"
#elif MODEL == D600
  #define MODEL_NAME "D600"
#elif MODEL == D600Mini
  #define MODEL_NAME "D600 Mini"
#elif MODEL == D600SE
  #define MODEL_NAME "D600 SE"
#elif MODEL == D600Pro
  #define MODEL_NAME "D600 Pro"
#elif MODEL == F160
  #define MODEL_NAME "F160"
#elif MODEL == F160PEEK
  #define MODEL_NAME "F160-PEEK"
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
#elif MODEL == F430Plus
  #define MODEL_NAME "F430Plus"
#elif MODEL == F1000
  #define MODEL_NAME "F1000"
#elif MODEL == PEEK300
  #define MODEL_NAME "PEEK-300"
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

#if MODEL >= D600
  #define MachineName OEM_NAME " " MODEL_NAME
#else
  #define MachineName OEM_NAME " " MODEL_NAME "0" STRINGIFY(EXTRUDERS)
#endif

#endif  // OEM_INFO_H
