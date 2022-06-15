/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * configuration_store.cpp
 *
 * Settings and EEPROM storage
 *
 * IMPORTANT:  Whenever there are changes made to the variables stored in EEPROM
 * in the functions below, also increment the version number. This makes sure that
 * the default values are used whenever there is a change to the data, to prevent
 * wrong data being written to the variables.
 *
 * ALSO: Variables in the Store and Retrieve sections must be in the same order.
 *       If a feature is disabled, some data must still be written that, when read,
 *       either sets a Sane Default, or results in No Change to the existing value.
 *
 */

/**
 * V41 EEPROM Layout:
 *
 *  100  Version                                    (char x12)
 *
 * Base Setting:																																							(160 + 48)
 *  112  M92 XYZE  planner.axis_steps_per_mm        (float x4 ... x8) + 16
 *  128  M203 XYZE planner.max_feedrate_mm_s        (float x4 ... x8) + 16
 *  144  M201 XYZE planner.max_acceleration_mm_per_s2 (uint32_t x4 ... x8) + 16
 *  160  M204 P    planner.acceleration             (float)
 *  164  M204 R    planner.retract_acceleration     (float)
 *  168  M204 T    planner.travel_acceleration      (float)
 *  172  M205 S    planner.min_feedrate_mm_s        (float)
 *  176  M205 T    planner.min_travel_feedrate_mm_s (float)
 *  180  M205 B    planner.min_segment_time         (millis_t)
 *  184  M205 X    planner.max_jerk[X_AXIS]         (float)
 *  188  M205 Y    planner.max_jerk[Y_AXIS]         (float)
 *  192  M205 Z    planner.max_jerk[Z_AXIS]         (float)
 *  196  M205 E    planner.max_jerk[E_AXIS]         (float)
 *  200  M206 XYZ  home_offset                      (float x3)
 *  212  M218 XYZ  hotend_offset                    (float x3 x5)
 *
 * Global Leveling:																																						(4)
 *  272            z_fade_height                    (float)
 *
 * HAS_BED_PROBE:																																							(4)
 *  276  M851      zprobe_zoffset                   (float)
 *
 * ABL_PLANAR:																																								(36)
 *  280            bed_level_matrix									(matrix_3x3 = float x9)
 *
 * MESH_BED_LEVELING / AUTO_BED_LEVELING_BILINEAR / AUTO_BED_LEVELING_UBL:										(52 + 988)
 *  316  M420 S    mbl.status                  			(bool)
 *  		 G29 A     ubl.state.active
 *  317            GRID_MAX_POINTS_X           			(uint8_t)
 *  318            GRID_MAX_POINTS_Y                (uint8_t)
 *  319            bilinear_grid_spacing						(int x2)
 *  323  G29 L F   bilinear_start										(int x2)
 *  327            mbl.z_offset                     (float)
 *  		 G29 Z     ubl.state.z_offset
 *  331  G29 S     ubl.state.eeprom_storage_slot		(int8_t)
 *  332 G29 S3 XYZ mbl.z_values[][]                 (float x9, up to float x81) +288
 *  							 z_values[][]											(float x9, up to float x256) +988
 *
 * DELTA / Z_DUAL_ENDSTOPS:																																		(40)
 *  368  M666 XYZ  endstop_adj                      (float x3)
 *  		 M666 Z    z_endstop_adj
 *  380  M665 R    delta_radius                     (float)
 *  384  M665 L    delta_diagonal_rod               (float)
 *  388  M665 S    delta_segments_per_second        (float)
 *  392  M665 B    delta_calibration_radius         (float)
 *  396  M665 X    delta_tower_angle_trim[A]        (float)
 *  400  M665 Y    delta_tower_angle_trim[B]        (float)
 *  404  M665 Z    delta_tower_angle_trim[C]        (float) is always 0.0
 *
 * ULTIPANEL:																																									(16)
 *  408  M145 S0 H lcd_preheat_hotend_temp          (int x2)
 *  412  M145 S0 B lcd_preheat_bed_temp             (int x2)
 *  416            lcd_preheat_chamber_temp         (int x2)
 *  420  M145 S0 F lcd_preheat_fan_speed            (int x2)
 *
 * DWIN_LCD:																																									(16)
 *	408						 lcd_preheat_hotend_temp					(int x5)
 *	418						 lcd_preheat_bed_temp							(int)
 *	420						 lcd_preheat_chamber_temp					(int)
 *	422						 lcd_preheat_fan_speed						(int)
 *
 * PIDTEMP:																																										(82)
 *  424  M301 E0 PIDC  Kp[0], Ki[0], Kd[0], Kc[0]   (float x4)
 *  440  M301 E1 PIDC  Kp[1], Ki[1], Kd[1], Kc[1]   (float x4)
 *  456  M301 E2 PIDC  Kp[2], Ki[2], Kd[2], Kc[2]   (float x4)
 *  472  M301 E3 PIDC  Kp[3], Ki[3], Kd[3], Kc[3]   (float x4)
 *  488  M301 E4 PIDC  Kp[3], Ki[3], Kd[3], Kc[3]   (float x4)
 *  504  M301 L        lpq_len                      (int)
 *
 * PIDTEMPBED:																																								(12)
 *  506  M304 PID  thermalManager.bedKp, .bedKi, .bedKd (float x3)
 *
 * DOGLCD:																																										(2)
 *  518  M250 C    lcd_contrast                     (int)
 *
 * FWRETRACT:																																									(33)
 *  520  M209 S    autoretract_enabled              (bool)
 *  521  M207 S    retract_length                   (float)
 *  525  M207 F    retract_feedrate_mm_s            (float)
 *  529  M207 Z    retract_zlift                    (float)
 *  533  M208 S    retract_recover_length           (float)
 *  537  M208 F    retract_recover_feedrate_mm_s    (float)
 *  541  M207 W    swap_retract_length              (float)
 *  545  M208 W    swap_retract_recover_length      (float)
 *  549  M208 R    swap_retract_recover_feedrate_mm_s (float)
 *
 * Volumetric Extrusion:																																			(21)
 *  553  M200 D    volumetric_enabled               (bool)
 *  554  M200 T D  filament_size                    (float x5) (T0..3)
 *
 * HAVE_TMC2130:																																							(22)
 *  574  M906 X    Stepper X current                (uint16_t)
 *  576  M906 Y    Stepper Y current                (uint16_t)
 *  578  M906 Z    Stepper Z current                (uint16_t)
 *  580  M906 X2   Stepper X2 current               (uint16_t)
 *  582  M906 Y2   Stepper Y2 current               (uint16_t)
 *  584  M906 Z2   Stepper Z2 current               (uint16_t)
 *  586  M906 E0   Stepper E0 current               (uint16_t)
 *  588  M906 E1   Stepper E1 current               (uint16_t)
 *  590  M906 E2   Stepper E2 current               (uint16_t)
 *  592  M906 E3   Stepper E3 current               (uint16_t)
 *  594  M906 E4   Stepper E4 current               (uint16_t)
 *
 * LIN_ADVANCE:																																								(8)
 *  596  M900 K    extruder_advance_k               (float)
 *  600  M900 WHD  advance_ed_ratio                 (float)
 *
 * HAS_MOTOR_CURRENT_PWM:																																			(12)
 *  604  M907 X    Stepper XY current               (uint32_t)
 *  608  M907 Z    Stepper Z current                (uint32_t)
 *  612  M907 E    Stepper E current                (uint32_t)
 *
 * Fan Setting (By LYN)																																				(4)
 *  616							extruder_auto_fan_speed					(int)
 *  618							air_fan_speed										(int)
 *
 * Registered & Used Time (By LYN)																														(8)
 *  620							regSN														(float)
 *  624							usedTime												(uint32_t)
 *
 * WIFI setting (By LYN)																																			(25)
 *  628							wifiEnable											(bool)
 *  629							hostName												(char x24)
 *
 * Accident Data (By LYN)																																			(283)
 *  653							isAccident											(bool)
 *  654							pauseLeveling										(bool)
 *  655							pausePos												(float x4)
 *  671 						pauseSpeed											(float)
 *  675							pauseByte												(uint32_t)
 *  679							lastPos													(float x4)
 *  695							lastToolsState									(int x8)
 *  711							lastFilename										(char x225)
 *
 *  936                      Minimum end-point
 * 1972 (936 + 48 + 988)     Maximum end-point
 *
 * ========================================================================
 * meshes_begin (between max and min end-point, directly above)
 * -- MESHES --
 * meshes_end
 * -- MAT (Mesh Allocation Table) --                128 bytes (placeholder size)
 * mat_end = E2END (0xFFF)
 *
 */
#include "Marlin.h"
#include "language.h"
#include "endstops.h"
#include "planner.h"
#include "temperature.h"
#include "ultralcd.h"
#include "dwin_lcd.h"
#include "stepper.h"
#include "WiFiSocket.h"

#if ENABLED(INCH_MODE_SUPPORT) || (ENABLED(ULTIPANEL) && ENABLED(TEMPERATURE_UNITS_SUPPORT))
  #include "gcode.h"
#endif

#include "configuration_store.h"

MarlinSettings settings;

#if ENABLED(MESH_BED_LEVELING)
  #include "mesh_bed_leveling.h"
#endif

#if ENABLED(HAVE_TMC2130)
  #include "stepper_indirection.h"
#endif

#if ENABLED(AUTO_BED_LEVELING_UBL)
  #include "ubl.h"
#endif

#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
  extern void refresh_bed_level();
#endif

/**
 * Post-process after Retrieve or Reset
 */
void MarlinSettings::postprocess() {
  // steps per s2 needs to be updated to agree with units per s2
  planner.reset_acceleration_rates();

  // Make sure delta kinematics are updated before refreshing the
  // planner position so the stepper counts will be set correctly.
  #if ENABLED(DELTA)
    recalc_delta_settings(delta_radius, delta_diagonal_rod, delta_tower_angle_trim);
  #endif

  // Refresh steps_to_mm with the reciprocal of axis_steps_per_mm
  // and init stepper.count[], planner.position[] with current_position
  planner.refresh_positioning();

  #if ENABLED(PIDTEMP)
    thermalManager.updatePID();
  #endif

  calculate_volumetric_multipliers();

  #if HAS_HOME_OFFSET || ENABLED(DUAL_X_CARRIAGE)
    // Software endstops depend on home_offset
    LOOP_XYZ(i) update_software_endstops((AxisEnum)i);
  #endif

  #if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
    set_z_fade_height(planner.z_fade_height);
  #endif

  #if HAS_BED_PROBE
    refresh_zprobe_zoffset();
  #endif

  #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
    refresh_bed_level();
    //set_bed_leveling_enabled(leveling_is_on);
  #endif

  #if HAS_MOTOR_CURRENT_PWM
    stepper.refresh_motor_power();
  #endif
}

#if ENABLED(EEPROM_SETTINGS)
  bool MarlinSettings::eeprom_error;

  #if ENABLED(AUTO_BED_LEVELING_UBL)
    int MarlinSettings::meshes_begin;
  #endif
	

  bool MarlinSettings::readCheck(){	// TODO		Check the value is suitable

//#define EEPROM_ASSERT(TST,ERR) if (!(TST)) do{ SERIAL_ERROR_START; SERIAL_ERRORLNPGM(ERR); eeprom_read_error = true; }while(0)
//EEPROM_ASSERT(
//  WITHIN(lcd_preheat_fan_speed, 0, 255),
//  "lcd_preheat_fan_speed out of range"
//);

  	return true;
  }

	#define CONFIG_COPY(destVar, srcVar) memcpy((void*)&(destVar), (void*)&(srcVar), min(sizeof((srcVar)), sizeof((destVar))))
	#define SETTING_TO_CONFIG(var) CONFIG_COPY(config.setting_ ## var, var)
	#define SETTING_TO_CONFIG2(setting, var) CONFIG_COPY(config.setting, var)
	#define CONFIG_TO_SETTING(var) CONFIG_COPY(var, config.setting_ ## var)
	#define CONFIG_TO_SETTING2(var, setting) CONFIG_COPY(var, config.setting)


  /**
   * M500 - Store Configuration
   */
  bool MarlinSettings::save() {


#ifdef DEBUG_FREE
  	SERIAL_ECHOLNPAIR("in save: ", freeMemory());
#endif

  	EEPROM_DATA config;


#ifdef DEBUG_FREE
  	SERIAL_ECHOLNPAIR("into save: ", freeMemory());
#endif

    SETTING_TO_CONFIG(versionFW);

    SETTING_TO_CONFIG2(setting_axis_steps_per_mm, planner.axis_steps_per_mm);
    SETTING_TO_CONFIG2(setting_max_feedrate_mm_s, planner.max_feedrate_mm_s);
    SETTING_TO_CONFIG2(setting_max_acceleration_mm_per_s2, planner.max_acceleration_mm_per_s2);
    SETTING_TO_CONFIG2(setting_acceleration, planner.acceleration);
    SETTING_TO_CONFIG2(setting_retract_acceleration, planner.retract_acceleration);
    SETTING_TO_CONFIG2(setting_travel_acceleration, planner.travel_acceleration);
    SETTING_TO_CONFIG2(setting_min_feedrate_mm_s, planner.min_feedrate_mm_s);
    SETTING_TO_CONFIG2(setting_min_travel_feedrate_mm_s, planner.min_travel_feedrate_mm_s);
    SETTING_TO_CONFIG2(setting_min_segment_time, planner.min_segment_time);
    SETTING_TO_CONFIG2(setting_max_jerk, planner.max_jerk);

		#if HAS_HOME_OFFSET
			SETTING_TO_CONFIG(home_offset);
		#endif

		#if HOTENDS > 1
			SETTING_TO_CONFIG(hotend_offset);
		#endif

    // Global Leveling
    #if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
			SETTING_TO_CONFIG2(setting_z_fade_height, planner.z_fade_height);
    #endif

		// HAS_BED_PROBE
		#if HAS_BED_PROBE
			SETTING_TO_CONFIG(zprobe_zoffset);
		#endif

		// Planar Bed Leveling matrix
		#if ABL_PLANAR
			SETTING_TO_CONFIG2(setting_bed_level_matrix, planner.bed_level_matrix);
		#endif


		#if ENABLED(MESH_BED_LEVELING)	// Mesh Bed Leveling
			const uint8_t grid_max_x = GRID_MAX_POINTS_X,
										grid_max_y = GRID_MAX_POINTS_Y;
      static_assert(
        sizeof(mbl.z_values) == (grid_max_x) * (grid_max_y) * sizeof(mbl.z_values[0][0]),
        "MBL Z array is the wrong size."
      );
      bool status = TEST(mbl.status, MBL_STATUS_HAS_MESH_BIT);
			SETTING_TO_CONFIG2(setting_mesh_bilinear_ubl_status, status);
			SETTING_TO_CONFIG2(setting_mesh_bilinear_max_x, grid_max_x);
			SETTING_TO_CONFIG2(setting_mesh_bilinear_max_y, grid_max_y);
			SETTING_TO_CONFIG2(setting_mesh_ubl_z_offset, mbl.z_offset);
			SETTING_TO_CONFIG2(setting_mesh_bilinear_z_values, mbl.z_values);
		#elif ENABLED(AUTO_BED_LEVELING_BILINEAR)		// Bilinear Auto Bed Leveling
			const uint8_t grid_max_x = GRID_MAX_POINTS_X,
										grid_max_y = GRID_MAX_POINTS_Y;
      static_assert(
        sizeof(z_values) == (grid_max_x) * (grid_max_y) * sizeof(z_values[0][0]),
        "Bilinear Z array is the wrong size."
      );
      SETTING_TO_CONFIG2(setting_mesh_bilinear_ubl_status, planner.abl_enabled);
			SETTING_TO_CONFIG2(setting_mesh_bilinear_max_x, grid_max_x);
			SETTING_TO_CONFIG2(setting_mesh_bilinear_max_y, grid_max_y);
			SETTING_TO_CONFIG(bilinear_grid_spacing);
			SETTING_TO_CONFIG(bilinear_start);
			SETTING_TO_CONFIG2(setting_mesh_bilinear_z_values, z_values);
		#elif ENABLED(AUTO_BED_LEVELING_UBL)				// Unified Bed Leveling
			SETTING_TO_CONFIG2(setting_mesh_bilinear_ubl_status, ubl.state.active);
			SETTING_TO_CONFIG2(setting_mesh_ubl_z_offset, ubl.state.z_offset);
			SETTING_TO_CONFIG2(setting_ubl_eeprom_storage_slot, ubl.state.eeprom_storage_slot);
		#endif

		#if ENABLED(DELTA)
			SETTING_TO_CONFIG(endstop_adj);
			SETTING_TO_CONFIG(delta_radius);
			SETTING_TO_CONFIG(delta_diagonal_rod);
			SETTING_TO_CONFIG(delta_segments_per_second);
			SETTING_TO_CONFIG(delta_calibration_radius);
			SETTING_TO_CONFIG(delta_tower_angle_trim);
		#elif ENABLED(Z_DUAL_ENDSTOPS)
			SETTING_TO_CONFIG2(setting_endstop_adj, z_endstop_adj);
		#endif

		#if ENABLED(ULTIPANEL)
			SETTING_TO_CONFIG(lcd_preheat_hotend_temp);
			SETTING_TO_CONFIG(lcd_preheat_bed_temp);
			SETTING_TO_CONFIG(lcd_preheat_fan_speed);
		#endif

		#if ENABLED(DWIN_LCD)
			SETTING_TO_CONFIG(lcd_preheat_hotend_temp);
			SETTING_TO_CONFIG(lcd_preheat_bed_temp);
			#ifdef HOTWIND_SYSTEM
				SETTING_TO_CONFIG(lcd_preheat_chamber_temp);
			#endif
			SETTING_TO_CONFIG(lcd_preheat_fan_speed);
		#endif

		#if ENABLED(PIDTEMP)
			#if ENABLED(PID_PARAMS_PER_HOTEND) && HOTENDS > 1
				HOTEND_LOOP()
			#else
				uint8_t e = 0;
			#endif
				{
					SETTING_TO_CONFIG2(setting_Kpidc[e][0], PID_PARAM(Kp, e));
					SETTING_TO_CONFIG2(setting_Kpidc[e][1], PID_PARAM(Ki, e));
					SETTING_TO_CONFIG2(setting_Kpidc[e][2], PID_PARAM(Kd, e));
					#if ENABLED(PID_EXTRUSION_SCALING)
						SETTING_TO_CONFIG2(setting_Kpidc[e][3], PID_PARAM(Kc, e));
					#endif
				}
		#endif

		#if ENABLED(PID_EXTRUSION_SCALING)
			SETTING_TO_CONFIG(lpq_len);
		#endif

		#if ENABLED(PIDTEMPBED)
			SETTING_TO_CONFIG2(setting_bedKpid[0], thermalManager.bedKp);
			SETTING_TO_CONFIG2(setting_bedKpid[1], thermalManager.bedKi);
			SETTING_TO_CONFIG2(setting_bedKpid[2], thermalManager.bedKd);
		#endif

		#if HAS_LCD_CONTRAST
			SETTING_TO_CONFIG(lcd_contrast);
		#endif

		#if ENABLED(FWRETRACT)
			SETTING_TO_CONFIG(autoretract_enabled);
			SETTING_TO_CONFIG(retract_length);
			SETTING_TO_CONFIG(retract_feedrate_mm_s);
			SETTING_TO_CONFIG(retract_zlift);
			SETTING_TO_CONFIG(retract_recover_length);
			SETTING_TO_CONFIG(retract_recover_feedrate_mm_s);
			#if EXTRUDERS > 1
				SETTING_TO_CONFIG(swap_retract_length);
				SETTING_TO_CONFIG(swap_retract_recover_length);
				SETTING_TO_CONFIG(swap_retract_recover_feedrate_mm_s);
			#endif
		#endif

		SETTING_TO_CONFIG(volumetric_enabled);
		SETTING_TO_CONFIG(filament_size);

    // Save TMC2130 Configuration, and placeholder values
		#if ENABLED(HAVE_TMC2130)
			#if ENABLED(X_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperX_current,  stepperX.getCurrent());
			#endif
			#if ENABLED(Y_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperY_current,  stepperY.getCurrent());
			#endif
			#if ENABLED(Z_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperZ_current,  stepperZ.getCurrent());
			#endif
			#if ENABLED(X2_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperX2_current, stepperX2.getCurrent());
			#endif
			#if ENABLED(Y2_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperY2_current, stepperY2.getCurrent());
			#endif
			#if ENABLED(Z2_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperZ2_current, stepperZ2.getCurrent());
			#endif
			#if ENABLED(E0_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperE0_current, stepperE0.getCurrent());
			#endif
			#if ENABLED(E1_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperE1_current, stepperE1.getCurrent());
			#endif
			#if ENABLED(E2_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperE2_current, stepperE2.getCurrent());
			#endif
			#if ENABLED(E3_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperE3_current, stepperE3.getCurrent());
			#endif
			#if ENABLED(E4_IS_TMC2130)
				SETTING_TO_CONFIG2(setting_stepperE4_current, stepperE4.getCurrent());
			#endif
		#endif

    // Linear Advance
    #if ENABLED(LIN_ADVANCE)
			SETTING_TO_CONFIG2(setting_extruder_advance_k, planner.extruder_advance_k);
			SETTING_TO_CONFIG2(setting_advance_ed_ratio, planner.advance_ed_ratio);
    #endif

    #if HAS_MOTOR_CURRENT_PWM
      SETTING_TO_CONFIG2(setting_motor_current, stepper.motor_current_setting);
    #endif

		#if HAS_AUTO_FAN
			SETTING_TO_CONFIG(extruder_auto_fan_speed);
		#endif

		#ifdef HAS_AIR_FAN
			SETTING_TO_CONFIG(air_fan_speed);
		#endif

		#ifdef REG_SN
			SETTING_TO_CONFIG(regSN);
		#endif
			SETTING_TO_CONFIG(usedTime);

		#ifdef WIFI_SUPPORT
			SETTING_TO_CONFIG2(setting_wifiEnable, myWifi.enable);
			SETTING_TO_CONFIG(hostName);
		#endif

		#ifdef ACCIDENT_DETECT
			SETTING_TO_CONFIG(isAccident);
			#ifdef HAS_LEVELING
				SETTING_TO_CONFIG(pauseLeveling);
			#endif
			SETTING_TO_CONFIG(pausePos);
			SETTING_TO_CONFIG(pauseSpeed);
			SETTING_TO_CONFIG(pauseByte);
			SETTING_TO_CONFIG(lastPos);
			SETTING_TO_CONFIG(lastToolsState);
			SETTING_TO_CONFIG(lastFilename);
		#endif

		EEPROM_UPDATE_CONFIG(config);

		// Report storage size
		#if ENABLED(EEPROM_CHITCHAT)
			SERIAL_ECHO_START();
			SERIAL_ECHOPAIR("Settings Stored (", (unsigned long)(sizeof(config) + EEPROM_OFFSET));
			SERIAL_ECHOLNPGM(" bytes)");
		#endif

		EEPROM_DATA config_check;
		EEPROM_READ_CONFIG(config_check);

		eeprom_error = !(config == config_check);
//		eeprom_error = false;
		
    #if ENABLED(UBL_SAVE_ACTIVE_ON_M500)
      if (ubl.state.storage_slot >= 0)
        store_mesh(ubl.state.storage_slot);
    #endif


#ifdef DEBUG_FREE
    SERIAL_ECHOLNPAIR("out save: ", freeMemory());
#endif

    return !eeprom_error;
  }

  /**
   * M501 - Retrieve Configuration
   */
  bool MarlinSettings::load() {
		eeprom_error = false;
  	char stored_ver[12];
  	EEPROM_READ_SETTING(setting_versionFW, stored_ver);

  	if (strncmp(versionFW, stored_ver, FW_STR_LEN) != 0) {
      #if ENABLED(EEPROM_CHITCHAT)
	      SERIAL_ECHO_START();
	      SERIAL_ECHOPGM("EEPROM version mismatch ");
	      SERIAL_ECHOPAIR("(EEPROM=", stored_ver);
	      SERIAL_ECHOPAIR(" LATEST=", versionFW);
	      SERIAL_ECHOLNPGM(")");
			#endif
  		reset(true);
  	} else {
  		EEPROM_DATA config;
  		EEPROM_READ_CONFIG(config);

  		CONFIG_TO_SETTING2(planner.axis_steps_per_mm, setting_axis_steps_per_mm);
  		CONFIG_TO_SETTING2(planner.max_feedrate_mm_s, setting_max_feedrate_mm_s);
  		CONFIG_TO_SETTING2(planner.max_acceleration_mm_per_s2, setting_max_acceleration_mm_per_s2);
  		CONFIG_TO_SETTING2(planner.acceleration, setting_acceleration);
  		CONFIG_TO_SETTING2(planner.retract_acceleration, setting_retract_acceleration);
  		CONFIG_TO_SETTING2(planner.travel_acceleration, setting_travel_acceleration);
  		CONFIG_TO_SETTING2(planner.min_feedrate_mm_s, setting_min_feedrate_mm_s);
  		CONFIG_TO_SETTING2(planner.min_travel_feedrate_mm_s, setting_min_travel_feedrate_mm_s);
  		CONFIG_TO_SETTING2(planner.min_segment_time, setting_min_segment_time);
  		CONFIG_TO_SETTING2(planner.max_jerk, setting_max_jerk);

			#if HAS_HOME_OFFSET
				#if ENABLED(DELTA)
					home_offset[X_AXIS] = 0.0;
					home_offset[Y_AXIS] = 0.0;
					home_offset[Z_AXIS] -= DELTA_HEIGHT;
				#else
					CONFIG_TO_SETTING(home_offset);
				#endif
			#endif

			#if HOTENDS > 1
				CONFIG_TO_SETTING(hotend_offset);
			#endif

			// Global Leveling
			#if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
				CONFIG_TO_SETTING2(planner.z_fade_height, setting_z_fade_height);
			#endif

			// HAS_BED_PROBE
			#if HAS_BED_PROBE
				CONFIG_TO_SETTING(zprobe_zoffset);
			#endif

			// Planar Bed Leveling matrix
			#if ABL_PLANAR
				CONFIG_TO_SETTING2(planner.bed_level_matrix, setting_bed_level_matrix);
			#endif

			#if ENABLED(MESH_BED_LEVELING)	// Mesh Bed Leveling
				if((config.setting_mesh_bilinear_max_x == GRID_MAX_POINTS_X) && (config.setting_mesh_bilinear_max_y == GRID_MAX_POINTS_Y)){
					mbl.status = config.setting_mesh_bilinear_ubl_status ? _BV(MBL_STATUS_HAS_MESH_BIT) : 0;
					CONFIG_TO_SETTING2(mbl.z_offset, setting_mesh_ubl_z_offset);
					CONFIG_TO_SETTING2(mbl.z_values, setting_mesh_bilinear_z_values);
				} else {
					mbl.reset();
				}
			#elif ENABLED(AUTO_BED_LEVELING_BILINEAR)		// Bilinear Auto Bed Leveling
				if((config.setting_mesh_bilinear_max_x == GRID_MAX_POINTS_X) && (config.setting_mesh_bilinear_max_y == GRID_MAX_POINTS_Y)){
					CONFIG_TO_SETTING(bilinear_grid_spacing);
					CONFIG_TO_SETTING(bilinear_start);
					CONFIG_TO_SETTING2(z_values, setting_mesh_bilinear_z_values);
					set_bed_leveling_enabled(config.setting_mesh_bilinear_ubl_status);
				}
			#elif ENABLED(AUTO_BED_LEVELING_UBL)				// Unified Bed Leveling
				CONFIG_TO_SETTING2(ubl.state.active, setting_mesh_bilinear_ubl_status);
				CONFIG_TO_SETTING2(ubl.state.z_offset, setting_mesh_ubl_z_offset);
				CONFIG_TO_SETTING2(ubl.state.eeprom_storage_slot, setting_ubl_eeprom_storage_slot);
			#endif

			#if ENABLED(DELTA)
				CONFIG_TO_SETTING(endstop_adj);
				CONFIG_TO_SETTING(delta_radius);
				CONFIG_TO_SETTING(delta_diagonal_rod);
				CONFIG_TO_SETTING(delta_segments_per_second);
				CONFIG_TO_SETTING(delta_calibration_radius);
				CONFIG_TO_SETTING(delta_tower_angle_trim);
			#elif ENABLED(Z_DUAL_ENDSTOPS)
				CONFIG_TO_SETTING2(z_endstop_adj, setting_endstop_adj);
			#endif

			#if ENABLED(ULTIPANEL)
				CONFIG_TO_SETTING(lcd_preheat_hotend_temp);
				CONFIG_TO_SETTING(lcd_preheat_bed_temp);
				CONFIG_TO_SETTING(lcd_preheat_fan_speed);
			#endif

			#if ENABLED(DWIN_LCD)
				CONFIG_TO_SETTING(lcd_preheat_hotend_temp);
				CONFIG_TO_SETTING(lcd_preheat_bed_temp);
				#ifdef HOTWIND_SYSTEM
					CONFIG_TO_SETTING(lcd_preheat_chamber_temp);
				#endif
				CONFIG_TO_SETTING(lcd_preheat_fan_speed);
			#endif

			#if ENABLED(PIDTEMP)
				#if ENABLED(PID_PARAMS_PER_HOTEND) && HOTENDS > 1
					HOTEND_LOOP()
				#else
					uint8_t e = 0;
				#endif
					{
						CONFIG_TO_SETTING2(PID_PARAM(Kp, e), setting_Kpidc[e][0]);
						CONFIG_TO_SETTING2(PID_PARAM(Ki, e), setting_Kpidc[e][1]);
						CONFIG_TO_SETTING2(PID_PARAM(Kd, e), setting_Kpidc[e][2]);
						#if ENABLED(PID_EXTRUSION_SCALING)
							CONFIG_TO_SETTING2(PID_PARAM(Kc, e), setting_Kpidc[e][3]);
						#endif
					}
			#endif

			#if ENABLED(PID_EXTRUSION_SCALING)
				CONFIG_TO_SETTING(lpq_len);
			#endif

			#if ENABLED(PIDTEMPBED)
				CONFIG_TO_SETTING2(thermalManager.bedKp, setting_bedKpid[0]);
				CONFIG_TO_SETTING2(thermalManager.bedKi, setting_bedKpid[1]);
				CONFIG_TO_SETTING2(thermalManager.bedKd, setting_bedKpid[2]);
			#endif

			#if HAS_LCD_CONTRAST
				CONFIG_TO_SETTING(lcd_contrast);
			#endif

		#if ENABLED(FWRETRACT)
			CONFIG_TO_SETTING(autoretract_enabled);
			CONFIG_TO_SETTING(retract_length);
			CONFIG_TO_SETTING(retract_feedrate_mm_s);
			CONFIG_TO_SETTING(retract_zlift);
			CONFIG_TO_SETTING(retract_recover_length);
			CONFIG_TO_SETTING(retract_recover_feedrate_mm_s);
			#if EXTRUDERS > 1
				CONFIG_TO_SETTING(swap_retract_length);
				CONFIG_TO_SETTING(swap_retract_recover_length);
				CONFIG_TO_SETTING(swap_retract_recover_feedrate_mm_s);
			#endif
		#endif

			CONFIG_TO_SETTING(volumetric_enabled);
			CONFIG_TO_SETTING(filament_size);

			#if ENABLED(HAVE_TMC2130)
				#if ENABLED(X_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperX.getCurrent(),  setting_stepperX_current);
				#endif
				#if ENABLED(Y_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperY.getCurrent(),  setting_stepperY_current);
				#endif
				#if ENABLED(Z_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperZ.getCurrent(),  setting_stepperZ_current);
				#endif
				#if ENABLED(X2_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperX2.getCurrent(), setting_stepperX2_current);
				#endif
				#if ENABLED(Y2_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperY2.getCurrent(), setting_stepperY2_current);
				#endif
				#if ENABLED(Z2_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperZ2.getCurrent(), setting_stepperZ2_current);
				#endif
				#if ENABLED(E0_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperE0.getCurrent(), setting_stepperE0_current);
				#endif
				#if ENABLED(E1_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperE1.getCurrent(), setting_stepperE1_current);
				#endif
				#if ENABLED(E2_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperE2.getCurrent(), setting_stepperE2_current);
				#endif
				#if ENABLED(E3_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperE3.getCurrent(), setting_stepperE3_current);
				#endif
				#if ENABLED(E4_IS_TMC2130)
					CONFIG_TO_SETTING2(stepperE4.getCurrent(), setting_stepperE4_current);
				#endif
			#endif

			// Linear Advance
			#if ENABLED(LIN_ADVANCE)
				CONFIG_TO_SETTING2(planner.extruder_advance_k, setting_extruder_advance_k);
				CONFIG_TO_SETTING2(planner.advance_ed_ratio, setting_advance_ed_ratio);
			#endif

      #if HAS_MOTOR_CURRENT_PWM
        CONFIG_TO_SETTING2(stepper.motor_current_setting, setting_motor_current);
      #endif

			#if HAS_AUTO_FAN
				CONFIG_TO_SETTING(extruder_auto_fan_speed);
			#endif

			#ifdef HAS_AIR_FAN
				CONFIG_TO_SETTING(air_fan_speed);
			#endif

			#ifdef REG_SN
				CONFIG_TO_SETTING(regSN);
			#endif
				CONFIG_TO_SETTING(usedTime);

			#ifdef WIFI_SUPPORT
				CONFIG_TO_SETTING2(myWifi.enable, setting_wifiEnable);
				CONFIG_TO_SETTING(hostName);
			#endif

			#ifdef ACCIDENT_DETECT
				CONFIG_TO_SETTING(isAccident);
				if(isAccident){
					#ifdef HAS_LEVELING
						CONFIG_TO_SETTING(pauseLeveling);
					#endif
					CONFIG_TO_SETTING(pausePos);
					CONFIG_TO_SETTING(pauseSpeed);
					CONFIG_TO_SETTING(pauseByte);
					CONFIG_TO_SETTING(lastPos);
					CONFIG_TO_SETTING(lastToolsState);
					CONFIG_TO_SETTING(lastFilename);
				} else {
					#ifdef HAS_LEVELING
						pauseLeveling = false;
					#endif
					ZERO(pausePos);
					pauseSpeed = 0.0;
					pauseByte = 0;
					ZERO(lastPos);
					ZERO(lastToolsState);
					ZERO(lastFilename);
				}
			#endif

			eeprom_error = !readCheck();

			if(eeprom_error){
				reset(true);
			} else {
        postprocess();
				#if ENABLED(EEPROM_CHITCHAT)
	    		SERIAL_ECHO_START();
	    		SERIAL_ECHO(versionFW);
	    		SERIAL_ECHOPAIR(" stored settings retrieved (", (unsigned long)(sizeof(config) + EEPROM_OFFSET));
	    		SERIAL_ECHOLNPGM(" bytes)");
				#endif
			}

      #if ENABLED(AUTO_BED_LEVELING_UBL)
				int16_t eeprom_index = sizeof(config) + EEPROM_OFFSET;
        meshes_begin = (eeprom_index + 32) & 0xFFF8;  // Pad the end of configuration data so it
                                                      // can float up or down a little bit without
                                                      // disrupting the mesh data
        ubl.report_state();

        if (!ubl.sanity_check()) {
          SERIAL_EOL();
          #if ENABLED(EEPROM_CHITCHAT)
            ubl.echo_name();
            SERIAL_ECHOLNPGM(" initialized.\n");
          #endif
        }
        else {
          #if ENABLED(EEPROM_CHITCHAT)
            SERIAL_PROTOCOLPGM("?Can't enable ");
            ubl.echo_name();
            SERIAL_PROTOCOLLNPGM(".");
          #endif
          ubl.reset();
        }

        if (ubl.state.storage_slot >= 0) {
          load_mesh(ubl.state.storage_slot);
          #if ENABLED(EEPROM_CHITCHAT)
            SERIAL_ECHOPAIR("Mesh ", ubl.state.storage_slot);
            SERIAL_ECHOLNPGM(" loaded from storage.");
          #endif
        }
        else {
          ubl.reset();
          #if ENABLED(EEPROM_CHITCHAT)
            SERIAL_ECHOLNPGM("UBL System reset()");
          #endif
        }
      #endif
    }

    #if ENABLED(EEPROM_CHITCHAT) && DISABLED(DISABLE_M503)
      report();
    #endif

    return !eeprom_error;
  }

  #if ENABLED(AUTO_BED_LEVELING_UBL)

    #if ENABLED(EEPROM_CHITCHAT)
      void ubl_invalid_slot(const int s) {
        SERIAL_PROTOCOLLNPGM("?Invalid slot.");
        SERIAL_PROTOCOL(s);
        SERIAL_PROTOCOLLNPGM(" mesh slots available.");
      }
    #endif

    int MarlinSettings::calc_num_meshes() {
      //obviously this will get more sophisticated once we've added an actual MAT

      if (meshes_begin <= 0) return 0;

      return (meshes_end - meshes_begin) / sizeof(ubl.z_values);
    }

    void MarlinSettings::store_mesh(int8_t slot) {

      #if ENABLED(AUTO_BED_LEVELING_UBL)
        const int a = calc_num_meshes();
        if (!WITHIN(slot, 0, a - 1)) {
          #if ENABLED(EEPROM_CHITCHAT)
            ubl_invalid_slot(a);
            SERIAL_PROTOCOLPAIR("E2END=", E2END);
            SERIAL_PROTOCOLPAIR(" meshes_end=", meshes_end);
            SERIAL_PROTOCOLLNPAIR(" slot=", slot);
            SERIAL_EOL();
          #endif
          return;
        }

        uint16_t crc = 0;
        int pos = meshes_end - (slot + 1) * sizeof(ubl.z_values);

        write_data(pos, (uint8_t *)&ubl.z_values, sizeof(ubl.z_values), &crc);

        // Write crc to MAT along with other data, or just tack on to the beginning or end

        #if ENABLED(EEPROM_CHITCHAT)
          SERIAL_PROTOCOLLNPAIR("Mesh saved in slot ", slot);
        #endif

      #else

        // Other mesh types

      #endif
    }

    void MarlinSettings::load_mesh(int8_t slot, void *into /* = 0 */) {

      #if ENABLED(AUTO_BED_LEVELING_UBL)

        const int16_t a = settings.calc_num_meshes();

        if (!WITHIN(slot, 0, a - 1)) {
          #if ENABLED(EEPROM_CHITCHAT)
            ubl_invalid_slot(a);
          #endif
          return;
        }

        uint16_t crc = 0;
        int pos = meshes_end - (slot + 1) * sizeof(ubl.z_values);
        uint8_t * const dest = into ? (uint8_t*)into : (uint8_t*)&ubl.z_values;
        read_data(pos, dest, sizeof(ubl.z_values), &crc);

        // Compare crc with crc from MAT, or read from end

        #if ENABLED(EEPROM_CHITCHAT)
          SERIAL_PROTOCOLLNPAIR("Mesh loaded from slot ", slot);
        #endif

      #else

        // Other mesh types

      #endif
    }

    //void MarlinSettings::delete_mesh() { return; }
    //void MarlinSettings::defrag_meshes() { return; }

  #endif // AUTO_BED_LEVELING_UBL

#else // !EEPROM_SETTINGS

  bool MarlinSettings::save() {
    SERIAL_ERROR_START();
    SERIAL_ERRORLNPGM("EEPROM disabled");
    return false;
  }

#endif // !EEPROM_SETTINGS

/**
 * M502 - Reset Configuration
 */
void MarlinSettings::reset(bool init/* = false*/) {
  static const float tmp1[] PROGMEM = DEFAULT_AXIS_STEPS_PER_UNIT, tmp2[] PROGMEM = DEFAULT_MAX_FEEDRATE;
  static const uint32_t tmp3[] PROGMEM = DEFAULT_MAX_ACCELERATION;
  LOOP_XYZE_N(i) {
    planner.axis_steps_per_mm[i]          = pgm_read_float(&tmp1[i < COUNT(tmp1) ? i : COUNT(tmp1) - 1]);
    planner.max_feedrate_mm_s[i]          = pgm_read_float(&tmp2[i < COUNT(tmp2) ? i : COUNT(tmp2) - 1]);
    planner.max_acceleration_mm_per_s2[i] = pgm_read_dword_near(&tmp3[i < COUNT(tmp3) ? i : COUNT(tmp3) - 1]);
  }

  planner.acceleration = DEFAULT_ACCELERATION;
  planner.retract_acceleration = DEFAULT_RETRACT_ACCELERATION;
  planner.travel_acceleration = DEFAULT_TRAVEL_ACCELERATION;
  planner.min_feedrate_mm_s = DEFAULT_MINIMUMFEEDRATE;
  planner.min_segment_time = DEFAULT_MINSEGMENTTIME;
  planner.min_travel_feedrate_mm_s = DEFAULT_MINTRAVELFEEDRATE;
  planner.max_jerk[X_AXIS] = DEFAULT_XJERK;
  planner.max_jerk[Y_AXIS] = DEFAULT_YJERK;
  planner.max_jerk[Z_AXIS] = DEFAULT_ZJERK;
  planner.max_jerk[E_AXIS] = DEFAULT_EJERK;

  #if HAS_HOME_OFFSET
    ZERO(home_offset);
  #endif

  #if HOTENDS > 1
    constexpr float tmp4[XYZ][HOTENDS] = {
      HOTEND_OFFSET_X,
      HOTEND_OFFSET_Y
      #ifdef HOTEND_OFFSET_Z
        , HOTEND_OFFSET_Z
      #else
        , { 0 }
      #endif
    };
    static_assert(
      tmp4[X_AXIS][0] == 0 && tmp4[Y_AXIS][0] == 0 && tmp4[Z_AXIS][0] == 0,
      "Offsets for the first hotend must be 0.0."
    );
    LOOP_XYZ(i) HOTEND_LOOP() hotend_offset[i][e] = tmp4[i][e];
  #endif

	#if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
//		planner.z_fade_height = 0.0;											// TODO (default off?)
		planner.z_fade_height = 2.0;
	#endif

  // Applies to all MBL and ABL
  #if HAS_LEVELING
    reset_bed_level();
  #endif

  #if HAS_BED_PROBE
    if(init){
    	zprobe_zoffset = Z_PROBE_OFFSET_FROM_EXTRUDER;
    }
  #endif

  #if ENABLED(DELTA)
    const float adj[ABC] = DELTA_ENDSTOP_ADJ,
                dta[ABC] = DELTA_TOWER_ANGLE_TRIM;
    COPY(endstop_adj, adj);
    delta_radius = DELTA_RADIUS;
    delta_diagonal_rod = DELTA_DIAGONAL_ROD;
    delta_segments_per_second = DELTA_SEGMENTS_PER_SECOND;
    delta_calibration_radius = DELTA_CALIBRATION_RADIUS;
    COPY(delta_tower_angle_trim, dta);
    home_offset[Z_AXIS] = 0;

  #elif ENABLED(Z_DUAL_ENDSTOPS)

    z_endstop_adj =
      #ifdef Z_DUAL_ENDSTOPS_ADJUSTMENT
        Z_DUAL_ENDSTOPS_ADJUSTMENT
      #else
        0
      #endif
    ;

  #endif

  #if ENABLED(ULTIPANEL)
    lcd_preheat_hotend_temp[0] = PREHEAT_1_TEMP_HOTEND;
    lcd_preheat_hotend_temp[1] = PREHEAT_2_TEMP_HOTEND;
    lcd_preheat_bed_temp[0] = PREHEAT_1_TEMP_BED;
    lcd_preheat_bed_temp[1] = PREHEAT_2_TEMP_BED;
    lcd_preheat_fan_speed[0] = PREHEAT_1_FAN_SPEED;
    lcd_preheat_fan_speed[1] = PREHEAT_2_FAN_SPEED;
  #endif

	#if ENABLED(DWIN_LCD)
    HOTEND_LOOP()	lcd_preheat_hotend_temp[e] = PREHEAT_TEMP_HOTEND;
    lcd_preheat_bed_temp = PREHEAT_TEMP_BED;
		#ifdef HOTWIND_SYSTEM
    	lcd_preheat_chamber_temp = PREHEAT_TEMP_CHAMBER;
		#endif
    lcd_preheat_fan_speed = PREHEAT_FAN_SPEED;
	#endif

  #if HAS_LCD_CONTRAST
    lcd_contrast = DEFAULT_LCD_CONTRAST;
  #endif

  #if ENABLED(PIDTEMP)
    #if ENABLED(PID_PARAMS_PER_HOTEND) && HOTENDS > 1
      HOTEND_LOOP()
    #endif
    {
      PID_PARAM(Kp, e) = DEFAULT_Kp;
      PID_PARAM(Ki, e) = scalePID_i(DEFAULT_Ki);
      PID_PARAM(Kd, e) = scalePID_d(DEFAULT_Kd);
      #if ENABLED(PID_EXTRUSION_SCALING)
        PID_PARAM(Kc, e) = DEFAULT_Kc;
      #endif
    }
    #if ENABLED(PID_EXTRUSION_SCALING)
      lpq_len = 20; // default last-position-queue size
    #endif
  #endif // PIDTEMP

  #if ENABLED(PIDTEMPBED)
    thermalManager.bedKp = DEFAULT_bedKp;
    thermalManager.bedKi = scalePID_i(DEFAULT_bedKi);
    thermalManager.bedKd = scalePID_d(DEFAULT_bedKd);
  #endif

  #if ENABLED(FWRETRACT)
    autoretract_enabled = false;
    retract_length = RETRACT_LENGTH;
    retract_feedrate_mm_s = RETRACT_FEEDRATE;
    retract_zlift = RETRACT_ZLIFT;
    retract_recover_length = RETRACT_RECOVER_LENGTH;
    retract_recover_feedrate_mm_s = RETRACT_RECOVER_FEEDRATE;
    #if EXTRUDERS > 1
    swap_retract_length = RETRACT_LENGTH_SWAP;
    swap_retract_recover_length = RETRACT_RECOVER_LENGTH_SWAP;
    swap_retract_recover_feedrate_mm_s = RETRACT_RECOVER_FEEDRATE_SWAP;
    #endif
  #endif // FWRETRACT

  volumetric_enabled =
    #if ENABLED(VOLUMETRIC_DEFAULT_ON)
      true
    #else
      false
    #endif
  ;
  for (uint8_t q = 0; q < COUNT(filament_size); q++)
    filament_size[q] = DEFAULT_NOMINAL_FILAMENT_DIA;

  endstops.enable_globally(
    #if ENABLED(ENDSTOPS_ALWAYS_ON_DEFAULT)
      true
    #else
      false
    #endif
  );

  #if ENABLED(HAVE_TMC2130)
    #if ENABLED(X_IS_TMC2130)
      stepperX.setCurrent(X_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
    #if ENABLED(Y_IS_TMC2130)
      stepperY.setCurrent(Y_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
    #if ENABLED(Z_IS_TMC2130)
      stepperZ.setCurrent(Z_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
    #if ENABLED(X2_IS_TMC2130)
      stepperX2.setCurrent(X2_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
    #if ENABLED(Y2_IS_TMC2130)
      stepperY2.setCurrent(Y2_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
    #if ENABLED(Z2_IS_TMC2130)
      stepperZ2.setCurrent(Z2_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
    #if ENABLED(E0_IS_TMC2130)
      stepperE0.setCurrent(E0_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
    #if ENABLED(E1_IS_TMC2130)
      stepperE1.setCurrent(E1_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
    #if ENABLED(E2_IS_TMC2130)
      stepperE2.setCurrent(E2_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
    #if ENABLED(E3_IS_TMC2130)
      stepperE3.setCurrent(E3_CURRENT, R_SENSE, HOLD_MULTIPLIER);
    #endif
  #endif

  #if ENABLED(LIN_ADVANCE)
    planner.extruder_advance_k = LIN_ADVANCE_K;
    planner.advance_ed_ratio = LIN_ADVANCE_E_D_RATIO;
  #endif

  #if HAS_MOTOR_CURRENT_PWM
    uint32_t tmp_motor_current_setting[3] = PWM_MOTOR_CURRENT;
    for (uint8_t q = 3; q--;)
      stepper.digipot_current(q, (stepper.motor_current_setting[q] = tmp_motor_current_setting[q]));
  #endif

	#if HAS_AUTO_FAN
		extruder_auto_fan_speed = DEFAULT_AUTO_FAN_SPEED;
	#endif

	#ifdef HAS_AIR_FAN
		if(init){
			air_fan_speed = DEFAULT_AIR_FAN_SPEED;
		}
	#endif

	#ifdef ACCIDENT_DETECT
		isAccident = false;
	#endif

  #if ENABLED(AUTO_BED_LEVELING_UBL)
    ubl.reset();
  #endif

  postprocess();

  #if ENABLED(EEPROM_CHITCHAT)
    SERIAL_ECHO_START();
    SERIAL_ECHOLNPGM("Hardcoded Default Settings Loaded");
  #endif

  save();
}

#if DISABLED(DISABLE_M503)

  #define CONFIG_ECHO_START do{ if (!forReplay) SERIAL_ECHO_START(); }while(0)

  /**
   * M503 - Report current settings in RAM
   *
   * Unless specifically disabled, M503 is available even without EEPROM
   */
  void MarlinSettings::report(bool forReplay) {

    /**
     * Announce current units, in case inches are being displayed
     */
    CONFIG_ECHO_START;
    #if ENABLED(INCH_MODE_SUPPORT)
      #define LINEAR_UNIT(N) ((N) / parser.linear_unit_factor)
      #define VOLUMETRIC_UNIT(N) ((N) / (volumetric_enabled ? parser.volumetric_unit_factor : parser.linear_unit_factor))
      SERIAL_ECHOPGM("  G2");
      SERIAL_CHAR(parser.linear_unit_factor == 1.0 ? '1' : '0');
      SERIAL_ECHOPGM(" ; Units in ");
      serialprintPGM(parser.linear_unit_factor == 1.0 ? PSTR("mm\n") : PSTR("inches\n"));
    #else
      #define LINEAR_UNIT(N) N
      #define VOLUMETRIC_UNIT(N) N
      SERIAL_ECHOLNPGM("  G21    ; Units in mm");
    #endif

    #if ENABLED(ULTIPANEL)

      // Temperature units - for Ultipanel temperature options

      CONFIG_ECHO_START;
      #if ENABLED(TEMPERATURE_UNITS_SUPPORT)
        #define TEMP_UNIT(N) parser.to_temp_units(N)
        SERIAL_ECHOPGM("  M149 ");
        SERIAL_CHAR(parser.temp_units_code());
        SERIAL_ECHOPGM(" ; Units in ");
        serialprintPGM(parser.temp_units_name());
      #else
        #define TEMP_UNIT(N) N
        SERIAL_ECHOLNPGM("  M149 C ; Units in Celsius");
      #endif

    #endif

    SERIAL_EOL();

    /**
     * Volumetric extrusion M200
     */
    if (!forReplay) {
      CONFIG_ECHO_START;
      SERIAL_ECHOPGM("Filament settings:");
      if (volumetric_enabled)
        SERIAL_EOL();
      else
        SERIAL_ECHOLNPGM(" Disabled");
    }

    CONFIG_ECHO_START;
    SERIAL_ECHOPAIR("  M200 D", filament_size[0]);
    SERIAL_EOL();
    #if EXTRUDERS > 1
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M200 T1 D", filament_size[1]);
      SERIAL_EOL();
      #if EXTRUDERS > 2
        CONFIG_ECHO_START;
        SERIAL_ECHOPAIR("  M200 T2 D", filament_size[2]);
        SERIAL_EOL();
        #if EXTRUDERS > 3
          CONFIG_ECHO_START;
          SERIAL_ECHOPAIR("  M200 T3 D", filament_size[3]);
          SERIAL_EOL();
          #if EXTRUDERS > 4
            CONFIG_ECHO_START;
            SERIAL_ECHOPAIR("  M200 T4 D", filament_size[4]);
            SERIAL_EOL();
          #endif // EXTRUDERS > 4
        #endif // EXTRUDERS > 3
      #endif // EXTRUDERS > 2
    #endif // EXTRUDERS > 1

    if (!volumetric_enabled) {
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPGM("  M200 D0");
    }

    if (!forReplay) {
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPGM("Steps per unit:");
    }
    CONFIG_ECHO_START;
    SERIAL_ECHOPAIR("  M92 X", LINEAR_UNIT(planner.axis_steps_per_mm[X_AXIS]));
    SERIAL_ECHOPAIR(" Y", LINEAR_UNIT(planner.axis_steps_per_mm[Y_AXIS]));
    SERIAL_ECHOPAIR(" Z", LINEAR_UNIT(planner.axis_steps_per_mm[Z_AXIS]));
    #if DISABLED(DISTINCT_E_FACTORS)
      SERIAL_ECHOPAIR(" E", VOLUMETRIC_UNIT(planner.axis_steps_per_mm[E_AXIS]));
    #endif
    SERIAL_EOL();
    #if ENABLED(DISTINCT_E_FACTORS)
      CONFIG_ECHO_START;
      for (uint8_t i = 0; i < E_STEPPERS; i++) {
        SERIAL_ECHOPAIR("  M92 T", (int)i);
        SERIAL_ECHOLNPAIR(" E", VOLUMETRIC_UNIT(planner.axis_steps_per_mm[E_AXIS + i]));
      }
    #endif

    if (!forReplay) {
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPGM("Maximum feedrates (units/s):");
    }
    CONFIG_ECHO_START;
    SERIAL_ECHOPAIR("  M203 X", LINEAR_UNIT(planner.max_feedrate_mm_s[X_AXIS]));
    SERIAL_ECHOPAIR(" Y", LINEAR_UNIT(planner.max_feedrate_mm_s[Y_AXIS]));
    SERIAL_ECHOPAIR(" Z", LINEAR_UNIT(planner.max_feedrate_mm_s[Z_AXIS]));
    #if DISABLED(DISTINCT_E_FACTORS)
      SERIAL_ECHOPAIR(" E", VOLUMETRIC_UNIT(planner.max_feedrate_mm_s[E_AXIS]));
    #endif
    SERIAL_EOL();
    #if ENABLED(DISTINCT_E_FACTORS)
      CONFIG_ECHO_START;
      for (uint8_t i = 0; i < E_STEPPERS; i++) {
        SERIAL_ECHOPAIR("  M203 T", (int)i);
        SERIAL_ECHOLNPAIR(" E", VOLUMETRIC_UNIT(planner.max_feedrate_mm_s[E_AXIS + i]));
      }
    #endif

    if (!forReplay) {
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPGM("Maximum Acceleration (units/s2):");
    }
    CONFIG_ECHO_START;
    SERIAL_ECHOPAIR("  M201 X", LINEAR_UNIT(planner.max_acceleration_mm_per_s2[X_AXIS]));
    SERIAL_ECHOPAIR(" Y", LINEAR_UNIT(planner.max_acceleration_mm_per_s2[Y_AXIS]));
    SERIAL_ECHOPAIR(" Z", LINEAR_UNIT(planner.max_acceleration_mm_per_s2[Z_AXIS]));
    #if DISABLED(DISTINCT_E_FACTORS)
      SERIAL_ECHOPAIR(" E", VOLUMETRIC_UNIT(planner.max_acceleration_mm_per_s2[E_AXIS]));
    #endif
    SERIAL_EOL();
    #if ENABLED(DISTINCT_E_FACTORS)
      CONFIG_ECHO_START;
      for (uint8_t i = 0; i < E_STEPPERS; i++) {
        SERIAL_ECHOPAIR("  M201 T", (int)i);
        SERIAL_ECHOLNPAIR(" E", VOLUMETRIC_UNIT(planner.max_acceleration_mm_per_s2[E_AXIS + i]));
      }
    #endif

    if (!forReplay) {
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPGM("Acceleration (units/s2): P<print_accel> R<retract_accel> T<travel_accel>");
    }
    CONFIG_ECHO_START;
    SERIAL_ECHOPAIR("  M204 P", LINEAR_UNIT(planner.acceleration));
    SERIAL_ECHOPAIR(" R", LINEAR_UNIT(planner.retract_acceleration));
    SERIAL_ECHOLNPAIR(" T", LINEAR_UNIT(planner.travel_acceleration));

    if (!forReplay) {
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPGM("Advanced: S<min_feedrate> T<min_travel_feedrate> B<min_segment_time_ms> X<max_xy_jerk> Z<max_z_jerk> E<max_e_jerk>");
    }
    CONFIG_ECHO_START;
    SERIAL_ECHOPAIR("  M205 S", LINEAR_UNIT(planner.min_feedrate_mm_s));
    SERIAL_ECHOPAIR(" T", LINEAR_UNIT(planner.min_travel_feedrate_mm_s));
    SERIAL_ECHOPAIR(" B", planner.min_segment_time);
    SERIAL_ECHOPAIR(" X", LINEAR_UNIT(planner.max_jerk[X_AXIS]));
    SERIAL_ECHOPAIR(" Y", LINEAR_UNIT(planner.max_jerk[Y_AXIS]));
    SERIAL_ECHOPAIR(" Z", LINEAR_UNIT(planner.max_jerk[Z_AXIS]));
    SERIAL_ECHOLNPAIR(" E", LINEAR_UNIT(planner.max_jerk[E_AXIS]));

    #if HAS_M206_COMMAND
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Home offset:");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M206 X", LINEAR_UNIT(home_offset[X_AXIS]));
      SERIAL_ECHOPAIR(" Y", LINEAR_UNIT(home_offset[Y_AXIS]));
      SERIAL_ECHOLNPAIR(" Z", LINEAR_UNIT(home_offset[Z_AXIS]));
    #endif

    #if HOTENDS > 1
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Hotend offsets:");
      }
      CONFIG_ECHO_START;
      for (uint8_t e = 1; e < HOTENDS; e++) {
        SERIAL_ECHOPAIR("  M218 T", (int)e);
        SERIAL_ECHOPAIR(" X", LINEAR_UNIT(hotend_offset[X_AXIS][e]));
        SERIAL_ECHOPAIR(" Y", LINEAR_UNIT(hotend_offset[Y_AXIS][e]));
        #if ENABLED(DUAL_X_CARRIAGE) || ENABLED(SWITCHING_NOZZLE) ||ENABLED(PARKING_EXTRUDER)
          SERIAL_ECHOPAIR(" Z", LINEAR_UNIT(hotend_offset[Z_AXIS][e]));
        #endif
        SERIAL_EOL();
      }
    #endif

    #if ENABLED(MESH_BED_LEVELING)

      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Mesh Bed Leveling:");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M420 S", leveling_is_valid() ? 1 : 0);
      #if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
        SERIAL_ECHOPAIR(" Z", LINEAR_UNIT(planner.z_fade_height));
      #endif
      SERIAL_EOL();
      for (uint8_t py = 0; py < GRID_MAX_POINTS_Y; py++) {
        for (uint8_t px = 0; px < GRID_MAX_POINTS_X; px++) {
          CONFIG_ECHO_START;
          SERIAL_ECHOPAIR("  G29 S3 X", (int)px + 1);
          SERIAL_ECHOPAIR(" Y", (int)py + 1);
          SERIAL_ECHOPGM(" Z");
          SERIAL_PROTOCOL_F(LINEAR_UNIT(mbl.z_values[px][py]), 5);
          SERIAL_EOL();
        }
      }

    #elif ENABLED(AUTO_BED_LEVELING_UBL)

      if (!forReplay) {
        CONFIG_ECHO_START;
        ubl.echo_name();
        SERIAL_ECHOLNPGM(":");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M420 S", leveling_is_active() ? 1 : 0);
      #if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
        SERIAL_ECHOPAIR(" Z", planner.z_fade_height);
      #endif
      SERIAL_EOL();

      if (!forReplay) {
        SERIAL_EOL();
        ubl.report_state();

        SERIAL_ECHOLNPAIR("\nActive Mesh Slot: ", ubl.state.storage_slot);

        SERIAL_ECHOPGM("z_offset: ");
        SERIAL_ECHO_F(ubl.state.z_offset, 6);
        SERIAL_EOL();

        SERIAL_ECHOPAIR("EEPROM can hold ", calc_num_meshes());
        SERIAL_ECHOLNPGM(" meshes.\n");
      }

    #elif HAS_ABL

      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Auto Bed Leveling:");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M420 S", leveling_is_active() ? 1 : 0);
      #if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
        SERIAL_ECHOPAIR(" Z", LINEAR_UNIT(planner.z_fade_height));
      #endif
      SERIAL_EOL();

    #endif

    #if ENABLED(DELTA)
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Endstop adjustment:");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M666 X", LINEAR_UNIT(endstop_adj[X_AXIS]));
      SERIAL_ECHOPAIR(" Y", LINEAR_UNIT(endstop_adj[Y_AXIS]));
      SERIAL_ECHOLNPAIR(" Z", LINEAR_UNIT(endstop_adj[Z_AXIS]));
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Delta settings: L<diagonal_rod> R<radius> H<height> S<segments_per_s> B<calibration radius> XYZ<tower angle corrections>");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M665 L", LINEAR_UNIT(delta_diagonal_rod));
      SERIAL_ECHOPAIR(" R", LINEAR_UNIT(delta_radius));
      SERIAL_ECHOPAIR(" H", LINEAR_UNIT(DELTA_HEIGHT + home_offset[Z_AXIS]));
      SERIAL_ECHOPAIR(" S", delta_segments_per_second);
      SERIAL_ECHOPAIR(" B", LINEAR_UNIT(delta_calibration_radius));
      SERIAL_ECHOPAIR(" X", LINEAR_UNIT(delta_tower_angle_trim[A_AXIS]));
      SERIAL_ECHOPAIR(" Y", LINEAR_UNIT(delta_tower_angle_trim[B_AXIS]));
      SERIAL_ECHOPAIR(" Z", LINEAR_UNIT(delta_tower_angle_trim[C_AXIS]));
      SERIAL_EOL();
    #elif ENABLED(Z_DUAL_ENDSTOPS)
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Z2 Endstop adjustment:");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPAIR("  M666 Z", LINEAR_UNIT(z_endstop_adj));
    #endif // DELTA

    #if ENABLED(ULTIPANEL)
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Material heatup parameters:");
      }
      CONFIG_ECHO_START;
      for (uint8_t i = 0; i < COUNT(lcd_preheat_hotend_temp); i++) {
        SERIAL_ECHOPAIR("  M145 S", (int)i);
        SERIAL_ECHOPAIR(" H", TEMP_UNIT(lcd_preheat_hotend_temp[i]));
        SERIAL_ECHOPAIR(" B", TEMP_UNIT(lcd_preheat_bed_temp[i]));
        SERIAL_ECHOLNPAIR(" F", lcd_preheat_fan_speed[i]);
      }
    #endif // ULTIPANEL

    #if HAS_PID_HEATING

      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("PID settings:");
      }
      #if ENABLED(PIDTEMP)
        #if HOTENDS > 1
          if (forReplay) {
            HOTEND_LOOP() {
              CONFIG_ECHO_START;
              SERIAL_ECHOPAIR("  M301 E", e);
              SERIAL_ECHOPAIR(" P", PID_PARAM(Kp, e));
              SERIAL_ECHOPAIR(" I", unscalePID_i(PID_PARAM(Ki, e)));
              SERIAL_ECHOPAIR(" D", unscalePID_d(PID_PARAM(Kd, e)));
              #if ENABLED(PID_EXTRUSION_SCALING)
                SERIAL_ECHOPAIR(" C", PID_PARAM(Kc, e));
                if (e == 0) SERIAL_ECHOPAIR(" L", lpq_len);
              #endif
              SERIAL_EOL();
            }
          }
          else
        #endif // HOTENDS > 1
        // !forReplay || HOTENDS == 1
        {
          CONFIG_ECHO_START;
          SERIAL_ECHOPAIR("  M301 P", PID_PARAM(Kp, 0)); // for compatibility with hosts, only echo values for E0
          SERIAL_ECHOPAIR(" I", unscalePID_i(PID_PARAM(Ki, 0)));
          SERIAL_ECHOPAIR(" D", unscalePID_d(PID_PARAM(Kd, 0)));
          #if ENABLED(PID_EXTRUSION_SCALING)
            SERIAL_ECHOPAIR(" C", PID_PARAM(Kc, 0));
            SERIAL_ECHOPAIR(" L", lpq_len);
          #endif
          SERIAL_EOL();
        }
      #endif // PIDTEMP

      #if ENABLED(PIDTEMPBED)
        CONFIG_ECHO_START;
        SERIAL_ECHOPAIR("  M304 P", thermalManager.bedKp);
        SERIAL_ECHOPAIR(" I", unscalePID_i(thermalManager.bedKi));
        SERIAL_ECHOPAIR(" D", unscalePID_d(thermalManager.bedKd));
        SERIAL_EOL();
      #endif

    #endif // PIDTEMP || PIDTEMPBED

    #if HAS_LCD_CONTRAST
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("LCD Contrast:");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPAIR("  M250 C", lcd_contrast);
    #endif

    #if ENABLED(FWRETRACT)

      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Retract: S<length> F<units/m> Z<lift>");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M207 S", LINEAR_UNIT(retract_length));
      SERIAL_ECHOPAIR(" W", LINEAR_UNIT(swap_retract_length));
      SERIAL_ECHOPAIR(" F", MMS_TO_MMM(LINEAR_UNIT(retract_feedrate_mm_s)));
      SERIAL_ECHOLNPAIR(" Z", LINEAR_UNIT(retract_zlift));

      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Recover: S<length> F<units/m>");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M208 S", LINEAR_UNIT(retract_recover_length));
      SERIAL_ECHOPAIR(" W", LINEAR_UNIT(swap_retract_recover_length));
      SERIAL_ECHOLNPAIR(" F", MMS_TO_MMM(LINEAR_UNIT(retract_recover_feedrate_mm_s)));

      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Auto-Retract: S=0 to disable, 1 to interpret E-only moves as retract/recover");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPAIR("  M209 S", autoretract_enabled ? 1 : 0);

    #endif // FWRETRACT

    /**
     * Auto Bed Leveling
     */
    #if HAS_BED_PROBE
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Z-Probe Offset (mm):");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOLNPAIR("  M851 Z", LINEAR_UNIT(zprobe_zoffset));
    #endif

    /**
     * TMC2130 stepper driver current
     */
    #if ENABLED(HAVE_TMC2130)
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Stepper driver current:");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHO("  M906");
      #if ENABLED(X_IS_TMC2130)
        SERIAL_ECHOPAIR(" X", stepperX.getCurrent());
      #endif
      #if ENABLED(Y_IS_TMC2130)
        SERIAL_ECHOPAIR(" Y", stepperY.getCurrent());
      #endif
      #if ENABLED(Z_IS_TMC2130)
        SERIAL_ECHOPAIR(" Z", stepperZ.getCurrent());
      #endif
      #if ENABLED(X2_IS_TMC2130)
        SERIAL_ECHOPAIR(" X2", stepperX2.getCurrent());
      #endif
      #if ENABLED(Y2_IS_TMC2130)
        SERIAL_ECHOPAIR(" Y2", stepperY2.getCurrent());
      #endif
      #if ENABLED(Z2_IS_TMC2130)
        SERIAL_ECHOPAIR(" Z2", stepperZ2.getCurrent());
      #endif
      #if ENABLED(E0_IS_TMC2130)
        SERIAL_ECHOPAIR(" E0", stepperE0.getCurrent());
      #endif
      #if ENABLED(E1_IS_TMC2130)
        SERIAL_ECHOPAIR(" E1", stepperE1.getCurrent());
      #endif
      #if ENABLED(E2_IS_TMC2130)
        SERIAL_ECHOPAIR(" E2", stepperE2.getCurrent());
      #endif
      #if ENABLED(E3_IS_TMC2130)
        SERIAL_ECHOPAIR(" E3", stepperE3.getCurrent());
      #endif
      SERIAL_EOL();
    #endif

    /**
     * Linear Advance
     */
    #if ENABLED(LIN_ADVANCE)
      if (!forReplay) {
        CONFIG_ECHO_START;
        SERIAL_ECHOLNPGM("Linear Advance:");
      }
      CONFIG_ECHO_START;
      SERIAL_ECHOPAIR("  M900 K", planner.extruder_advance_k);
      SERIAL_ECHOLNPAIR(" R", planner.advance_ed_ratio);
    #endif

    #if HAS_MOTOR_CURRENT_PWM
      CONFIG_ECHO_START;
      if (!forReplay) {
        SERIAL_ECHOLNPGM("Stepper motor currents:");
        CONFIG_ECHO_START;
      }
      SERIAL_ECHOPAIR("  M907 X", stepper.motor_current_setting[0]);
      SERIAL_ECHOPAIR(" Z", stepper.motor_current_setting[1]);
      SERIAL_ECHOPAIR(" E", stepper.motor_current_setting[2]);
      SERIAL_EOL();
    #endif
  }

#endif // !DISABLE_M503
