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
IC_TYPE                               = ab1565

# debug level: none, error, warning, info and debug
MTK_DEBUG_LEVEL                       = info
MTK_NVDM_ENABLE                       = y
# let syslog dump to flash
MTK_SAVE_LOG_TO_FLASH_ENABLE          = n

MTK_USB_DEMO_ENABLED                  = n
MTK_USB_AUDIO_V1_ENABLE               = n
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
MTK_AWS_MCE_ENABLE                  = y
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

#AT Command
MTK_AT_COMMAND                        = y
MTK_SMT_AUDIO_TEST                    = n

#FOTA Related
MTK_RACE_CMD_ENABLE                 = y
MTK_RACE_DUAL_CMD_ENABLE            = y
MTK_FOTA_ENABLE                     = y
MTK_FOTA_VIA_RACE_CMD               = y
MTK_FOTA_VIA_RACE_CMD_DUAL_DEVICES  = y
MTK_MBEDTLS_CONFIG_FILE             = config-mtk-fota-race-cmd.h

MTK_PORT_SERVICE_BT_ENABLE          = y
MTK_AIRUPDATE_ENABLE                = y
MTK_SWITCH_TO_RACE_COMMAND_ENABLE   = y
MTK_RACE_FIND_ME_ENABLE             = y

# prompt sound
MTK_PROMPT_SOUND_ENABLE	            = y
MTK_PROMPT_SOUND_SYNC_ENABLE        = n
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

# LINEIN PEQ
MTK_LINEIN_PEQ_ENABLE               = n

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
SUPPORT_ROLE_HANDOVER_SERVICE       = y
endif

ifeq ($(MTK_AWS_MCE_ENABLE),y)
MTK_AWS_MCE_ROLE_RECOVERY_ENABLE = y
endif

# boot reason check
MTK_BOOTREASON_CHECK_ENABLE         = y
MTK_BT_FAST_PAIR_ENABLE             = y
MTK_MINIDUMP_ENABLE                 = n
MTK_FULLDUMP_ENABLE                 = y

# ANC module
MTK_ANC_ENABLE = n
ifeq ($(MTK_ANC_ENABLE), y)
ANC_SOURCE_CODE = $(strip $(SOURCE_DIR))/../mcu/middleware/MTK/audio/anc_control_protected/GCC/module.mk
ifeq ($(ANC_SOURCE_CODE), $(wildcard $(ANC_SOURCE_CODE)))
MTK_ANC_LIB_ENABLE =  n
else
MTK_ANC_LIB_ENABLE =  y
endif
endif
ifeq ($(MTK_ANC_ENABLE),y)
MTK_ANC_BACKUP_STATUS_ENABLE        = n
MTK_HYBRID_ANC_ENABLE               = y
ifeq ($(MTK_HYBRID_ANC_ENABLE),y)
MTK_POST_PEQ_DEFAULT_ON             = n
MTK_VOICE_ANC_EQ                    = n
MTK_DEQ_ENABLE                      = n
endif
endif

# Race relay cmd
ifeq ($(MTK_AWS_MCE_ENABLE),y)
MTK_RACE_RELAY_CMD_ENABLE = y
endif

# APPs features
ifneq ($(CCASE_ENABLE),y)
APPS_DISABLE_BT_WHEN_CHARGING 		= y
#Enable BT when charger out, Disable BT when Charger in
endif

APPS_SLEEP_AFTER_NO_CONNECTION		= y

#define atci tx buffer slim
MTK_ATCI_BUFFER_SLIM         = y

# GSOUND LIBRARY ENABLE
GSOUND_LIB = $(strip $(SOURCE_DIR))/middleware/third_party/gsound/module.mk
ifeq ($(GSOUND_LIB), $(wildcard $(GSOUND_LIB)))
GSOUND_LIBRARY_ENABLE               = y
endif

ifeq ($(GSOUND_LIBRARY_ENABLE), y)
# BISTO_NOT_ENABLE_AM_RECORD
MTK_AM_NOT_SUPPORT_STREAM_IN        = y
MTK_SBC_ENCODER_ENABLE              = y
endif

ifeq ($(SUPPORT_ROLE_HANDOVER_SERVICE), y)
# Do RHO when agent low battery, power off or disable BT
APPS_AUTO_TRIGGER_RHO       		= y
# Do RHO press key on partner
APPS_TRIGGER_RHO_BY_KEY     		= y
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
MTK_AUDIO_SUPPORT_MULTIPLE_MICROPHONE = n

# Use HWSRC to do clkskew
ENABLE_HWSRC_CLKSKEW = y

