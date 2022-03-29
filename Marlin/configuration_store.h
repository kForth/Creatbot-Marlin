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


#if ENABLED(EEPROM_SETTINGS)
	typedef struct struct_eeprom_data{
		char			setting_versionFW[FW_STR_LEN + 1];
		float			setting_axis_steps_per_mm[XYZE_N];
		float			setting_max_feedrate_mm_s[XYZE_N];
		uint32_t	setting_max_acceleration_mm_per_s2[XYZE_N];
		float			setting_acceleration;
		float			setting_retract_acceleration;
		float			setting_travel_acceleration;
		float			setting_min_feedrate_mm_s;
		float			setting_min_travel_feedrate_mm_s;
		millis_t	setting_min_segment_time;
		float			setting_max_jerk[NUM_AXIS];
		float			setting_home_offset[XYZ];
		float			setting_hotend_offset[XYZ][MAX_EXTRUDERS];

		float			setting_z_fade_height;

		float			setting_zprobe_zoffset;

		float			setting_bed_level_matrix[3][3];

		bool			setting_mesh_bilinear_ubl_status;
		uint8_t		setting_mesh_bilinear_max_x;
		uint8_t		setting_mesh_bilinear_max_y;
		int				setting_bilinear_grid_spacing[2];
		int				setting_bilinear_start[2];
		float			setting_mesh_ubl_z_offset;
		int8_t		setting_ubl_eeprom_storage_slot;
	#if ENABLED(AUTO_BED_LEVELING_BILINEAR) || ENABLED(MESH_BED_LEVELING)
		float			setting_mesh_bilinear_z_values[GRID_MAX_POINTS_X][GRID_MAX_POINTS_Y];
	#else
		float			setting_mesh_bilinear_z_values[3][3];
	#endif

		float			setting_endstop_adj[ABC];
		float			setting_delta_radius;
		float			setting_delta_diagonal_rod;
		float			setting_delta_segments_per_second;
		float			setting_delta_calibration_radius;
		float			setting_delta_tower_angle_trim[ABC];

		int				setting_lcd_preheat_hotend_temp[MAX_EXTRUDERS];
		int				setting_lcd_preheat_bed_temp;
		int				setting_lcd_preheat_chamber_temp;
		int				setting_lcd_preheat_fan_speed;

		float			setting_Kpidc[MAX_EXTRUDERS][4];
		int				setting_lpq_len;

		float			setting_bedKpid[3];

		int				setting_lcd_contrast;

		bool			setting_autoretract_enabled;
		float			setting_retract_length;
		float			setting_retract_feedrate_mm_s;
		float			setting_retract_zlift;
		float			setting_retract_recover_length;
		float			setting_retract_recover_feedrate_mm_s;
		float			setting_swap_retract_length;
		float			setting_swap_retract_recover_length;
		float			setting_swap_retract_recover_feedrate_mm_s;

		bool			setting_volumetric_enabled;
		float			setting_filament_size[MAX_EXTRUDERS];

		uint16_t	setting_stepperX_current;
		uint16_t	setting_stepperY_current;
		uint16_t	setting_stepperZ_current;
		uint16_t	setting_stepperX2_current;
		uint16_t	setting_stepperY2_current;
		uint16_t	setting_stepperZ2_current;
		uint16_t	setting_stepperE0_current;
		uint16_t	setting_stepperE1_current;
		uint16_t	setting_stepperE2_current;
		uint16_t	setting_stepperE3_current;
		uint16_t	setting_stepperE4_current;

		float			setting_extruder_advance_k;
		float			setting_advance_ed_ratio;

		uint32_t	setting_motor_current[3];

		int				setting_extruder_auto_fan_speed;
		int				setting_air_fan_speed;

		float			setting_regSN;
		uint32_t	setting_usedTime;

		bool			setting_wifiEnable;
		char 			setting_hostName[24];

		bool			setting_isAccident;
		bool			setting_pauseLeveling;
		float			setting_pausePos[XYZE];
		float			setting_pauseSpeed;
		uint32_t	setting_pauseByte;
		float			setting_lastPos[XYZE];
		int				setting_lastToolsState[TOOLS_NUM];	// T0 T1 T2 T3 T4 Bed Fan Active_T
		char			setting_lastFilename[225];									// max 16 level directory

		bool operator == (const struct_eeprom_data &data) const { return memcmp(this, &data, sizeof(struct_eeprom_data)) ? false : true; };
	} EEPROM_DATA, *EEPROM_DATA_Ptr;

	#define EEPROM_OFFSET 100

	#define EEPROM_WRITE_VAR(address, var) eeprom_write_block((const void *)&(var), (void *)(address), sizeof(var))
	#define EEPROM_UPDATE_VAR(address, var) eeprom_update_block((const void *)&(var), (void *)(address), sizeof(var))
	#define EEPROM_READ_VAR(address, var) eeprom_read_block((void *)&(var), (const void *)(address), sizeof(var))

	#define EEPROM_WRITE_SETTING(setting, var) eeprom_write_block((const void *)&(var), (void *)(offsetof(EEPROM_DATA, setting) + EEPROM_OFFSET), sizeof(((EEPROM_DATA_Ptr) 0) -> setting) )
	#define EEPROM_UPDATE_SETTING(setting, var) eeprom_update_block((const void *)&(var), (void *)(offsetof(EEPROM_DATA, setting) + EEPROM_OFFSET), sizeof(((EEPROM_DATA_Ptr) 0) -> setting) )
	#define EEPROM_READ_SETTING(setting, var) eeprom_read_block((void *)&(var), (void *)(offsetof(EEPROM_DATA, setting) + EEPROM_OFFSET), sizeof(((EEPROM_DATA_Ptr) 0) -> setting) )

	#define EEPROM_WRITE_CONFIG(config) EEPROM_WRITE_VAR(EEPROM_OFFSET, config)
	#define EEPROM_UPDATE_CONFIG(config) EEPROM_UPDATE_VAR(EEPROM_OFFSET, config)
	#define EEPROM_READ_CONFIG(config) EEPROM_READ_VAR(EEPROM_OFFSET, config)

	#define EEPROM_STORE(var, setting) EEPROM_UPDATE_SETTING(setting, var)
	#define STORE_SETTING(var) EEPROM_UPDATE_SETTING(setting_ ## var, var)
#else
	#define EEPROM_STORE(var, setting) NOOP
	#define STORE_SETTING(var) NOOP
#endif // EEPROM_SETTINGS

#endif // CONFIGURATION_STORE_H
