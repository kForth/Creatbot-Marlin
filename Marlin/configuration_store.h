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

#ifndef CONFIGURATION_STORE_H
#define CONFIGURATION_STORE_H

#include "MarlinConfig.h"

class MarlinSettings {
  public:
    MarlinSettings() { }

    static void reset(bool init = false);
    static bool save();

    #if ENABLED(EEPROM_SETTINGS)
      static bool load();

      #if ENABLED(AUTO_BED_LEVELING_UBL) // Eventually make these available if any leveling system
                                         // That can store is enabled
        FORCE_INLINE static int get_start_of_meshes() { return meshes_begin; }
        FORCE_INLINE static int get_end_of_meshes() { return meshes_end; }
        static int calc_num_meshes();
        static void store_mesh(int8_t slot);
        static void load_mesh(int8_t slot, void *into = 0);

        //static void delete_mesh();    // necessary if we have a MAT
        //static void defrag_meshes();  // "
      #endif
    #else
      FORCE_INLINE
      static bool load() { reset(); report(); return true; }
    #endif

    #if DISABLED(DISABLE_M503)
      static void report(bool forReplay=false);
    #else
      FORCE_INLINE
      static void report(bool forReplay=false) { UNUSED(forReplay); }
    #endif

  private:
    static void postprocess();

    #if ENABLED(EEPROM_SETTINGS)
      static bool eeprom_error;
	    static bool readCheck();
      #if ENABLED(AUTO_BED_LEVELING_UBL) // Eventually make these available if any leveling system
                                         // That can store is enabled
        static int meshes_begin;
        const static int meshes_end = E2END - 128; // 128 is a placeholder for the size of the MAT; the MAT will always
                                                   // live at the very end of the eeprom

      #endif

    #endif
};

extern MarlinSettings settings;

//#define PARSE_SETTING_ADDR

#if ENABLED(EEPROM_SETTINGS)
	#define EEPROM_OFFSET 100
	#if ENABLED(PARSE_SETTING_ADDR)
		#define SETTING_ADDR_versionFW														(EEPROM_OFFSET)
		#define SETTING_ADDR_axis_steps_per_mm										(sizeof(char) 			* (FW_STR_LEN + 1) + SETTING_ADDR_versionFW)
		#define SETTING_ADDR_max_feedrate_mm_s										(sizeof(float) 		* XYZE_N + SETTING_ADDR_axis_steps_per_mm)
		#define SETTING_ADDR_max_acceleration_mm_per_s2						(sizeof(float) 		* XYZE_N + SETTING_ADDR_max_feedrate_mm_s)
		#define SETTING_ADDR_acceleration													(sizeof(uint32_t) 	* XYZE_N + SETTING_ADDR_max_acceleration_mm_per_s2)
		#define SETTING_ADDR_retract_acceleration									(sizeof(float) 		+ SETTING_ADDR_acceleration)
		#define SETTING_ADDR_travel_acceleration									(sizeof(float)			+ SETTING_ADDR_retract_acceleration)
		#define SETTING_ADDR_min_feedrate_mm_s										(sizeof(float) 		+ SETTING_ADDR_travel_acceleration)
		#define SETTING_ADDR_min_travel_feedrate_mm_s							(sizeof(float) 		+ SETTING_ADDR_min_feedrate_mm_s)
		#define SETTING_ADDR_min_segment_time											(sizeof(float)			+ SETTING_ADDR_min_travel_feedrate_mm_s)
		#define SETTING_ADDR_max_jerk															(sizeof(millis_t)		+ SETTING_ADDR_min_segment_time)
		#define SETTING_ADDR_home_offset													(sizeof(float)			* NUM_AXIS + SETTING_ADDR_max_jerk)
		#define SETTING_ADDR_hotend_offset												(sizeof(float)			* XYZ + SETTING_ADDR_home_offset)

		#define SETTING_ADDR_z_fade_height												(sizeof(float)			* XYZ * MAX_EXTRUDERS + SETTING_ADDR_hotend_offset)

		#define SETTING_ADDR_zprobe_zoffset												(sizeof(float)			+ SETTING_ADDR_z_fade_height)

		#define SETTING_ADDR_bed_level_matrix											(sizeof(float)			+ SETTING_ADDR_zprobe_zoffset)

		#define SETTING_ADDR_mesh_bilinear_ubl_status							(sizeof(float)			* 9 + SETTING_ADDR_bed_level_matrix)
		#define SETTING_ADDR_mesh_bilinear_max_x									(sizeof(bool)			+ SETTING_ADDR_mesh_bilinear_ubl_status)
		#define SETTING_ADDR_mesh_bilinear_max_y									(sizeof(uint8_t)		+ SETTING_ADDR_mesh_bilinear_max_x)
		#define SETTING_ADDR_bilinear_grid_spacing								(sizeof(uint8_t)		+ SETTING_ADDR_mesh_bilinear_max_y)
		#define SETTING_ADDR_bilinear_start												(sizeof(int)				* 2 + SETTING_ADDR_bilinear_grid_spacing)
		#define SETTING_ADDR_mesh_ubl_z_offset										(sizeof(int)				* 2 + SETTING_ADDR_bilinear_start)
		#define SETTING_ADDR_ubl_eeprom_storage_slot							(sizeof(float)			+ SETTING_ADDR_mesh_ubl_z_offset)
		#define SETTING_ADDR_mesh_bilinear_z_values								(sizeof(int8_t)			+ SETTING_ADDR_ubl_eeprom_storage_slot)

		#if ENABLED(AUTO_BED_LEVELING_BILINEAR) || ENABLED(MESH_BED_LEVELING)
			#define SETTING_ADDR_endstop_adj												(sizeof(float)			* GRID_MAX_POINTS_X * GRID_MAX_POINTS_Y + SETTING_ADDR_mesh_bilinear_z_values)
		#else
			#define SETTING_ADDR_endstop_adj												(sizeof(float)			* 9 + SETTING_ADDR_mesh_bilinear_z_values)
		#endif

		#define SETTING_ADDR_delta_radius													(sizeof(float)			* ABC + SETTING_ADDR_endstop_adj)
		#define SETTING_ADDR_delta_diagonal_rod										(sizeof(float)			+ SETTING_ADDR_delta_radius)
		#define SETTING_ADDR_delta_segments_per_second						(sizeof(float)			+ SETTING_ADDR_delta_diagonal_rod)
		#define SETTING_ADDR_delta_calibration_radius							(sizeof(float)			+ SETTING_ADDR_delta_segments_per_second)
		#define SETTING_ADDR_delta_tower_angle_trim								(sizeof(float)			+ SETTING_ADDR_delta_calibration_radius)

		#define SETTING_ADDR_lcd_preheat_hotend_temp							(sizeof(float)			* ABC + SETTING_ADDR_delta_tower_angle_trim)
		#define SETTING_ADDR_lcd_preheat_bed_temp									(sizeof(int)				* MAX_EXTRUDERS + SETTING_ADDR_lcd_preheat_hotend_temp)
		#define SETTING_ADDR_lcd_preheat_chamber_temp							(sizeof(int)				+ SETTING_ADDR_lcd_preheat_bed_temp)
		#define SETTING_ADDR_lcd_preheat_fan_speed								(sizeof(int)				+ SETTING_ADDR_lcd_preheat_chamber_temp)

		#define SETTING_ADDR_Kpidc																(sizeof(int)				+ SETTING_ADDR_lcd_preheat_fan_speed)
		#define SETTING_ADDR_lpq_len															(sizeof(float)			* MAX_EXTRUDERS * 4 + SETTING_ADDR_Kpidc)

		#define SETTING_ADDR_bedKpid															(sizeof(int)				+ SETTING_ADDR_lpq_len)

		#define SETTING_ADDR_lcd_contrast													(sizeof(float)			* 3 + SETTING_ADDR_bedKpid)

		#define SETTING_ADDR_autoretract_enabled									(sizeof(int)				+ SETTING_ADDR_lcd_contrast)
		#define SETTING_ADDR_retract_length												(sizeof(bool)			+ SETTING_ADDR_autoretract_enabled)
		#define SETTING_ADDR_retract_feedrate_mm_s								(sizeof(float)			+ SETTING_ADDR_retract_length)
		#define SETTING_ADDR_retract_zlift												(sizeof(float)			+ SETTING_ADDR_retract_feedrate_mm_s)
		#define SETTING_ADDR_retract_recover_length								(sizeof(float)			+ SETTING_ADDR_retract_zlift)
		#define SETTING_ADDR_retract_recover_feedrate_mm_s				(sizeof(float)			+ SETTING_ADDR_retract_recover_length)
		#define SETTING_ADDR_swap_retract_length									(sizeof(float)			+ SETTING_ADDR_retract_recover_feedrate_mm_s)
		#define SETTING_ADDR_swap_retract_recover_length					(sizeof(float)			+ SETTING_ADDR_swap_retract_length)
		#define SETTING_ADDR_swap_retract_recover_feedrate_mm_s		(sizeof(float)			+ SETTING_ADDR_swap_retract_recover_length)

		#define SETTING_ADDR_volumetric_enabled										(sizeof(float)			+ SETTING_ADDR_swap_retract_recover_feedrate_mm_s)
		#define SETTING_ADDR_filament_size												(sizeof(bool)			+ SETTING_ADDR_volumetric_enabled)

		#define SETTING_ADDR_stepperX_current											(sizeof(float)			* MAX_EXTRUDERS + SETTING_ADDR_filament_size)
		#define SETTING_ADDR_stepperY_current											(sizeof(uint16_t)		+ SETTING_ADDR_stepperX_current)
		#define SETTING_ADDR_stepperZ_current											(sizeof(uint16_t)		+ SETTING_ADDR_stepperY_current)
		#define SETTING_ADDR_stepperX2_current										(sizeof(uint16_t)		+ SETTING_ADDR_stepperZ_current)
		#define SETTING_ADDR_stepperY2_current										(sizeof(uint16_t)		+ SETTING_ADDR_stepperX2_current)
		#define SETTING_ADDR_stepperZ2_current										(sizeof(uint16_t)		+ SETTING_ADDR_stepperY2_current)
		#define SETTING_ADDR_stepperE0_current										(sizeof(uint16_t)		+ SETTING_ADDR_stepperZ2_current)
		#define SETTING_ADDR_stepperE1_current										(sizeof(uint16_t)		+ SETTING_ADDR_stepperE0_current)
		#define SETTING_ADDR_stepperE2_current										(sizeof(uint16_t)		+ SETTING_ADDR_stepperE1_current)
		#define SETTING_ADDR_stepperE3_current										(sizeof(uint16_t)		+ SETTING_ADDR_stepperE2_current)
		#define SETTING_ADDR_stepperE4_current										(sizeof(uint16_t)		+ SETTING_ADDR_stepperE3_current)

		#define SETTING_ADDR_extruder_advance_k										(sizeof(uint16_t)		+ SETTING_ADDR_stepperE4_current)
		#define SETTING_ADDR_advance_ed_ratio											(sizeof(float)			+ SETTING_ADDR_extruder_advance_k)

		#define SETTING_ADDR_motor_current												(sizeof(float)			+ SETTING_ADDR_advance_ed_ratio)

		#define SETTING_ADDR_extruder_auto_fan_speed							(sizeof(uint32_t)		* 3 + SETTING_ADDR_motor_current)
		#define SETTING_ADDR_air_fan_speed												(sizeof(int)				+ SETTING_ADDR_extruder_auto_fan_speed)

		#define SETTING_ADDR_regSN																(sizeof(int)				+ SETTING_ADDR_air_fan_speed)
		#define SETTING_ADDR_usedTime															(sizeof(float)			+ SETTING_ADDR_regSN)

		#define SETTING_ADDR_wifiEnable														(sizeof(uint32_t)		+ SETTING_ADDR_usedTime)

		#define SETTING_ADDR_isAccident														(sizeof(bool)     + SETTING_ADDR_wifiEnable)
		#define SETTING_ADDR_pauseLeveling												(sizeof(bool)			+ SETTING_ADDR_isAccident)
		#define SETTING_ADDR_pausePos															(sizeof(bool)			+ SETTING_ADDR_pauseLeveling)
		#define SETTING_ADDR_pauseSpeed														(sizeof(float)			* XYZE + SETTING_ADDR_pausePos)
		#define SETTING_ADDR_pauseByteOrLineN											(sizeof(float)			+ SETTING_ADDR_pauseSpeed)
		#define SETTING_ADDR_lastPos															(sizeof(uint32_t)		+ SETTING_ADDR_pauseByteOrLineN)
		#define SETTING_ADDR_lastToolsState												(sizeof(float)			* XYZE + SETTING_ADDR_lastPos)
		#define SETTING_ADDR_lastFilename													(sizeof(int)				* TOOLS_NUM + SETTING_ADDR_lastToolsState)

		#define SETTING_ADDR_END																	(sizeof(char)			* 225 + SETTING_ADDR_lastFilename)
	#else
		#define SETTING_ADDR_OFFSET																(EEPROM_OFFSET - 100)

		#define SETTING_ADDR_versionFW														(100 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_axis_steps_per_mm										(112 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_max_feedrate_mm_s										(128 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_max_acceleration_mm_per_s2						(144 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_acceleration													(160 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_retract_acceleration									(164 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_travel_acceleration									(168 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_min_feedrate_mm_s										(172 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_min_travel_feedrate_mm_s							(176 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_min_segment_time											(180 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_max_jerk															(184 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_home_offset													(200 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_hotend_offset												(212 + SETTING_ADDR_OFFSET)

		#define SETTING_ADDR_z_fade_height												(272 + SETTING_ADDR_OFFSET)

		#define SETTING_ADDR_zprobe_zoffset												(276 + SETTING_ADDR_OFFSET)

		#define SETTING_ADDR_bed_level_matrix											(280 + SETTING_ADDR_OFFSET)

		#define SETTING_ADDR_mesh_bilinear_ubl_status							(316 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_mesh_bilinear_max_x									(317 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_mesh_bilinear_max_y									(318 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_bilinear_grid_spacing								(319 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_bilinear_start												(323 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_mesh_ubl_z_offset										(327 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_ubl_eeprom_storage_slot							(331 + SETTING_ADDR_OFFSET)
		#define SETTING_ADDR_mesh_bilinear_z_values								(332 + SETTING_ADDR_OFFSET)

		#if ENABLED(AUTO_BED_LEVELING_BILINEAR) || ENABLED(MESH_BED_LEVELING)
			#define SETTING_ADDR_OFFSET_2														(SETTING_ADDR_OFFSET + GRID_MAX_POINTS * 4 - 36)
		#else
			#define SETTING_ADDR_OFFSET_2														(SETTING_ADDR_OFFSET)
		#endif

		#define SETTING_ADDR_endstop_adj													(368 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_delta_radius													(380 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_delta_diagonal_rod										(384 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_delta_segments_per_second						(388 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_delta_calibration_radius							(392 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_delta_tower_angle_trim								(396 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_lcd_preheat_hotend_temp							(408 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_lcd_preheat_bed_temp									(418 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_lcd_preheat_chamber_temp							(420 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_lcd_preheat_fan_speed								(422 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_Kpidc																(424 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_lpq_len															(504 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_bedKpid															(506 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_lcd_contrast													(518 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_autoretract_enabled									(520 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_retract_length												(521 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_retract_feedrate_mm_s								(525 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_retract_zlift												(529 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_retract_recover_length								(533 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_retract_recover_feedrate_mm_s				(537 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_swap_retract_length									(541 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_swap_retract_recover_length					(545 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_swap_retract_recover_feedrate_mm_s		(549 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_volumetric_enabled										(553 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_filament_size												(554 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_stepperX_current											(574 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperY_current											(576 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperZ_current											(578 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperX2_current										(580 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperY2_current										(582 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperZ2_current										(584 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperE0_current										(586 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperE1_current										(588 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperE2_current										(590 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperE3_current										(592 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_stepperE4_current										(594 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_extruder_advance_k										(596 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_advance_ed_ratio											(600 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_motor_current												(604 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_extruder_auto_fan_speed							(616 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_air_fan_speed												(618 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_regSN																(620 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_usedTime															(624 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_wifiEnable														(628 + SETTING_ADDR_OFFSET_2)

		#define SETTING_ADDR_isAccident														(629 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_pauseLeveling												(630 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_pausePos															(631 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_pauseSpeed														(647 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_pauseByteOrLineN											(651 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_lastPos															(655 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_lastToolsState												(671 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_lastFilename													(687 + SETTING_ADDR_OFFSET_2)
		#define SETTING_ADDR_END																	(912 + SETTING_ADDR_OFFSET_2)
	#endif


	#define EEPROM_UPDATE_VAR(address, var)		eeprom_update_block((const void *)&(var), (void *)(address), sizeof(var))
	#define EEPROM_READ_VAR(address, var) 		eeprom_read_block((void *)&(var), (const void *)(address), sizeof(var))

	#define EEPROM_STORE(var, setting) 				EEPROM_UPDATE_VAR(SETTING_ADDR_ ## setting, var)
	#define STORE_SETTING(var) 								EEPROM_UPDATE_VAR(SETTING_ADDR_ ## var, var)
	#define STORE_PLAN(var) 									EEPROM_UPDATE_VAR(SETTING_ADDR_ ## var, planner.var)
	#define STORE_TMC(var)										EEPROM_UPDATE_VAR(SETTING_ADDR_ ## var ## _current, var.getCurrent())

	#define EEPROM_READ(var, setting) 				EEPROM_READ_VAR(SETTING_ADDR_ ## setting, var)
	#define READ_SETTING(var) 								EEPROM_READ_VAR(SETTING_ADDR_ ## var, var)
	#define READ_PLAN(var) 										EEPROM_READ_VAR(SETTING_ADDR_ ## var, planner.var)
	#define READ_TMC(var)											EEPROM_READ_VAR(SETTING_ADDR_ ## var ## _current, var.getCurrent())

#else
	#define EEPROM_STORE(var, setting) 				NOOP
	#define STORE_SETTING(var) 								NOOP
	#define STORE_PLAN(var) 									NOOP
	#define STORE_TMC(var)										NOOP
#endif

#endif // CONFIGURATION_STORE_H
