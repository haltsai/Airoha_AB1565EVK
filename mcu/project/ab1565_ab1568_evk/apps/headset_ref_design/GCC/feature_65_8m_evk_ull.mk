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

IC_CONFIG                             = ab156x
BOARD_CONFIG                          = ab156x_evb
IC_TYPE                               = ab1565
BOARD_TYPE                            = ab1565_8m_evk_ull
BL_FEATURE                            = feature_65_evb.mk
PRODUCT_CATEGORY                      = AB1565

FLASH_SIZE_8M						  = y
# debug level: none, error, warning, info and debug
MTK_DEBUG_LEVEL                       = info
MTK_NVDM_ENABLE                       = y

MTK_USB_DEMO_ENABLED                  = y
MTK_USB_AUDIO_V1_ENABLE               = y
MTK_USB_AUDIO_V2_ENABLE               = n
MTK_USB_AUDIO_MICROPHONE              = n

MTK_HAL_EXT_32K_ENABLE                = n
MTK_NO_PSRAM_ENABLE = y
# heap dump
MTK_SUPPORT_HEAP_DEBUG                = y
MTK_SUPPORT_HEAP_DEBUG_ADVANCED       = n
# heap peak profiling
MTK_HEAP_SIZE_GUARD_ENABLE            = n
#SWLA
MTK_SWLA_ENABLE                       = y
#system service at cmd
MTK_SYSTEM_AT_COMMAND_ENABLE          = y
MTK_OS_CPU_UTILIZATION_ENABLE         = y
# system hang debug: none, y, o1, o2 and mp
MTK_SYSTEM_HANG_TRACER_ENABLE         = y
###################################################
# bt at command
MTK_BT_AT_COMMAND_ENABLE = y

# CAPID store to NVDM
MTK_CAPID_IN_NVDM_AT_COMMAND_ENABLE = y

# port service
MTK_PORT_SERVICE_ENABLE = y

# bt codec enable
MTK_BT_CODEC_ENABLED = y
# bt codec ble enable
AIR_BT_CODEC_BLE_ENABLED = n

# BT A2DP codec AAC support
MTK_BT_A2DP_AAC_ENABLE = y

# BT A2DP codec vendor support
MTK_BT_A2DP_VENDOR_ENABLE = n

# avm direct feature
MTK_AVM_DIRECT                       = y

# BT Dual mode
MTK_BT_DUO_ENABLE = y

# bt module enable
MTK_BT_ENABLE                       = y
MTK_BLE_ONLY_ENABLE                 = n
MTK_BT_HFP_ENABLE                   = y
MTK_BT_HSP_ENABLE                   = y
MTK_BT_AVRCP_ENABLE                 = y
MTK_BT_AVRCP_ENH_ENABLE             = y
MTK_BT_A2DP_ENABLE                  = y
MTK_BT_PBAP_ENABLE                  = n
MTK_BT_SPP_ENABLE                   = y
# aws earbuds feature
MTK_AWS_MCE_ENABLE                  = n
MTK_MUX_AWS_MCE_ENABLE      = n

MTK_BT_AVRCP_BROWSE_ENABLE          = y
# HCI log output mixed with system log.
MTK_HCI_LOG_MIX_WITH_SYSLOG         = y

#BT external timer
MTK_BT_TIMER_EXTERNAL_ENABLE = y
MTK_PORT_SERVICE_ENABLE               = y

MTK_ATCI_VIA_PORT_SERVICE           = n
MTK_ATCI_VIA_MUX                    = y

#MTK BATTERY CONTROL
MTK_BATTERY_MANAGEMENT_ENABLE		  = y
MTK_BATTERY_EOC_ENTER_RTC		      = n


#AT Command
MTK_AT_COMMAND                        = y
MTK_SMT_AUDIO_TEST                    = n

#FOTA Related
MTK_RACE_CMD_ENABLE                 = y
MTK_RACE_DUAL_CMD_ENABLE            = n
MTK_FOTA_ENABLE                     = y
MTK_FOTA_VIA_RACE_CMD               = y
MTK_FOTA_VIA_RACE_CMD_DUAL_DEVICES  = n
MTK_MBEDTLS_CONFIG_FILE             = config-mtk-fota-race-cmd.h

MTK_PORT_SERVICE_BT_ENABLE          = y
MTK_AIRUPDATE_ENABLE                = y
MTK_SWITCH_TO_RACE_COMMAND_ENABLE   = y
MTK_RACE_FIND_ME_ENABLE             = y

# prompt sound
MTK_PROMPT_SOUND_ENABLE	            = y

MTK_AUDIO_AT_CMD_PROMPT_SOUND_ENABLE          = n

# mp3
MTK_AUDIO_MP3_ENABLED               = n
MTK_MP3_DECODER_ENABLED             = y
MTK_MP3_CODEC_TASK_DEDICATE         = y
MTK_MP3_STEREO_SUPPORT              = y
# wave decoder by charlie
MTK_WAV_DECODER_ENABLE              = n
# record middleware
MTK_RECORD_ENABLE                   = y
# audio dump
MTK_AUDIO_DUMP_ENABLE               = y
# PEQ
MTK_PEQ_ENABLE                      = y

# linein playback
MTK_LINEIN_PLAYBACK_ENABLE          = y
MTK_PURE_LINEIN_PLAYBACK_ENABLE     = n
# LINEIN PEQ
MTK_LINEIN_PEQ_ENABLE               = y
# LINEIN INS
MTK_LINEIN_INS_ENABLE               = y

# AUDIO LOOPBACK TEST module
MTK_AUDIO_LOOPBACK_TEST_ENABLE = y
MTK_AUDIO_TRANSMITTER_ENABLE = y

# AirDump module
MTK_AIRDUMP_EN                      = y

# amp dc compensation
MTK_AMP_DC_COMPENSATION_ENABLE      = y
DC_COMPENSATION_PARA_ENABLE         = y

MTK_SYSTEM_AT_COMMAND_ENABLE        = y

# NVDM gain setting table
MTK_AUDIO_GAIN_TABLE_ENABLE         = y

# BT controller debug port
MTK_BT_PKA_IDS_ENABLE               = n
ifeq ($(MTK_BT_PKA_IDS_ENABLE),y)
EINT_KEY_ENABLED                    = n
else
EINT_KEY_ENABLED                    = y
endif
ifeq ($(MTK_AWS_MCE_ENABLE),y)
SUPPORT_ROLE_HANDOVER_SERVICE       = n
endif

ifeq ($(MTK_AWS_MCE_ENABLE),y)
MTK_AWS_MCE_ROLE_RECOVERY_ENABLE = n
endif

# boot reason check
MTK_BOOTREASON_CHECK_ENABLE         = y
MTK_BT_FAST_PAIR_ENABLE             = y
MTK_MINIDUMP_ENABLE                 = y
MTK_FULLDUMP_ENABLE                 = y

# ANC module
MTK_ANC_ENABLE = y
ifeq ($(MTK_ANC_ENABLE), y)
ANC_SOURCE_CODE = $(strip $(SOURCE_DIR))/../mcu/middleware/MTK/audio/anc_control_protected/GCC/module.mk
ifeq ($(ANC_SOURCE_CODE), $(wildcard $(ANC_SOURCE_CODE)))
MTK_ANC_LIB_ENABLE =  n
else
MTK_ANC_LIB_ENABLE =  y
endif
endif

# Race relay cmd
ifeq ($(MTK_AWS_MCE_ENABLE),y)
MTK_RACE_RELAY_CMD_ENABLE = y
endif

# APPs features
ifneq ($(CCASE_ENABLE),y)
APPS_DISABLE_BT_WHEN_CHARGING 		= n
#Enable BT when charger out, Disable BT when Charger in
endif

APPS_SLEEP_AFTER_NO_CONNECTION		= y

# BLE GAP SRV features
MTK_BLE_GAP_SRV_ENABLE       = y

#define atci tx buffer slim
MTK_ATCI_BUFFER_SLIM         = y

# GSOUND LIBRARY ENABLE
GSOUND_LIB = $(strip $(SOURCE_DIR))/middleware/third_party/gsound/module.mk
ifeq ($(GSOUND_LIB), $(wildcard $(GSOUND_LIB)))
GSOUND_LIBRARY_ENABLE               = n
endif

ifeq ($(GSOUND_LIBRARY_ENABLE), y)
# BISTO_NOT_ENABLE_AM_RECORD
MTK_AM_NOT_SUPPORT_STREAM_IN        = y
MTK_SBC_ENCODER_ENABLE              = y
GSOUND_HOTWORD_ENABLE               = y
ifeq ($(MTK_AWS_MCE_ENABLE), n)
GSOUND_ENABLE_SINGLE_MODE = y
endif
endif

ifeq ($(SUPPORT_ROLE_HANDOVER_SERVICE), y)
# Do RHO when agent low battery, power off or disable BT
APPS_AUTO_TRIGGER_RHO       		= n
# Do RHO press key on partner
APPS_TRIGGER_RHO_BY_KEY     		= n
endif

# Add IAP2 profile support
# If exist the iap2 module.mk file, check the mfi_coprocessor module.mk file exist or not
# if the both module.mk exist, means the MTK_IAP2_PROFILE_ENABLE can be enabled.
IAP2_CODE = $(strip $(SOURCE_DIR))/middleware/MTK/iap2/GCC/module.mk
MFI_COPROCESSOR_CODE = $(strip $(SOURCE_DIR))/middleware/MTK/mfi_coprocessor/module.mk
ifeq ($(IAP2_CODE), $(wildcard $(IAP2_CODE)))
ifeq ($(MFI_COPROCESSOR_CODE), $(wildcard $(MFI_COPROCESSOR_CODE)))
MTK_IAP2_PROFILE_ENABLE 			= y
MTK_MFI_COPROCESSOR_ENABLE 			= y
MTK_IAP2_VIA_MUX_ENABLE = y
endif
endif

# Add AMA support
AMA_LIB = $(strip $(SOURCE_DIR))/middleware/MTK/ama/module.mk
ifeq ($(AMA_LIB), $(wildcard $(AMA_LIB)))
MTK_AMA_ENABLE = n

# Configure AMA trigger mode
# TTT : Tap-To-Talk
# PTT : Push-To-Talk
# WWD : Wake-Word-Detection
AMA_TRIGGER_MODE_TTT_ENABLE = y
AMA_TRIGGER_MODE_PTT_ENABLE = y
AMA_TRIGGER_MODE_WWD_ENABLE = y

# Configure AMA side tone
AIR_AMA_SIDETONE_ENABLE = y

ifeq ($(MTK_IAP2_PROFILE_ENABLE),y)
AMA_IAP2_SUPPORT_ENABLE = y
AMA_IAP2_APP_RELAY_ENABLE = n

ifeq ($(MTK_IAP2_VIA_MUX_ENABLE),y)
AMA_IAP2_VIA_MUX_ENABLE = y
endif #MTK_IAP2_VIA_MUX_ENABLE
endif #MTK_IAP2_PROFILE_ENABLE
endif #AMA

# Add VA model language manager support
VA_MM_MODULE = $(strip $(SOURCE_DIR))/middleware/MTK/va_model_manager/module.mk
AMA_MODEL_FOLDER = $(SOURCE_DIR)/prebuilt/middleware/third_party/language_model/ama
GVA_MODEL_FOLDER = $(SOURCE_DIR)/prebuilt/middleware/third_party/language_model/gsound/ab156x
ifeq ($(VA_MM_MODULE), $(wildcard $(VA_MM_MODULE)))
ifneq ("$(wildcard $(AMA_MODEL_FOLDER))", "")
MTK_VA_MODEL_MANAGER_ENABLE = n
endif
ifneq ("$(wildcard $(GVA_MODEL_FOLDER))", "")
MTK_VA_MODEL_MANAGER_ENABLE = n
else
GSOUND_HOTWORD_ENABLE = n
endif
endif

MULTI_VA_SUPPORT_COMPETITION  		= y

MTK_AUDIO_CODEC_MANAGER_ENABLE = y

# Add opus encode support
MTK_OPUS_ENCODER_ENABLE				= y

# Record opus encoder
MTK_RECORD_OPUS_ENABLE = n

# Enable DSP1 DRAM for DSP0 memory pool
MTK_DSP1_DRAM_FOR_DSP0_POOL_ENABLE = n

MTK_MEMORY_MONITOR_ENABLE           = n

# bluetooth connection manager feature support.
MTK_BT_CM_SUPPORT                   = y

MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT  = y

# 2a2d
MTK_AUDIO_SUPPORT_MULTIPLE_MICROPHONE = y

#define LE Audio
LE_AUDIO_LIB = $(strip $(SOURCE_DIR))/middleware/MTK/bt_le_audio/module.mk
ifeq  ($(LE_AUDIO_LIB), $(wildcard $(LE_AUDIO_LIB)))
AIR_LE_AUDIO_ENABLE                             = n
endif


# Tickless
MTK_TICKLESS_USE_SW_GPT = n

# Use HWSRC to do clkskew
ENABLE_HWSRC_CLKSKEW = y

# For 3rd party NR
ifneq ($(wildcard $(strip $(SOURCE_DIR))/../dsp/prebuilt/middleware/third_party/dspalg/igo_nr/),)
AIR_3RD_PARTY_NR_ENABLE = y
else
AIR_3RD_PARTY_NR_ENABLE = n
endif

# For 3rd party NR
MTK_3RD_PARTY_NR = n

MTK_MUX_BT_ENABLE = y

# BT Role Handover with SPP and BLE link
BT_ROLE_HANDOVER_WITH_SPP_BLE = n

#HW IO config enhance
MTK_AUDIO_HW_IO_CONFIG_ENHANCE = y

#Gain setting enhance
MTK_AUDIO_GAIN_SETTING_ENHANCE = y

APPS_LINE_IN_SUPPORT = y
LINE_IN_ENABLE_WITH_LOW_LEVEL = y

# Enable usb audio enable
APPS_USB_AUDIO_SUPPORT             = y

# usb audio play enable
MTK_USB_AUDIO_PLAYBACK_ENABLE      = y

# UI shell max activity count in activity stack.
MTK_UI_SHELL_MAX_ACTIVITY_COUNT	= 40

MTK_CONN_VP_SYNC_ENABLE = n

# Leakage detection
ifeq ($(MTK_ANC_ENABLE), y)
MTK_LEAKAGE_DETECTION_ENABLE     = n
endif

#Fix afe rate to 48KHZ
FIXED_SAMPLING_RATE_TO_48KHZ = y

# Ultra low latency
ULL_LIB = $(strip $(SOURCE_DIR))/middleware/MTK/bt_ultra_low_latency/module.mk
ifeq ($(ULL_LIB), $(wildcard $(ULL_LIB)))
AIR_BT_ULTRA_LOW_LATENCY_ENABLE = y
endif

# Enable multi point
AIR_MULTI_POINT_ENABLE = y

AIR_LINE_IN_ENABLE = y
AIR_LINE_IN_WITH_LOW_LEVEL_ENABLE = y

#Silence Detection enable
AIR_SILENCE_DETECTION_ENABLE = y

#Microsoft Swift Pair
AIR_SWIFT_PAIR_ENABLE = n

# ULL voice uplink low latency (AFE buffering 7.5ms)
AIR_ULL_VOICE_LOW_LATENCY_ENABLE = y
