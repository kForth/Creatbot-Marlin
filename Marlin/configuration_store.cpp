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


#include "configuration_store.h"

MarlinSettings settings;

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
	

  bool MarlinSettings::readCheck(){
  	bool eeprom_read_pass = true;
		#define EEPROM_ASSERT(TST, ERR) if (!(TST)) do{ SERIAL_ERROR_START(); SERIAL_ERRORLNPGM(ERR); eeprom_read_pass = false; }while(0)
		#define EEPROM_ASSERT_SHOW(TST, ERR, VAR) if (!(TST))\
			do{ SERIAL_ERROR_START(); SERIAL_ERRORLNPGM(ERR); SERIAL_ECHOLNPAIR("	Current: ", VAR); eeprom_read_pass = false; }while(0)

//		#define EEPROM_CHECK(var, min, max, err) EEPROM_ASSERT(WITHIN((var), (min), (max)), err)
		#define EEPROM_CHECK(var, min, max, err) EEPROM_ASSERT_SHOW(WITHIN((var), (min), (max)), err, (var))

		LOOP_XYZE_N(e){
			EEPROM_CHECK(planner.axis_steps_per_mm[e], 5, 9999, "axis_steps_per_mm out of range");
		}

		#if HAS_HOME_OFFSET
			EEPROM_CHECK(home_offset[X_AXIS], X_MIN_POS, X_MAX_POS, "home_offset_x_axis out of range");
			EEPROM_CHECK(home_offset[Y_AXIS], Y_MIN_POS, Y_MAX_POS, "home_offset_y_axis out of range");
			EEPROM_CHECK(home_offset[Z_AXIS], Z_MIN_POS, Z_MAX_POS, "home_offset_z_axis out of range");
		#endif

		#if HOTENDS > 1
			//TODO hotend_offset
		#endif

		#if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
			EEPROM_CHECK(planner.z_fade_height, 0, 100, "z_fade_height out of range");
		#endif

		#if HAS_BED_PROBE
			EEPROM_CHECK(zprobe_zoffset, -20, 20, "zprobe_zoffset out of range");
		#endif

		//TODO Leveling settings

		#if ENABLED(ULTIPANEL)
			LOOP_L_N(i, 2){
				EEPROM_CHECK(lcd_preheat_hotend_temp[i], 0, HEATER_0_MAXTEMP - 15, "lcd_preheat_hotend_temp out of range");
				EEPROM_CHECK(lcd_preheat_bed_temp[i], 0, BED_MAXTEMP - 15, "lcd_preheat_bed_temp out of range");
				#ifdef HOTWIND_SYSTEM
					EEPROM_CHECK(lcd_preheat_chamber_temp[i], 0, CHAMBER_MAXTEMP - 5, "lcd_preheat_chamber_temp out of range");
				#endif
				EEPROM_CHECK(lcd_preheat_fan_speed[i], 0, 255, "lcd_preheat_fan_speed out of range");
			}
		#endif

		#if ENABLED(DWIN_LCD)
			LOOP_L_N(i, EXTRUDERS){
				EEPROM_CHECK(lcd_preheat_hotend_temp[i], 0, HEATER_0_MAXTEMP - 15, "lcd_preheat_hotend_temp out of range");
			}
			EEPROM_CHECK(lcd_preheat_bed_temp, 0, BED_MAXTEMP - 15, "lcd_preheat_bed_temp out of range");
			#ifdef HOTWIND_SYSTEM
				EEPROM_CHECK(lcd_preheat_chamber_temp, 0, CHAMBER_MAXTEMP - 5, "lcd_preheat_chamber_temp out of range");
			#endif
			EEPROM_CHECK(lcd_preheat_fan_speed, 0, 255, "lcd_preheat_fan_speed out of range");
		#endif

		#if HAS_LCD_CONTRAST
			EEPROM_CHECK(lcd_contrast, LCD_CONTRAST_MIN, LCD_CONTRAST_MAX, "lcd_contrast out of range");
		#endif

		//TODO PID Settings
		//TODO FWRETRACT

		LOOP_L_N(e, EXTRUDERS){
			EEPROM_CHECK(filament_size[e], 1.5, 3.25, "filament_size out of range");
		}

		//TODO TMC2130

		#if ENABLED(LIN_ADVANCE)
			EEPROM_CHECK(extruder_advance_k, 0, 999, "extruder_advance_k out of range");
		#endif

		#if HAS_AUTO_FAN
			EEPROM_CHECK(extruder_auto_fan_speed, 0, 255, "extruder_auto_fan_speed out of range");
		#endif

		#ifdef HAS_AIR_FAN
			EEPROM_CHECK(air_fan_speed, 0, 255, "air_fan_speed out of range");
		#endif

		#ifdef WIFI_SUPPORT
			//TODO Mode Check
		#endif

		#ifdef ACCIDENT_DETECT
			//TODO CRC Check
		#endif

  	return eeprom_read_pass;
  }

  /**
   * M500 - Store Configuration
   */
  bool MarlinSettings::save() {
  	STORE_SETTING(versionFW);
  	STORE_PLAN(axis_steps_per_mm);
  	STORE_PLAN(max_feedrate_mm_s);
  	STORE_PLAN(max_acceleration_mm_per_s2);
  	STORE_PLAN(acceleration);
  	STORE_PLAN(retract_acceleration);
  	STORE_PLAN(travel_acceleration);
  	STORE_PLAN(min_feedrate_mm_s);
  	STORE_PLAN(min_travel_feedrate_mm_s);
  	STORE_PLAN(min_segment_time);
  	STORE_PLAN(max_jerk);

		#if HAS_HOME_OFFSET
  		STORE_SETTING(home_offset);
		#endif

		#if HOTENDS > 1
  		STORE_SETTING(hotend_offset);
		#endif

    // Global Leveling
    #if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
  		STORE_PLAN(z_fade_height);
    #endif

		// HAS_BED_PROBE
		#if HAS_BED_PROBE
  		STORE_SETTING(zprobe_zoffset);
		#endif

		// Planar Bed Leveling matrix
		#if ABL_PLANAR
  		STORE_PLAN(bed_level_matrix);
		#endif

		#if ENABLED(MESH_BED_LEVELING)	// Mesh Bed Leveling
			// Compile time test that sizeof(mbl.z_values) is as expected
      static_assert(
        sizeof(mbl.z_values) == GRID_MAX_POINTS * sizeof(mbl.z_values[0][0]),
        "MBL Z array is the wrong size."
      );
      const bool status = TEST(mbl.status, MBL_STATUS_HAS_MESH_BIT);
      const uint8_t mesh_num_x = GRID_MAX_POINTS_X, mesh_num_y = GRID_MAX_POINTS_Y;
  		EEPROM_STORE(status, mesh_bilinear_ubl_status);
  		EEPROM_STORE(mesh_num_x, mesh_bilinear_max_x);
  		EEPROM_STORE(mesh_num_y, mesh_bilinear_max_y);
  		EEPROM_STORE(mbl.z_offset, mesh_ubl_z_offset);
  		EEPROM_STORE(mbl.z_values, mesh_bilinear_z_values);
		#elif ENABLED(AUTO_BED_LEVELING_BILINEAR)		// Bilinear Auto Bed Leveling
      // Compile time test that sizeof(z_values) is as expected
      static_assert(
        sizeof(z_values) == GRID_MAX_POINTS * sizeof(z_values[0][0]),
        "Bilinear Z array is the wrong size."
      );
      const uint8_t grid_max_x = GRID_MAX_POINTS_X, grid_max_y = GRID_MAX_POINTS_Y;
      EEPROM_STORE(planner.abl_enabled, mesh_bilinear_ubl_status);
      EEPROM_STORE(grid_max_x, mesh_bilinear_max_x);
      EEPROM_STORE(grid_max_y, mesh_bilinear_max_y);
      STORE_SETTING(bilinear_grid_spacing);
      STORE_SETTING(bilinear_start);
      EEPROM_STORE(z_values, mesh_bilinear_z_values);
		#elif ENABLED(AUTO_BED_LEVELING_UBL)				// Unified Bed Leveling
      EEPROM_STORE(ubl.state.active, mesh_bilinear_ubl_status);
  		EEPROM_STORE(ubl.state.z_offset, mesh_ubl_z_offset);
  		EEPROM_STORE(ubl.state.eeprom_storage_slot, ubl_eeprom_storage_slot);
		#endif

		#if ENABLED(DELTA)
  		STORE_SETTING(endstop_adj);
  		STORE_SETTING(delta_radius);
  		STORE_SETTING(delta_diagonal_rod);
  		STORE_SETTING(delta_segments_per_second);
  		STORE_SETTING(delta_calibration_radius);
  		STORE_SETTING(delta_tower_angle_trim);
		#elif ENABLED(Z_DUAL_ENDSTOPS)
			EEPROM_STORE(z_endstop_adj, endstop_adj);
		#endif

		#if ENABLED(ULTIPANEL) || ENABLED(DWIN_LCD)
			STORE_SETTING(lcd_preheat_hotend_temp);
			STORE_SETTING(lcd_preheat_bed_temp);
			#ifdef HOTWIND_SYSTEM
				STORE_SETTING(lcd_preheat_chamber_temp);
			#endif
			STORE_SETTING(lcd_preheat_fan_speed);
		#endif

		#if ENABLED(PIDTEMP)
    #if PID_PARAMS_USE_TEMP_RANGE
			TEMP_RANGE_LOOP(){
        EEPROM_STORE(PID_PARAM(Kp, s), Kpidc + s * 16 + 0);
        EEPROM_STORE(PID_PARAM(Ki, s), Kpidc + s * 16 + 4);
        EEPROM_STORE(PID_PARAM(Kd, s), Kpidc + s * 16 + 8);
        #if ENABLED(PID_EXTRUSION_SCALING)
          EEPROM_STORE(PID_PARAM(Kd, s), Kpidc + s * 16 + 12);
        #endif
      }
    #else
			#if ENABLED(PID_PARAMS_PER_HOTEND) && HOTENDS > 1
				HOTEND_LOOP()
			#else
				uint8_t e = 0;
			#endif
				{
					EEPROM_STORE(PID_PARAM(Kp, e), Kpidc + e * 16 + 0);
					EEPROM_STORE(PID_PARAM(Ki, e), Kpidc + e * 16 + 4);
					EEPROM_STORE(PID_PARAM(Kd, e), Kpidc + e * 16 + 8);
					#if ENABLED(PID_EXTRUSION_SCALING)
						EEPROM_STORE(PID_PARAM(Kd, e), Kpidc + e * 16 + 12);
					#endif
				}
    #endif // PID_PARAMS_USE_TEMP_RANGE
		#endif

		#if ENABLED(PID_EXTRUSION_SCALING)
				STORE_SETTING(lpq_len);
		#endif

		#if ENABLED(PIDTEMPBED)
			EEPROM_STORE(thermalManager.bedKp, bedKpid + 0);
			EEPROM_STORE(thermalManager.bedKi, bedKpid + 4);
			EEPROM_STORE(thermalManager.bedKd, bedKpid + 8);
		#endif

		#if HAS_LCD_CONTRAST
			STORE_SETTING(lcd_contrast);
		#endif

		#if ENABLED(FWRETRACT)
			STORE_SETTING(autoretract_enabled);
			STORE_SETTING(retract_length);
			STORE_SETTING(retract_feedrate_mm_s);
			STORE_SETTING(retract_zlift);
			STORE_SETTING(retract_recover_length);
			STORE_SETTING(retract_recover_feedrate_mm_s);
			#if EXTRUDERS > 1
				STORE_SETTING(swap_retract_length);
				STORE_SETTING(swap_retract_recover_length);
				STORE_SETTING(swap_retract_recover_feedrate_mm_s);
			#endif
		#endif

		STORE_SETTING(volumetric_enabled);
		STORE_SETTING(filament_size);

    // Save TMC2130 Configuration, and placeholder values
		#if ENABLED(HAVE_TMC2130)
			#if ENABLED(X_IS_TMC2130)
				STORE_TMC(stepperX);
			#endif
			#if ENABLED(Y_IS_TMC2130)
				STORE_TMC(stepperY);
			#endif
			#if ENABLED(Z_IS_TMC2130)
				STORE_TMC(stepperZ);
			#endif
			#if ENABLED(X2_IS_TMC2130)
				STORE_TMC(stepperX2);
			#endif
			#if ENABLED(Y2_IS_TMC2130)
				STORE_TMC(stepperY2);
			#endif
			#if ENABLED(Z2_IS_TMC2130)
				STORE_TMC(stepperZ2);
			#endif
			#if ENABLED(E0_IS_TMC2130)
				STORE_TMC(stepperE0);
			#endif
			#if ENABLED(E1_IS_TMC2130)
				STORE_TMC(stepperE1);
			#endif
			#if ENABLED(E2_IS_TMC2130)
				STORE_TMC(stepperE2);
			#endif
			#if ENABLED(E3_IS_TMC2130)
				STORE_TMC(stepperE3);
			#endif
			#if ENABLED(E4_IS_TMC2130)
				STORE_TMC(stepperE4);
			#endif
		#endif

    // Linear Advance
    #if ENABLED(LIN_ADVANCE)
			STORE_PLAN(extruder_advance_k);
			STORE_PLAN(advance_ed_ratio);
    #endif

    #if HAS_MOTOR_CURRENT_PWM
			EEPROM_STORE(stepper.motor_current_setting, motor_current);
    #endif

		#if HAS_AUTO_FAN
      STORE_SETTING(extruder_auto_fan_speed);
		#endif

		#ifdef HAS_AIR_FAN
      STORE_SETTING(air_fan_speed);
		#endif

		#ifdef REG_SN
      STORE_SETTING(regSN);
		#endif
      STORE_SETTING(usedTime);

		#ifdef WIFI_SUPPORT
      EEPROM_STORE(myWifi.enable, wifiEnable);
		#endif

		#ifdef ACCIDENT_DETECT
      STORE_SETTING(isAccident);
		#ifdef HAS_LEVELING
     	STORE_SETTING(pauseLeveling);
		#endif
      STORE_SETTING(pausePos);
      STORE_SETTING(pauseSpeed);
      STORE_SETTING(pauseByteOrLineN);
      STORE_SETTING(lastPos);
      STORE_SETTING(lastToolsState);
		#if HAS_READER
      STORE_SETTING(lastFilename);
		#endif
		#endif

		// Report storage size
		#if ENABLED(EEPROM_CHITCHAT)
			SERIAL_ECHO_START();
			SERIAL_ECHOPAIR("Settings Stored (", (unsigned long)(SETTING_ADDR_END - EEPROM_OFFSET));
			SERIAL_ECHOLNPGM(" bytes)");
		#endif

		eeprom_error = false;
		
    #if ENABLED(UBL_SAVE_ACTIVE_ON_M500)
      if (ubl.state.storage_slot >= 0)
        store_mesh(ubl.state.storage_slot);
    #endif

    return !eeprom_error;
  }

  /**
   * M501 - Retrieve Configuration
   */
  bool MarlinSettings::load() {
		eeprom_error = false;
  	char stored_ver[12];
  	EEPROM_READ(stored_ver, versionFW);

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
  		READ_PLAN(axis_steps_per_mm);
  		READ_PLAN(max_feedrate_mm_s);
  		READ_PLAN(max_acceleration_mm_per_s2);
  		READ_PLAN(acceleration);
  		READ_PLAN(retract_acceleration);
  		READ_PLAN(travel_acceleration);
  		READ_PLAN(min_feedrate_mm_s);
  		READ_PLAN(min_travel_feedrate_mm_s);
  		READ_PLAN(min_segment_time);
  		READ_PLAN(max_jerk);

			#if HAS_HOME_OFFSET
				#if ENABLED(DELTA)
					home_offset[X_AXIS] = 0.0;
					home_offset[Y_AXIS] = 0.0;
					home_offset[Z_AXIS] -= DELTA_HEIGHT;
				#else
					READ_SETTING(home_offset);
				#endif
			#endif

			#if HOTENDS > 1
				READ_SETTING(hotend_offset);
			#endif

			// Global Leveling
			#if ENABLED(ENABLE_LEVELING_FADE_HEIGHT)
				READ_PLAN(z_fade_height);
			#endif

			// HAS_BED_PROBE
			#if HAS_BED_PROBE
				READ_SETTING(zprobe_zoffset);
			#endif

			// Planar Bed Leveling matrix
			#if ABL_PLANAR
				READ_PLAN(bed_level_matrix);
			#endif

			#if ENABLED(MESH_BED_LEVELING)	// Mesh Bed Leveling
				uint8_t grid_max_x, grid_max_y; bool status;
				EEPROM_READ(grid_max_x, mesh_bilinear_max_x);
				EEPROM_READ(grid_max_y, mesh_bilinear_max_y);
				if((grid_max_x == GRID_MAX_POINTS_X) && (grid_max_y == GRID_MAX_POINTS_Y)){
					EEPROM_READ(status, mesh_bilinear_ubl_status);
					mbl.status = status ? _BV(MBL_STATUS_HAS_MESH_BIT) : 0;
					EEPROM_READ(mbl.z_offset, mesh_ubl_z_offset);
					EEPROM_READ(mbl.z_values, mesh_bilinear_z_values);
				} else {
					mbl.reset();
				}
			#elif ENABLED(AUTO_BED_LEVELING_BILINEAR)		// Bilinear Auto Bed Leveling
				uint8_t grid_max_x, grid_max_y; bool status;
				EEPROM_READ(grid_max_x, mesh_bilinear_max_x);
				EEPROM_READ(grid_max_y, mesh_bilinear_max_y);
				if((grid_max_x == GRID_MAX_POINTS_X) && (grid_max_y == GRID_MAX_POINTS_Y)){
					EEPROM_READ(status, mesh_bilinear_ubl_status);
					READ_SETTING(bilinear_grid_spacing);
					READ_SETTING(bilinear_start);
					EEPROM_READ(z_values, mesh_bilinear_z_values);
					set_bed_leveling_enabled(status);
				}
			#elif ENABLED(AUTO_BED_LEVELING_UBL)				// Unified Bed Leveling
				EEPROM_READ(ubl.state.active, mesh_bilinear_ubl_status);
				EEPROM_READ(ubl.state.z_offset, mesh_ubl_z_offset);
				EEPROM_READ(ubl.state.eeprom_storage_slot, ubl_eeprom_storage_slot);
			#endif

			#if ENABLED(DELTA)
				READ_SETTING(endstop_adj);
				READ_SETTING(delta_radius);
				READ_SETTING(delta_diagonal_rod);
				READ_SETTING(delta_segments_per_second);
				READ_SETTING(delta_calibration_radius);
				READ_SETTING(delta_tower_angle_trim);
			#elif ENABLED(Z_DUAL_ENDSTOPS)
				EEPROM_READ(z_endstop_adj, endstop_adj);
			#endif

			#if ENABLED(ULTIPANEL) || ENABLED(DWIN_LCD)
				READ_SETTING(lcd_preheat_hotend_temp);
				READ_SETTING(lcd_preheat_bed_temp);
				#ifdef HOTWIND_SYSTEM
					READ_SETTING(lcd_preheat_chamber_temp);
				#endif
				READ_SETTING(lcd_preheat_fan_speed);
			#endif

			#if ENABLED(PIDTEMP)
      #if PID_PARAMS_USE_TEMP_RANGE
        TEMP_RANGE_LOOP(){
          EEPROM_READ(PID_PARAM(Kp, s), Kpidc + s * 16 + 0);
          EEPROM_READ(PID_PARAM(Ki, s), Kpidc + s * 16 + 4);
          EEPROM_READ(PID_PARAM(Kd, s), Kpidc + s * 16 + 8);
          #if ENABLED(PID_EXTRUSION_SCALING)
            EEPROM_READ(PID_PARAM(Kd, s), Kpidc + s * 16 + 12);
          #endif
        }
      #else
				#if ENABLED(PID_PARAMS_PER_HOTEND) && HOTENDS > 1
					HOTEND_LOOP()
				#else
					uint8_t e = 0;
				#endif
					{
						EEPROM_READ(PID_PARAM(Kp, e), Kpidc + e * 16 + 0);
						EEPROM_READ(PID_PARAM(Ki, e), Kpidc + e * 16 + 4);
						EEPROM_READ(PID_PARAM(Kd, e), Kpidc + e * 16 + 8);
						#if ENABLED(PID_EXTRUSION_SCALING)
							EEPROM_READ(PID_PARAM(Kd, e), Kpidc + e * 16 + 12);
						#endif
					}
      #endif // PID_PARAMS_USE_TEMP_RANGE
			#endif // PIDTEMP

			#if ENABLED(PID_EXTRUSION_SCALING)
				READ_SETTING(lpq_len);
			#endif

			#if ENABLED(PIDTEMPBED)
				EEPROM_READ(thermalManager.bedKp, bedKpid + 0);
				EEPROM_READ(thermalManager.bedKi, bedKpid + 4);
				EEPROM_READ(thermalManager.bedKd, bedKpid + 8);
			#endif

      #if ENABLED(PIDTEMP_CHAMBER)
		    thermalManager.chamberKp = DEFAULT_chamberKp;
		    thermalManager.chamberKi = scalePID_i(DEFAULT_chamberKi);
		    thermalManager.chamberKd = scalePID_d(DEFAULT_chamberKd);
      #endif

			#if HAS_LCD_CONTRAST
				READ_SETTING(lcd_contrast);
			#endif

		#if ENABLED(FWRETRACT)
			READ_SETTING(autoretract_enabled);
			READ_SETTING(retract_length);
			READ_SETTING(retract_feedrate_mm_s);
			READ_SETTING(retract_zlift);
			READ_SETTING(retract_recover_length);
			READ_SETTING(retract_recover_feedrate_mm_s);
			#if EXTRUDERS > 1
				READ_SETTING(swap_retract_length);
				READ_SETTING(swap_retract_recover_length);
				READ_SETTING(swap_retract_recover_feedrate_mm_s);
			#endif
		#endif

			READ_SETTING(volumetric_enabled);
			READ_SETTING(filament_size);

			#if ENABLED(HAVE_TMC2130)
				#if ENABLED(X_IS_TMC2130)
					READ_TMC(stepperX);
				#endif
				#if ENABLED(Y_IS_TMC2130)
					READ_TMC(stepperY);
				#endif
				#if ENABLED(Z_IS_TMC2130)
					READ_TMC(stepperZ);
				#endif
				#if ENABLED(X2_IS_TMC2130)
					READ_TMC(stepperX2);
				#endif
				#if ENABLED(Y2_IS_TMC2130)
					READ_TMC(stepperY2);
				#endif
				#if ENABLED(Z2_IS_TMC2130)
					READ_TMC(stepperZ2);
				#endif
				#if ENABLED(E0_IS_TMC2130)
					READ_TMC(stepperE0);
				#endif
				#if ENABLED(E1_IS_TMC2130)
					READ_TMC(stepperE1);
				#endif
				#if ENABLED(E2_IS_TMC2130)
					READ_TMC(stepperE2);
				#endif
				#if ENABLED(E3_IS_TMC2130)
					READ_TMC(stepperE3);
				#endif
				#if ENABLED(E4_IS_TMC2130)
					READ_TMC(stepperE4);
				#endif
			#endif

			// Linear Advance
			#if ENABLED(LIN_ADVANCE)
				READ_PLAN(extruder_advance_k);
				READ_PLAN(advance_ed_ratio);
			#endif

      #if HAS_MOTOR_CURRENT_PWM
				EEPROM_READ(stepper.motor_current_setting, setting_motor_current);
      #endif

			#if HAS_AUTO_FAN
				READ_SETTING(extruder_auto_fan_speed);
			#endif

			#ifdef HAS_AIR_FAN
				READ_SETTING(air_fan_speed);
			#endif

			#ifdef REG_SN
				READ_SETTING(regSN);
			#endif
				READ_SETTING(usedTime);

			#ifdef WIFI_SUPPORT
				EEPROM_READ(myWifi.enable, wifiEnable);
			#endif

			#ifdef ACCIDENT_DETECT
				READ_SETTING(isAccident);
				if(isAccident){
				#ifdef HAS_LEVELING
					READ_SETTING(pauseLeveling);
				#endif
					READ_SETTING(pausePos);
					READ_SETTING(pauseSpeed);
					READ_SETTING(pauseByteOrLineN);
					READ_SETTING(lastPos);
					READ_SETTING(lastToolsState);
				#if HAS_READER
					READ_SETTING(lastFilename);
				#endif
				} else{
					#ifdef HAS_LEVELING
						pauseLeveling = false;
					#endif
					ZERO(pausePos);
					pauseSpeed = 0.0;
					pauseByteOrLineN = 0;
					ZERO(lastPos);
					ZERO(lastToolsState);
				#if HAS_READER
					ZERO(lastFilename);
				#endif
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
	    		SERIAL_ECHOPAIR(" stored settings retrieved (", (unsigned long)(SETTING_ADDR_END - EEPROM_OFFSET));
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
  #if PID_PARAMS_USE_TEMP_RANGE
    static float default_kp[PID_PARAMS_TEMP_RANGE_NUM] =  DEFAULT_Kp_ARRAY;
    static float default_ki[PID_PARAMS_TEMP_RANGE_NUM] =  DEFAULT_Ki_ARRAY;
    static float default_kd[PID_PARAMS_TEMP_RANGE_NUM] =  DEFAULT_Kd_ARRAY;
    TEMP_RANGE_LOOP(){
      PID_PARAM(Kp, s) = default_kp[s];
      PID_PARAM(Ki, s) = scalePID_i(default_ki[s]);
      PID_PARAM(Kd, s) = scalePID_d(default_kd[s]);
      #if ENABLED(PID_EXTRUSION_SCALING)
        PID_PARAM(Kc, s) = DEFAULT_Kc;
      #endif
    }
  #else
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
  #endif // PID_PARAMS_USE_TEMP_RANGE
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
		extruder_auto_fan_speed = EXTRUDER_AUTO_FAN_SPEED;
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
      #if PID_PARAMS_USE_TEMP_RANGE
        if(!forReplay){
          TEMP_RANGE_LOOP(){
            CONFIG_ECHO_START;
            SERIAL_ECHOPAIR("  M301 S", s);
            SERIAL_ECHOPAIR(" P", PID_PARAM(Kp, s));
            SERIAL_ECHOPAIR(" I", unscalePID_i(PID_PARAM(Ki, s)));
            SERIAL_ECHOPAIR(" D", unscalePID_d(PID_PARAM(Kd, s)));
            #if ENABLED(PID_EXTRUSION_SCALING)
              SERIAL_ECHOPAIR(" C", PID_PARAM(Kc, s));
              if (e == 0) SERIAL_ECHOPAIR(" L", lpq_len);
            #endif
            SERIAL_EOL();
          }
        }
      #else
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
      #endif // PID_PARAMS_USE_TEMP_RANGE
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
