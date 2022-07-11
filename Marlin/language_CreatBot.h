/*
 * language_CreatBot.h
 *
 *  Created on: 2017年5月5日
 *      Author: CreatBot-SW
 */

#ifndef MARLIN_LANGUAGE_CREATBOT_H_
#define MARLIN_LANGUAGE_CREATBOT_H_

// en cn fr es it
#define en				0
#define cn				1
#define fr				2
#define es				3
#define it				4
#define pl				5

#if LANGUAGE == cn
	#define DISPLAY_CHARSET_CREATBOT_CN
#endif

/*********************** By LYN (our self lcd) *********************************/
#if LANGUAGE_NUM == cn
	#define MSG_FIRMWARE_VERSION		"\x7F\x80\x81\x82"							//固件版本
#else
	#define MSG_FIRMWARE_VERSION		"Firmware"
#endif

#if LANGUAGE == cn
	#define MSG_ELAPSED_TIME				"\x83\x84\xCA\xF3"							//使用时间
#elif LANGUAGE == es
	#define MSG_ELAPSED_TIME				"Tiempo de uso"
#elif LANGUAGE == it
	#define MSG_ELAPSED_TIME				"Tempo utilizzo"
#else
	#define MSG_ELAPSED_TIME				"Time used"
#endif

#if LANGUAGE == cn
	#define MSG_UNLOAD_FILAMENT			"\xC8\x93\xD9\xDA"							//退出耗材
#elif LANGUAGE == es
	#define MSG_UNLOAD_FILAMENT			"Descargar Filamento"
#elif LANGUAGE == it
	#define MSG_UNLOAD_FILAMENT			"Ritrazione Filamento"
#else
	#define MSG_UNLOAD_FILAMENT			"Unload Filament"
#endif

#if LANGUAGE == cn
	#define MSG_UNLOAD_FILAMENT_E		"\xC8\x93\xD9\xDA "							//退出耗材_
#elif LANGUAGE == es
	#define MSG_UNLOAD_FILAMENT_E		"Descargar Fil. "
#elif LANGUAGE == it
	#define MSG_UNLOAD_FILAMENT_E		"Ritrazione Fil. "
#else
	#define MSG_UNLOAD_FILAMENT_E		"Unload Filament "
#endif

#if LANGUAGE == cn
	#define MSG_LOAD_FILAMENT				"\xAF\xC8\xD9\xDA"							//加载耗材
#elif LANGUAGE == es
	#define MSG_LOAD_FILAMENT				"Cargar Filamento"
#elif LANGUAGE == it
	#define MSG_LOAD_FILAMENT				"Carica Filamento"
#else
	#define MSG_LOAD_FILAMENT				"Load Filament"
#endif

#if LANGUAGE == cn
	#define MSG_PRINT_PAUSED				"\xCD\xCE\x8F\xCC\xAD"					//打印已暂停
#elif LANGUAGE == es
	#define MSG_PRINT_PAUSED				"Imp. en Pausa"
#elif LANGUAGE == it
	#define MSG_PRINT_PAUSED				"Stampa in pausa"
#else
	#define MSG_PRINT_PAUSED				"Print paused"
#endif

#if LANGUAGE == cn
	#define	MSG_UNLOADING_FILAMENT	"\xD4\xD5\xC8\x93\xD9\xDA..."		//正在退出耗材...
#elif LANGUAGE == es
	#define	MSG_UNLOADING_FILAMENT	"Descargando Fil. ..."
#elif LANGUAGE == it
	#define	MSG_UNLOADING_FILAMENT	"Ritrazione Fil. ..."
#else
	#define	MSG_UNLOADING_FILAMENT	"Unloading Filament..."
#endif

#if LANGUAGE == cn
	#define		MSG_ATUO_SHUTDOWN			"\x9F\xA0\xE2\x9E\xEA... "				//自动关机中...
#elif LANGUAGE == es
	#define		MSG_ATUO_SHUTDOWN			"Apagando... "
#elif LANGUAGE == it
	#define		MSG_ATUO_SHUTDOWN			"Spegnimento... "
#else
	#define		MSG_ATUO_SHUTDOWN			"Shutting down.. "
#endif

#if LANGUAGE == cn
	#define MSG_FILAMENT_NOT_READY	"\xD9\xDA\xD6\x87\x88"					//耗材未就绪
#elif LANGUAGE == es
	#define MSG_FILAMENT_NOT_READY	"Filamento no listo"
#elif LANGUAGE == it
	#define MSG_FILAMENT_NOT_READY	"Errore filamento"
#else
	#define MSG_FILAMENT_NOT_READY	"Filament Error"
#endif

#if LANGUAGE == cn
	#define MSG_HOME_RESUME_PRINT		"\x95\xA2\xA3\xCF\xD0\xCD\xCE"	//回零位继续打印
#elif LANGUAGE == es
	#define MSG_HOME_RESUME_PRINT		"Home & Reanudar impres."
#elif LANGUAGE == it
	#define MSG_HOME_RESUME_PRINT		"Home & riprendi stampa"
#else
	#define MSG_HOME_RESUME_PRINT		"Home & Resume print"
#endif

#if LANGUAGE == cn
	#define MSG_CANCEL							"\xD1\xD2"											//取消
#elif LANGUAGE == es
	#define MSG_CANCEL							"Cancelar"
#elif LANGUAGE == it
	#define MSG_CANCEL							"Stop"
#else
	#define MSG_CANCEL							"Cancel"
#endif

#if LANGUAGE == cn
	#define MSG_SHUTTING						"\xD4\xD5\xE2\x9E..."						//正在关机...
#elif LANGUAGE == es
	#define MSG_SHUTTING						"Apagando..."
#elif LANGUAGE == it
	#define MSG_SHUTTING						"Spegnimento..."
#else
	#define MSG_SHUTTING						"Shutting down..."
#endif

#if LANGUAGE == cn
	#define MSG_ACCIDENT						"\x8F\xC5\xC6,\xD4\xD5\xE2\x9E..."		//已保存，正在关机...
#elif LANGUAGE == es
	#define MSG_ACCIDENT						"Aalvado & Apagando..."
#elif LANGUAGE == it
	#define MSG_ACCIDENT						"Salva e spegni..."
#else
	#define MSG_ACCIDENT						"Saved & Shutting down..."
#endif

#if LANGUAGE == cn
	#define MSG_COOLING							"\xC9\xEB\xEC\xA7\xA8\xED\xEE.."			//请等待喷头冷却...
#elif LANGUAGE == it
	#define MSG_COOLING							"Raffreddamento estrusore.."
#else
	#define MSG_COOLING							"cooling hotend.."
#endif

#if LANGUAGE == cn
	#define MSG_AUTO_FAN_SPEED			"\xBC\xC0\xBD\xBE\xBA\xBB"			//温控风扇速度
#elif LANGUAGE == es
	#define MSG_AUTO_FAN_SPEED			"Velocidad ventilador auto"
#elif LANGUAGE == it
	#define MSG_AUTO_FAN_SPEED			"Velocita ventola auto"
#else
	#define MSG_AUTO_FAN_SPEED			"Auto fan speed"
#endif

#if LANGUAGE == cn
	#define MSG_PRINTING						"\xD4\xD5\xCD\xCE..."						//正在打印...
#elif LANGUAGE == es
	#define MSG_PRINTING						"Imprimiendo..."
#elif LANGUAGE == it
	#define MSG_PRINTING						"Stampa..."
#elif LANGUAGE == pl
	#define MSG_PRINTING						"Drukowanie..."
#else
	#define MSG_PRINTING						"Printing..."
#endif

#if LANGUAGE == cn
	#define MSG_PRINTFINISHED				"\xCD\xCE\x8F\xF1\xF2."					//打印已完成.
#elif LANGUAGE == es
	#define MSG_PRINTFINISHED				"Imp. finalizada"
#elif LANGUAGE == it
	#define MSG_PRINTFINISHED				"Stampa terminata"
#elif LANGUAGE == pl
	#define MSG_PRINTFINISHED				"Wydrukowano."
#else
	#define MSG_PRINTFINISHED				"Print finished."
#endif

#if LANGUAGE == cn
	#define MSG_PREHEAT_MENU				"\xA4\xA5\x97\x98\x19"					//预热菜单
#elif LANGUAGE == es
	#define MSG_PREHEAT_MENU				"Precalentamiento"
#elif LANGUAGE == it
	#define MSG_PREHEAT_MENU				"Preriscaldamento"
#else
	#define MSG_PREHEAT_MENU				"Preheat"
#endif

#if LANGUAGE == cn
	#define MSG_PRINT_MENU					"\xCD\xCE\x97\x98"							//打印菜单
#elif LANGUAGE == es
	#define MSG_PRINT_MENU					"Imprimir"
#elif LANGUAGE == it
	#define MSG_PRINT_MENU					"Stampa"
#else
	#define MSG_PRINT_MENU					"Print"
#endif

#if LANGUAGE == cn
	#define MSG_MOVE_AXIS_MENU			"\xB0\xB1\x97\x98"							//移轴菜单
#elif LANGUAGE == es
	#define MSG_MOVE_AXIS_MENU			"Mover ejes"
#elif LANGUAGE == it
	#define MSG_MOVE_AXIS_MENU			"Muovi asse"
#else
	#define MSG_MOVE_AXIS_MENU			"Move axis"
#endif

#if LANGUAGE == cn
	#define MSG_FILAMENT_MENU				"\xD9\xDA\x97\x98"							//耗材菜单
#elif LANGUAGE == es
	#define MSG_FILAMENT_MENU				"Filamento"
#elif LANGUAGE == it
	#define MSG_FILAMENT_MENU				"Filamento"
#else
	#define MSG_FILAMENT_MENU				"Filament"
#endif

#if LANGUAGE == cn
	#define MSG_SETTING_MENU				"\xAB\xAC\x97\x98"							//设置菜单
#elif LANGUAGE == es
	#define MSG_SETTING_MENU				"Configuracion"
#elif LANGUAGE == it
	#define MSG_SETTING_MENU				"Impostazioni"
#else
	#define MSG_SETTING_MENU				"Setting"
#endif

#if LANGUAGE == cn
	#define MSG_LEVELING_BED				"\x89\x8A\x8B\x8C\x8D"					//调平工作台
#elif LANGUAGE == es
	#define MSG_LEVELING_BED				"Nivel cama"
#elif LANGUAGE == it
	#define MSG_LEVELING_BED				"Livellamento piano"
#else
	#define MSG_LEVELING_BED				"Leveling bed"
#endif

#if LANGUAGE == cn
	#define MSG_PROBE_BED						"\xB5\xBF\x8B\x8C\x8D"					//测量工作台
#else
	#define MSG_PROBE_BED						"Probe bed"
#endif

#if LANGUAGE == cn
	#define MSG_PROBING_BED					"\xB5\xBF\xEA..."								//测量中...
#else
	#define MSG_PROBING_BED					"Probing... Wait.."
#endif

#if LANGUAGE == cn
	#define MSG_PROBE_BED_DONE			"\xB5\xBF\xF1\xF2!"							//测量完成
#else
	#define MSG_PROBE_BED_DONE			"Probe Done!"
#endif

#if LANGUAGE == cn
	#define MSG_AUTO_LEVELING				"\x9F\xA0\x89\x8A"							//自动调平
#else
	#define MSG_AUTO_LEVELING				"Auto leveling"
#endif

#define MSG_REMAIN								"Remain "
#define MSG_REMAIN_LESS						"< 2 mins"


/*******************************************************************************/

/********************* By LYN (dwin_lcd , encoded by GB2312.) ******************/
#if LANGUAGE == cn
	#define DWIN_MSG_WELCOME						MACHINE_NAME " \xD7\xBC\xB1\xB8\xBE\xCD\xD0\xF7."				//准备就绪.
#else
	#define DWIN_MSG_WELCOME						WELCOME_MSG
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_PRINTING						"\xD5\xFD\xD4\xDA\xB4\xF2\xD3\xA1..."										//正在打印...
#else
	#define DWIN_MSG_PRINTING						MSG_PRINTING
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_RESUMING						"\xD5\xFD\xD4\xDA\xB4\xF2\xD3\xA1..."										//正在打印...
#else
	#define DWIN_MSG_RESUMING						MSG_RESUMING
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_PRINTFINISHED			"\xB4\xF2\xD3\xA1\xD2\xD1\xCD\xEA\xB3\xC9."							//打印已完成.
#else
	#define DWIN_MSG_PRINTFINISHED			MSG_PRINTFINISHED
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_PRINT_PAUSED				"\xB4\xF2\xD3\xA1\xD2\xD1\xD4\xDD\xCD\xA3."							//打印已暂停.
#else
	#define DWIN_MSG_PRINT_PAUSED				MSG_PRINT_PAUSED
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_STOPPED						"\xB4\xF2\xD3\xA1\xD2\xD1\xC8\xA1\xCF\xFB."							//打印已取消.
#else
	#define DWIN_MSG_STOPPED						MSG_STOPPED
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_HEATING						"\xC5\xE7\xCD\xB7\xBC\xD3\xC8\xC8\xD6\xD0..."						//喷头预热中...
#else
	#define DWIN_MSG_HEATING						MSG_HEATING
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_HEATING_COMPLETE		"\xC5\xE7\xCD\xB7\xBC\xD3\xC8\xC8\xCD\xEA\xB3\xC9."			//喷头预热完成.
#else
	#define DWIN_MSG_HEATING_COMPLETE		MSG_HEATING_COMPLETE
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_BED_HEATING				"\xB9\xA4\xD7\xF7\xCC\xA8\xBC\xD3\xC8\xC8\xD6\xD0..."		//工作台预热中...
#else
	#define DWIN_MSG_BED_HEATING				MSG_BED_HEATING
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_BED_DONE						"\xB9\xA4\xD7\xF7\xCC\xA8\xBC\xD3\xC8\xC8\xCD\xEA\xB3\xC9."		//工作台预热完成.
#else
	#define DWIN_MSG_BED_DONE						MSG_BED_DONE
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_ATUO_SHUTDOWN			"\xD7\xD4\xB6\xAF\xB9\xD8\xBB\xFA\xD6\xD0..."						//自动关机中...
#else
	#define DWIN_MSG_ATUO_SHUTDOWN			MSG_ATUO_SHUTDOWN
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_FILAMENT_NOT_READY	"\xBA\xC4\xB2\xC4\xCE\xB4\xBE\xCD\xD0\xF7."							//耗材未就绪.
#else
	#define DWIN_MSG_FILAMENT_NOT_READY	MSG_FILAMENT_NOT_READY
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_CHANGE_FILAMENT	"\xe6\x9b\xb4\xe6\x8d\xa2\xe7\x81\xaf\xe4\xb8\x9d."							//耗材未就绪.
#else
	#define DWIN_MSG_CHANGE_FILAMENT	MSG_FILAMENTCHANGE
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_SD_INSERTED				"SD\xBF\xA8\xD2\xD1\xB2\xE5\xC8\xEB."										//SD卡已插入.
#else
	#define DWIN_MSG_SD_INSERTED				MSG_SD_INSERTED
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_SD_REMOVED					"SD\xBF\xA8\xD2\xD1\xB0\xCE\xB3\xF6."										//SD卡已拔出.
#else
	#define DWIN_MSG_SD_REMOVED					MSG_SD_REMOVED
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_NO_FILE						"\xBF\xD5\xCE\xC4\xBC\xFE\xBC\xD0!"											//空文件夹!
#else
	#define DWIN_MSG_NO_FILE						"Folder is Empty!"
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_USB_INSERTED				"U\xC5\xCC\xD2\xD1\xB2\xE5\xC8\xEB."										//U盘已插入.
#else
	#define DWIN_MSG_USB_INSERTED				"USB disk inserted"
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_USB_REMOVED				"U\xC5\xCC\xD2\xD1\xB0\xCE\xB3\xF6."										//U盘已拔出.
#else
	#define DWIN_MSG_USB_REMOVED				"USB disk removed"
#endif

#if LANGUAGE == cn
	#define DWIN_MSG_USB_READY					"U\xC5\xCC\xD2\xD1\xBE\xCD\xD0\xF7."										//U盘已就绪.
#else
	#define DWIN_MSG_USB_READY					"USB disk ready"
#endif
/*******************************************************************************/

#if LANGUAGE == cn
	#define WELCOME_MSG                         MACHINE_NAME "\x85\x86\x87\x88"				//准备就绪
	#define MSG_SD_INSERTED                     "SD\x8E\x8F\x90\x91"									//SD卡已插入
	#define MSG_SD_REMOVED                      "SD\x8E\x8F\x92\x93"									//SD卡已拨出
	#define MSG_MAIN                            "\x94\x95\x96\x97\x98"								//返回主菜单
	#define MSG_AUTOSTART                       "\x9F\xA0\xDB\xDC"										//自动开始
	#define MSG_DISABLE_STEPPERS                "\x99\x9A\x9B\x9C\x9D\x9E"						//释放所有电机
	#define MSG_AUTO_HOME                       "\x9F\xA0\x95\xA1\xA2\xA3"						//自动回归零位
	#define MSG_LEVEL_BED_HOMING                "Homing XYZ"
	#define MSG_LEVEL_BED_WAITING               "Click to Begin"
	#define MSG_LEVEL_BED_DONE                  "Leveling Done!"
	#define MSG_LEVEL_BED_CANCEL                "Cancel"
	#define MSG_SET_HOME_OFFSETS                "Set home offsets"
	#define MSG_HOME_OFFSETS_APPLIED            "Offsets applied"
	#define MSG_SET_ORIGIN                      "\xAB\xC2\xA2\xA3"										//设定零位
	#define MSG_PREHEAT_1                       "\xA4\xA5\x9E\xA6 PLA"								//预热机器 PLA
	#define MSG_PREHEAT_1_N                     "\xA4\xA5\x8B\x8C\x8D/\xA7\xA8 "			//预热工作台/喷头
	#define MSG_PREHEAT_1_END										"\xA4\xA5\xA7\xA8 "										//预热喷头
	#define MSG_PREHEAT_1_ALL                   "\xA4\xA5\xA9\xAA\xA7\xA8"						//预热全部喷头
	#define MSG_PREHEAT_1_BEDONLY               "\xA4\xA5\x8B\x8C\x8D"								//预热工作台
	#define MSG_PREHEAT_1_SETTINGS              "\xA4\xA5\x9E\xA6\xAB\xAC PLA"				//预热机器设置 PLA
	#define MSG_PREHEAT_2                       "\xA4\xA5\x9E\xA6 ABS"								//预热机器 ABS
	#define MSG_PREHEAT_2_N                     MSG_PREHEAT_1_N												//预热工作台/喷头
	#define MSG_PREHEAT_2_END										MSG_PREHEAT_1_END											//预热喷头
	#define MSG_PREHEAT_2_ALL                   MSG_PREHEAT_1_ALL											//预热全部喷头
	#define MSG_PREHEAT_2_BEDONLY               MSG_PREHEAT_1_BEDONLY									//预热工作台
	#define MSG_PREHEAT_2_SETTINGS              "\xA4\xA5\x9E\xA6\xAB\xAC ABS"				//预热机器设置 ABS
	#define MSG_COOLDOWN                        "\xAD\xAE\x9B\x9C\xAF\xA5"						//停止所有加热
	#define MSG_SWITCH_PS_ON                    "Switch power on"
	#define MSG_SWITCH_PS_OFF                   "Switch power off"
	#define MSG_EXTRUDE                         "\xB3\xB4"														//送丝
	#define MSG_RETRACT                         "\x95\xB4"														//回丝
	#define MSG_MOVE_AXIS                       "\xB0\xB1\xB2\xB3\xB4\xB5\xB6"				//移轴及送丝测试
	#define MSG_LEVEL_BED                       "Level bed"
	#define MSG_MOVE_X                          "\xB0\xA0 X \xB1"											//移动 X 轴
	#define MSG_MOVE_Y                          "\xB0\xA0 Y \xB1"											//移动 Y 轴
	#define MSG_MOVE_Z                          "\xB0\xA0 Z \xB1"											//移动 Z 轴
	#define MSG_MOVE_E                          "\xA7\xA8\xB3\xB4"										//喷头送丝
	#define MSG_MOVE_01MM                       "0.1 \xB7\xB8"												//0.1 毫米
	#define MSG_MOVE_1MM                        "  1 \xB7\xB8"												//  1 毫米
	#define MSG_MOVE_10MM                       " 10 \xB7\xB8"												// 10 毫米
	#define MSG_SPEED                           "\xA9\xB9\xBA\xBB"										//全局速度
	#define MSG_NOZZLE                          "\xA7\xA8\xBC\xBB"										//喷头温度
	#define MSG_BED                             "\x8B\x8C\x8D\xBC\xBB"								//工作台温度
	#define MSG_FAN_SPEED                       "\xBD\xBE\xBA\xBB"										//风扇速度
	#define MSG_FLOW                            "\xB3\xB4\xBF"												//送丝量
	#define MSG_CONTROL                         "\xC0\xC1\xB2\xAB\xAC\x97\x98"				//控制及设置菜单
	#define MSG_MIN                             LCD_STR_THERMOMETER " \xDD\xDE"				//(温度计)最小
	#define MSG_MAX                             LCD_STR_THERMOMETER " \xDD\xDF"				//(温度计)最大
	#define MSG_FACTOR                          LCD_STR_THERMOMETER " \xE0\xE1"				//(温度计)因子
	#define MSG_AUTOTEMP                        "\x9F\xA0\x89\xBC"										//自动调温
	#define MSG_ON                              "\xDB"																//开
	#define MSG_OFF                             "\xE2"																//关
	#define MSG_PID_P                           "PID-P"
	#define MSG_PID_I                           "PID-I"
	#define MSG_PID_D                           "PID-D"
	#define MSG_PID_C                           "PID-C"
	#define MSG_ACC                             "Accel"
	#define MSG_VX_JERK                         "Vx-jerk"
	#define MSG_VY_JERK                         "Vy-jerk"
	#define MSG_VZ_JERK                         "Vz-jerk"
	#define MSG_VE_JERK                         "Ve-jerk"
	#define MSG_VMAX                            "Vmax "
	#define MSG_VMIN                            "Vmin"
	#define MSG_VTRAV_MIN                       "VTrav min"
	#define MSG_AMAX                            "Amax "
	#define MSG_A_RETRACT                       "A-retract"
	#define MSG_A_TRAVEL                        "A-travel"
	#define MSG_XSTEPS                          "X\xB1\xE4\xE5\xC4"										//X轴步进数
	#define MSG_YSTEPS                          "Y\xB1\xE4\xE5\xC4"										//Y轴步进数
	#define MSG_ZSTEPS                          "Z\xB1\xE4\xE5\xC4"										//Z轴步进数
	#define MSG_ESTEPS                          "E\xB1\xE4\xE5\xC4"										//E轴步进数
	#define MSG_E1STEPS                         "E1\xB1\xE4\xE5\xC4"									//E1轴步进数
	#define MSG_E2STEPS                         "E2\xB1\xE4\xE5\xC4"									//E2轴步进数
	#define MSG_E3STEPS                         "E3\xB1\xE4\xE5\xC4"									//E3轴步进数
	#define MSG_E4STEPS                         "E4\xB1\xE4\xE5\xC4"									//E4轴步进数
	#define MSG_E5STEPS                         "E5\xB1\xE4\xE5\xC4"									//E5轴步进数
	#define MSG_TEMPERATURE                     "\xAB\xC2\xBC\xBB"										//设定温度
	#define MSG_MOTION                          "\x9E\xA6\xC3\xC4"										//机器参数
	#define MSG_FILAMENT                        "Filament"
	#define MSG_VOLUMETRIC_ENABLED              "E in mm3"
	#define MSG_FILAMENT_DIAM                   "Fil. Dia."
	#define MSG_CONTRAST                        "LCD contrast"
	#define MSG_STORE_EEPROM                    "\xC5\xC6\xAB\xAC"										//保存设置
	#define MSG_LOAD_EEPROM                     "Load memory"
	#define MSG_RESTORE_FAILSAFE                "\x93\xC7\xAB\xAC"										//出厂设置
	#define MSG_REFRESH                         "Refresh"
	#define MSG_WATCH                           "\x94\x95\x96\xEF\xF0"								//返回主界面
	#define MSG_PREPARE                         "\xA4\xA5\xB2\xB0\xB1\x97\x98"				//预热及移轴菜单
	#define MSG_TUNE                            "\xCD\xCE\x89\xCB"										//打印调整
	#define MSG_PAUSE_PRINT                     "\xCC\xAD\xCD\xCE"										//暂停打印
	#define MSG_RESUME_PRINT                    "\xCF\xD0\xCD\xCE"										//继续打印
	#define MSG_STOP_PRINT                      "\xD1\xD2\xCD\xCE"										//取消打印
	#define MSG_CARD_MENU                       "\xCD\xDBSD\x8E"											//打开SD卡
	#define MSG_NO_CARD                         "SD\x8E\xD6\x90\x91"									//SD卡未插入
	#define MSG_DWELL                           "Sleep..."
	#define MSG_USERWAIT                        "Wait for user..."
	#define MSG_RESUMING                        "Resuming print"
	#define MSG_PRINT_ABORTED                   "Print aborted"
	#define MSG_NO_MOVE                         "No move."
	#define MSG_KILLED                          "KILLED. "
	#define MSG_STOPPED                         "\xCD\xCE\x8F\xD1\xD2"								//打印已取消
	#define MSG_CONTROL_RETRACT                 "Retract mm"
	#define MSG_CONTROL_RETRACT_SWAP            "Swap Re.mm"
	#define MSG_CONTROL_RETRACTF                "Retract  V"
	#define MSG_CONTROL_RETRACT_ZLIFT           "Hop mm"
	#define MSG_CONTROL_RETRACT_RECOVER         "UnRet mm"
	#define MSG_CONTROL_RETRACT_RECOVER_SWAP    "S UnRet mm"
	#define MSG_CONTROL_RETRACT_RECOVERF        "UnRet  V"
	#define MSG_AUTORETRACT                     "AutoRetr."
	#define MSG_FILAMENTCHANGE                  "\xD7\xD8\xD9\xDA"										//更换耗材
	#define MSG_INIT_SDCARD                     "Init. SD card"
	#define MSG_CNG_SDCARD                      "\xD7\xD8\x53\x44\x8E"								//更换SD卡
	#define MSG_ZPROBE_OUT                      "Z probe out. bed"
	#define MSG_HOME                            "Home"  // Used as MSG_HOME " " MSG_X MSG_Y MSG_Z " " MSG_FIRST
	#define MSG_FIRST                           "first"
	#define MSG_ZPROBE_ZOFFSET                  "Z Offset"
	#define MSG_BABYSTEP_X                      "Babystep X"
	#define MSG_BABYSTEP_Y                      "Babystep Y"
	#define MSG_BABYSTEP_Z                      "Babystep Z"
	#define MSG_ENDSTOP_ABORT                   "Endstop abort"
	#define MSG_HEATING_FAILED_LCD              "Heating failed"
	#define MSG_ERR_REDUNDANT_TEMP              "Err: REDUNDANT TEMP"
	#define MSG_THERMAL_RUNAWAY                 "THERMAL RUNAWAY"
	#define MSG_HEATING													"\xA7\xA8\xAF\xA5\xEA..."							//喷头加热中
	#define MSG_HEATING_COMPLETE								"\xA7\xA8\xAF\xA5\xF1\xF2"						//喷头加热完成
	#define MSG_BED_HEATING											"\x8B\x8C\x8D\xAF\xA5\xEA..."					//工作台加热中
	#define MSG_BED_DONE												"\x8B\x8C\x8D\xAF\xA5\xF1\xF2"				//工作台加热完成
	#define MSG_INFO_MENU                       "About Printer"												//关于本机
  #define MSG_INFO_PRINTER_MENU               "Printer Info"												//固件信息
	#define MSG_INFO_STATS_MENU                 "Printer Stats"												//统计信息
  #define MSG_INFO_BOARD_MENU                 "Board Info"													//主板信息
  #define MSG_INFO_THERMISTOR_MENU            "Thermistors"													//温控器信息
  #define MSG_INFO_EXTRUDERS                  "Extruders"														//喷嘴数量
  #define MSG_INFO_BAUDRATE                   "Baud"																//波特率
  #define MSG_INFO_PROTOCOL                   "Protocol"														//协议版本
  #define MSG_LIGHTS_ON                       "Case light on"												//开启灯光
  #define MSG_LIGHTS_OFF                      "Case light off"											//关闭灯光
#endif

// LANGUAGE will be used as string, so we should undef them
#undef en
#undef cn
#undef fr
#undef es
#undef it
#undef pl

#endif /* MARLIN_LANGUAGE_CREATBOT_H_ */
