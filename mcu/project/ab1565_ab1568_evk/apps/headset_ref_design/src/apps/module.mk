# Copyright Statement:
#
# (C) 2017  Airoha Technology Corp. All rights reserved.
#
# This software/firmware and related documentation ("Airoha Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
# Without the prior written permission of Airoha and/or its licensors,
# any reproduction, modification, use or disclosure of Airoha Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
# You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
# if you have agreed to and been bound by the applicable license agreement with
# Airoha ("License Agreement") and been granted explicit permission to do so within
# the License Agreement ("Permitted User").  If you are not a Permitted User,
# please cease any access or use of Airoha Software immediately.
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
# AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
#

APPS_SRC = $(APP_PATH)/src/apps
APPS_INC = $(APP_PATH)/inc/apps

C_FILES += $(APPS_SRC)/apps_init.c
# Apps C files
ifeq ($(MTK_FOTA_ENABLE),y)
C_FILES += $(APPS_SRC)/app_fota/app_fota_idle_activity.c
endif

# Pre-proc activity
C_FILES += $(APPS_SRC)/app_preproc/app_preproc_activity.c
C_FILES += $(APPS_SRC)/app_preproc/app_detachable_mic.c
#C_FILES += $(APPS_SRC)/app_preproc/app_preproc_sys_pwr.c

ifneq ($(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE),y)
ifeq ($(MTK_BATTERY_MANAGEMENT_ENABLE),y)
# Battery app
C_FILES += $(APPS_SRC)/app_battery/app_battery_idle_activity.c
C_FILES += $(APPS_SRC)/app_battery/app_battery_transient_activity.c
endif
endif

# BT state service
C_FILES += $(APPS_SRC)/app_bt_state/app_bt_state_service.c

# app service
ifeq ($(AIR_BT_ULTRA_LOW_LATENCY_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_dongle_service
C_FILES += $(APPS_SRC)/app_dongle_service/app_dongle_service.c
endif

# BT takeover service
C_FILES += $(APPS_SRC)/app_bt_state/app_bt_takeover_service.c

ifeq ($(APPS_SLEEP_AFTER_NO_CONNECTION),y)
# Power saving
C_FILES += $(APPS_SRC)/app_power_save/app_power_save_idle_activity.c
C_FILES += $(APPS_SRC)/app_power_save/app_power_save_timeout_activity.c
C_FILES += $(APPS_SRC)/app_power_save/app_power_save_waiting_activity.c
C_FILES += $(APPS_SRC)/app_power_save/app_power_save_utils.c
endif

# multi va
C_FILES += $(APPS_SRC)/app_multi_va/app_multi_va_idle_activity.c
C_FILES += $(APPS_SRC)/app_multi_va/multi_va_manager.c
C_FILES += $(APPS_SRC)/app_multi_va/multi_ble_adv_manager.c

ifeq ($(AIR_BT_ULTRA_LOW_LATENCY_ENABLE),y)
C_FILES += $(APPS_SRC)/app_ull/app_ull_idle_activity.c
ifeq ($(AIR_ONLY_DONGLE_MODE_ENABLE),y)
CFLAGS += -DAIR_ONLY_DONGLE_MODE_ENABLE
endif
ifeq ($(AIR_APP_ULL_GAMING_MODE_UI_ENABLE),y)
CFLAGS += -DAIR_APP_ULL_GAMING_MODE_UI_ENABLE
endif
endif

ifeq ($(AIR_TILE_ENABLE),y)
#tile
C_FILES += $(APPS_SRC)/app_tile/app_tile.c
endif

# Some utils app and home screen app
ifeq ($(SUPPORT_ROLE_HANDOVER_SERVICE),y)
C_FILES += $(APPS_SRC)/app_idle/app_rho_idle_activity.c
endif
C_FILES += $(APPS_SRC)/app_idle/app_home_screen_idle_activity.c
C_FILES += $(APPS_SRC)/app_idle/app_bt_conn_componet_in_homescreen.c
ifeq ($(MTK_ANC_ENABLE), y)
C_FILES += $(APPS_SRC)/app_idle/app_anc_service.c
endif
ifeq ($(AIR_MULTI_POINT_ENABLE), y)
C_FILES += $(APPS_SRC)/app_idle/app_bt_emp_service.c
endif
ifeq ($(AIR_LE_AUDIO_ENABLE), y)
ifeq ($(AIR_MULTI_POINT_ENABLE), y)
C_FILES += $(APPS_SRC)/app_idle/app_bt_conn_manager.c
else
ifeq ($(AIR_LE_AUDIO_MULTIPOINT_ENABLE), y)
C_FILES += $(APPS_SRC)/app_idle/app_bt_conn_manager.c
endif
endif
endif

ifeq ($(AIR_SWIFT_PAIR_ENABLE),y)
CFLAGS += -DAIR_SWIFT_PAIR_ENABLE
C_FILES += $(APPS_SRC)/app_swift_pair/app_swift_pair_idle_activity.c
endif

#in-ear part
C_FILES += $(APPS_SRC)/app_in_ear/app_in_ear_idle_activity.c
C_FILES += $(APPS_SRC)/app_in_ear/app_in_ear_utils.c

#music part
C_FILES += $(APPS_SRC)/app_music/app_music_idle_activity.c
C_FILES += $(APPS_SRC)/app_music/app_music_utils.c
C_FILES += $(APPS_SRC)/app_music/app_music_activity.c

#hfp part
C_FILES += $(APPS_SRC)/app_hfp/app_hfp_idle_activity.c
C_FILES += $(APPS_SRC)/app_hfp/app_hfp_utils.c
C_FILES += $(APPS_SRC)/app_hfp/app_hfp_activity.c
C_FILES += $(APPS_SRC)/app_hfp/app_hfp_va_activity.c

# FIND ME
C_FILES += $(APPS_SRC)/app_fm/app_fm_idle_activity.c
C_FILES += $(APPS_SRC)/app_fm/app_fm_activity.c

# App state report
C_FILES += $(APPS_SRC)/app_state_report/apps_state_report.c

ifeq ($(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE),y)
C_FILES += $(APPS_SRC)/app_master/app_master_idle_activity.c
C_FILES += $(APPS_SRC)/app_master/app_master_transient_activity.c
C_FILES += $(APPS_SRC)/app_master/app_master_utils.c
endif

ifeq ($(findstring y, $(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) $(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)),y)
C_FILES += $(APPS_SRC)/events/apps_race_cmd_co_sys_event.c
endif

# Events folder
C_FILES += $(APPS_SRC)/events/apps_events_audio_event.c
C_FILES += $(APPS_SRC)/events/apps_events_key_event.c
C_FILES += $(APPS_SRC)/events/apps_events_battery_event.c
C_FILES += $(APPS_SRC)/events/apps_events_bt_event.c
C_FILES += $(APPS_SRC)/events/apps_events_test_event.c
C_FILES += $(APPS_SRC)/events/apps_race_cmd_event.c
ifeq ($(AIR_USB_ENABLE), y)
C_FILES += $(APPS_SRC)/events/apps_events_usb_event.c
endif

# Config folder
C_FILES += $(APPS_SRC)/config/apps_config_audio_helper.c
ifeq ($(AIR_ADVANCED_PASSTHROUGH_ENABLE),y)
C_FILES += $(APPS_SRC)/config/app_advance_passthrough.c
endif
C_FILES += $(APPS_SRC)/config/apps_config_key_remapper.c
C_FILES += $(APPS_SRC)/config/apps_config_led_manager.c
C_FILES += $(APPS_SRC)/config/apps_config_vp_manager.c
C_FILES += $(APPS_SRC)/config/apps_config_features_dynamic_setting.c

# LED
C_FILES += $(APPS_SRC)/led/app_led_control.c
C_FILES += $(APPS_SRC)/led/led_control_internal.c
C_FILES += $(APPS_SRC)/led/led_control_style_cfg.c

# FAST_PAIR
ifeq ($(MTK_BT_FAST_PAIR_ENABLE),y)
ifeq ($(AIR_SPOT_ENABLE),y)
CFLAGS += -DAIR_SPOT_ENABLE
endif
C_FILES += $(APPS_SRC)/app_fast_pair/app_fast_pair_idle_activity.c
endif

# SMART_CHARGER
ifeq ($(MTK_SMART_CHARGER_ENABLE),y)
C_FILES += $(APPS_SRC)/app_smart_charger/app_smcharger_idle_activity.c
C_FILES += $(APPS_SRC)/app_smart_charger/app_smcharger_lid_close_activity.c
C_FILES += $(APPS_SRC)/app_smart_charger/app_smcharger_lid_open_activity.c
C_FILES += $(APPS_SRC)/app_smart_charger/app_smcharger_off_activity.c
C_FILES += $(APPS_SRC)/app_smart_charger/app_smcharger_out_of_case_activity.c
C_FILES += $(APPS_SRC)/app_smart_charger/app_smcharger_startup_activity.c
C_FILES += $(APPS_SRC)/app_smart_charger/app_smcharger_utils.c
endif

ifeq ($(GSOUND_LIBRARY_ENABLE),y)
C_FILES += $(APPS_SRC)/app_gsound/app_gsound_idle_activity.c
endif


# Utils
C_FILES += $(APPS_SRC)/utils/apps_debug.c
C_FILES += $(APPS_SRC)/utils/apps_atcmd.c
ifeq ($(MTK_AWS_MCE_ENABLE),y)
C_FILES += $(APPS_SRC)/utils/apps_aws_sync_event.c
endif
C_FILES += $(APPS_SRC)/utils/apps_control_touch_key_status.c
C_FILES += $(APPS_SRC)/utils/apps_dongle_sync_event.c

#voice_prompt
ifeq ($(MTK_PROMPT_SOUND_ENABLE),y)
C_FILES +=   $(APPS_SRC)/vp/app_voice_prompt.c
C_FILES +=   $(APPS_SRC)/vp/app_voice_prompt_nvdm.c
endif

#ama
ifeq ($(MTK_AMA_ENABLE),y)
C_FILES +=   $(APPS_SRC)/app_ama/app_ama_idle_activity.c
C_FILES +=   $(APPS_SRC)/app_ama/app_ama_activity.c
C_FILES +=   $(APPS_SRC)/app_ama/app_ama_audio.c
C_FILES +=   $(APPS_SRC)/app_ama/app_ama_multi_va.c
CFLAGS  += -DMTK_AMA_ENABLE

# Add AMA iAP2 support feature option
ifeq ($(AMA_IAP2_SUPPORT_ENABLE),y)
CFLAGS  += -DAMA_IAP2_SUPPORT_ENABLE
endif

# Add app relay enable (for Spotify)
ifeq ($(AMA_IAP2_APP_RELAY_ENABLE),y)
CFLAGS  += -DAMA_IAP2_APP_RELAY_ENABLE
endif

# Configure AMA iAP2 via MUX
ifeq ($(AMA_IAP2_VIA_MUX_ENABLE),y)
CFLAGS  += -DAMA_IAP2_VIA_MUX_ENABLE
endif

# Configure AMA trigger mode
ifeq ($(AMA_TRIGGER_MODE_TTT_ENABLE),y)
CFLAGS  += -DAMA_TRIGGER_MODE_TTT_ENABLE
endif

ifeq ($(AMA_TRIGGER_MODE_PTT_ENABLE),y)
CFLAGS  += -DAMA_TRIGGER_MODE_PTT_ENABLE
endif

ifeq ($(AMA_TRIGGER_MODE_WWD_ENABLE),y)
CFLAGS  += -DAMA_TRIGGER_MODE_WWD_ENABLE
endif

ifeq ($(AIR_AMA_SIDETONE_ENABLE),y)
CFLAGS  += -DAIR_AMA_SIDETONE_ENABLE
endif
endif # AMA end

# Add xiaowei VA activity support
ifeq ($(MTK_VA_XIAOWEI_ENABLE), y)
C_FILES += $(APPS_SRC)/app_va_xiaowei/app_va_xiaowei_activity.c
C_FILES += $(APPS_SRC)/app_va_xiaowei/app_va_xiaowei_multi_support.c
C_FILES += $(APPS_SRC)/app_va_xiaowei/app_va_xiaowei_transient_activity.c
endif

# Add xiaoai VA activity support
ifeq ($(MTK_VA_XIAOAI_ENABLE), y)
C_FILES += $(APPS_SRC)/app_va_xiaoai/app_va_xiaoai_activity.c
C_FILES += $(APPS_SRC)/app_va_xiaoai/app_va_xiaoai_ble_adv.c
C_FILES += $(APPS_SRC)/app_va_xiaoai/app_va_xiaoai_bt_helper.c
C_FILES += $(APPS_SRC)/app_va_xiaoai/app_va_xiaoai_device_config.c
C_FILES += $(APPS_SRC)/app_va_xiaoai/app_va_xiaoai_hfp_at_cmd.c
ifeq ($(MTK_XIAOAI_PTT_ENABLE), y)
C_FILES += $(APPS_SRC)/app_va_xiaoai/app_va_xiaoai_ptt_speech_activity.c
endif
endif

ifeq ($(AIR_MS_TEAMS_ENABLE), y)
CFLAGS  += -DAIR_MS_TEAMS_ENABLE
C_FILES += $(APPS_SRC)/app_ms_teams/app_ms_teams_idle_activity.c
C_FILES += $(APPS_SRC)/app_ms_teams/app_ms_teams_activity.c
C_FILES += $(APPS_SRC)/app_ms_teams/app_ms_teams_utils.c
endif

# leakage detection
ifeq ($(MTK_LEAKAGE_DETECTION_ENABLE), y)
C_FILES += $(APPS_SRC)/app_leakage_detection/app_leakage_detection_idle_activity.c
endif

# Add for line_in feature
ifeq ($(APPS_LINE_IN_SUPPORT), y)
CFLAGS += -DAPPS_LINE_IN_SUPPORT
ifeq ($(AIR_LINE_IN_MIX_ENABLE), y)
CFLAGS += -DAIR_LINE_IN_MIX_ENABLE
endif
ifeq ($(AIR_LINE_OUT_ENABLE), y)
CFLAGS += -DAIR_LINE_OUT_ENABLE
endif
C_FILES += $(APPS_SRC)/app_line_in/app_line_in_idle_activity.c
C_FILES += $(APPS_SRC)/app_line_in/app_line_in_utils.c
endif

#usb audio
ifeq ($(APPS_USB_AUDIO_SUPPORT),y)
C_FILES +=   $(APPS_SRC)/app_usb_audio/app_usb_audio_idle_activity.c
C_FILES +=   $(APPS_SRC)/app_usb_audio/app_usb_audio_activity.c
C_FILES +=   $(APPS_SRC)/app_usb_audio/app_usb_audio_utils.c
endif

# CFU
ifeq ($(AIR_CFU_ENABLE), y)
C_FILES += $(APPS_SRC)/app_ms_cfu/app_ms_cfu_idle_activity.c
endif

# Include bt sink path
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)
ifeq ($(MTK_BATTERY_MANAGEMENT_ENABLE),y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_battery
endif
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_bt_state
ifeq ($(MTK_FOTA_ENABLE),y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_fota
endif
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_gsound
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_hfp
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_idle
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_ull
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_multi_va
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/config
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/events
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/utils
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_music
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/led
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_power_save
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_preproc
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_fm
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_fast_pair
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/vp
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_smart_charger
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_ama
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_in_ear
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_leakage_detection
ifeq ($(AIR_TILE_ENABLE),y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_tile
endif
ifeq ($(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE),y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_master
endif
ifeq ($(AIR_SWIFT_PAIR_ENABLE),y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_swift_pair
endif

# Add xiaowei VA activity support
ifeq ($(MTK_VA_XIAOWEI_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_va_xiaowei
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/xiaowei/inc
endif

# Add xiaoai VA activity support
ifeq ($(MTK_VA_XIAOAI_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_va_xiaoai
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/xiaoai/inc
endif
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_state_report

#APP LE AUDIO
ifeq ($(AIR_LE_AUDIO_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_le_audio
endif

# Add MS Teams support
ifeq ($(AIR_MS_TEAMS_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_ms_teams
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/ms_teams/inc
endif

# Add MS CFU support
ifeq ($(AIR_CFU_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_ms_cfu
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/ms_cfu/inc
endif

#APPs features

##
## APPS_DISABLE_BT_WHEN_CHARGING
## Brief:       This option is used to enable the feature that disabling BT if device is charging.
## Usage:       Enable the feature by configuring it as y.
## Path:        project
## Dependency:  None
## Notice:      Do not enable the feature if CCASE_ENABLE is y
## Relative doc:None
##
ifeq ($(APPS_DISABLE_BT_WHEN_CHARGING), y)
CFLAGS += -DAPPS_DISABLE_BT_WHEN_CHARGING
endif

##
## APPS_AUTO_TRIGGER_RHO
## Brief:       This option is used to enable the feature that triggers RHO before power off, disable BT and battery level of agent is lower than partner.
## Usage:       Enable the feature by configuring it as y.
## Path:        project
## Dependency:  SUPPORT_ROLE_HANDOVER_SERVICE
## Notice:      Do not enable APPS_TRIGGER_RHO_BY_KEY and APPS_AUTO_TRIGGER_RHO in one project
## Relative doc:None
##
ifeq ($(APPS_AUTO_TRIGGER_RHO), y)
# Do RHO when agent low battery, power off or disable BT
CFLAGS += -DAPPS_AUTO_TRIGGER_RHO
endif

##
## APPS_TRIGGER_RHO_BY_KEY
## Brief:       This option is used to enable the feature trigger RHO user presses key.
## Usage:       Enable the feature by configuring it as y.
## Path:        project
## Dependency:  SUPPORT_ROLE_HANDOVER_SERVICE
## Notice:      Do not enable APPS_TRIGGER_RHO_BY_KEY and APPS_AUTO_TRIGGER_RHO in one project
## Relative doc:None
##
ifeq ($(APPS_TRIGGER_RHO_BY_KEY), y)
# Do RHO press key on partner
CFLAGS += -DAPPS_TRIGGER_RHO_BY_KEY
endif

##
## APPS_SLEEP_AFTER_NO_CONNECTION
## Brief:       This option is used to enable the feature start a timer to sleep when device doesn't connect to smart phone.
## Usage:       Enable the feature by configuring it as y.
## Path:        project
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(APPS_SLEEP_AFTER_NO_CONNECTION), y)
# Press key to do air pairing
CFLAGS += -DAPPS_SLEEP_AFTER_NO_CONNECTION
endif

##
## AIR_TILE_ENABLE
## Brief:       This option is used to enable the feature to start or change the advertising of Tile.
## Usage:       Enable the feature by configuring it as y.
## Path:        project
## Dependency:  None
## Notice:      None
## Relative doc:None
##
ifeq ($(AIR_TILE_ENABLE), y)
CFLAGS += -DAIR_TILE_ENABLE
endif

# app gsound
ifeq ($(GSOUND_LIBRARY_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_gsound
C_FILES += $(APPS_SRC)/app_gsound/app_gsound_service.c
C_FILES += $(APPS_SRC)/app_gsound/app_gsound_multi_va.c
endif

# Multi Voice assistant
ifeq ($(MULTI_VA_SUPPORT_COMPETITION), y)
CFLAGS += -DMULTI_VA_SUPPORT_COMPETITION
endif

ifeq ($(MTK_IN_EAR_FEATURE_ENABLE), y)
CFLAGS += -DMTK_IN_EAR_FEATURE_ENABLE
endif

ifeq ($(MTK_LEAKAGE_DETECTION_ENABLE), y)
CFLAGS += -DMTK_LEAKAGE_DETECTION_ENABLE
endif

ifeq ($(APPS_LINE_IN_SUPPORT), y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_line_in
ifeq ($(LINE_IN_ENABLE_WITH_LOW_LEVEL), y)
CFLAGS += -DLINE_IN_ENABLE_WITH_LOW_LEVEL
endif
ifeq ($(LINE_IN_DET_WITH_HIGH_LEVEL), y)
CFLAGS += -DLINE_IN_DET_WITH_HIGH_LEVEL
endif
# Pressing key to do power off only disable BT when Line in. If system on is triggered by line in, do not enable BT.
ifeq ($(AIR_APP_SYSTEM_ON_BY_LINE_IN_ENABLE),y)
CFLAGS += -DAIR_APP_SYSTEM_ON_BY_LINE_IN_ENABLE
endif
endif

#usb audio
ifeq ($(APPS_USB_AUDIO_SUPPORT),y)
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_usb_audio
CFLAGS += -DAPPS_USB_AUDIO_SUPPORT
ifeq ($(AIR_USB_HID_MEDIA_CTRL_ENABLE),y)
CFLAGS += -DAIR_USB_HID_MEDIA_CTRL_ENABLE
endif
ifeq ($(AIR_USB_AUDIO_MIX_ENABLE),y)
CFLAGS += -DAIR_USB_AUDIO_MIX_ENABLE
endif
ifeq ($(AIR_USB_AUDIO_OUT_ENABLE),y)
CFLAGS += -DAIR_USB_AUDIO_OUT_ENABLE
endif
ifeq ($(AIR_USB_IN_PORT2_ENABLE),y)
CFLAGS += -DAIR_USB_IN_PORT2_ENABLE
endif
endif

#usb audio
ifeq ($(AIR_SLIDE_POWER_KEY_ENABLE),y)
CFLAGS += -DAIR_SLIDE_POWER_KEY_ENABLE
endif

# app leaudio
ifeq ($(AIR_LE_AUDIO_ENABLE), y)
C_FILES += $(APPS_SRC)/app_le_audio/app_le_audio.c
C_FILES += $(APPS_SRC)/app_le_audio/app_mcp.c
C_FILES += $(APPS_SRC)/app_le_audio/app_ccp.c
C_FILES += $(APPS_SRC)/app_le_audio/app_le_audio_hfp_at_cmd.c
C_FILES += $(APPS_SRC)/app_le_audio/app_le_audio_aird_client.c
ifeq ($(AIR_LE_AUDIO_BIS_ENABLE), y)
C_FILES += $(APPS_SRC)/app_le_audio/app_le_audio_bis.c
C_FILES += $(APPS_SRC)/app_le_audio/app_le_audio_bis_activity.c
endif
endif

CFLAGS += -DTEMP_CLASSIC_BT_OFF

#detachable mic
ifeq ($(AIR_AUDIO_DETACHABLE_MIC_ENABLE),y)
CFLAGS += -DAIR_AUDIO_DETACHABLE_MIC_ENABLE
endif

# Add MS XBOX UI
ifneq ($(AIR_BT_ULTRA_LOW_LATENCY_ENABLE), y)
# If not define ULL, GIP should not enable
AIR_MS_GIP_ENABLE = n
endif # AIR_BT_ULTRA_LOW_LATENCY_ENABLE

ifeq ($(AIR_MS_GIP_ENABLE), y)
CFLAGS += -DAIR_MS_GIP_ENABLE
CFLAGS += -I$(SOURCE_DIR)/$(APPS_INC)/app_ms_xbox
C_FILES += $(APPS_SRC)/app_ms_xbox/app_ms_xbox_idle_activity.c
endif # AIR_MS_GIP_ENABLE
