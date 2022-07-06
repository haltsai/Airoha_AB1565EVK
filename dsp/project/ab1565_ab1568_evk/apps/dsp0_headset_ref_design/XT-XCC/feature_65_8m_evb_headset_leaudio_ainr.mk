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
BOARD_TYPE                            = ab1565_evb

# Using specific linker script during linking process.
LINKER_SCRIPT_FILE                    = dsp0_flash_8m.lcf

# debug level: none, error, warning, info and debug
MTK_DEBUG_LEVEL                       = info

# let syslog dump to flash
MTK_SAVE_LOG_TO_FLASH_ENABLE          = n

# heap peak profiling
MTK_HEAP_SIZE_GUARD_ENABLE            = n

CCCI_ENABLE                           = n
###################################################
# bt at command
MTK_BT_AT_COMMAND_ENABLE = y

# port service
MTK_PORT_SERVICE_ENABLE = y

#SWLA
MTK_SWLA_ENABLE                       = y
# heap dump
MTK_SUPPORT_HEAP_DEBUG                = y
#cpu mips profiling
AIR_CPU_MCPS_PRIORING_ENABLE          = y

MTK_USB_DEMO_ENABLED                  = n

#NVDM feature
MTK_NVDM_ENABLE = y

# bt codec enable
MTK_BT_CODEC_ENABLED = y

# BT A2DP codec AAC support
MTK_BT_A2DP_AAC_ENABLE = y

# BT A2DP codec vendor support
VENDOR_LIB = $(strip $(ROOTDIR))/prebuilt/middleware/third_party/dspalg/vendor_decoder/vend.flag
ifeq ($(VENDOR_LIB), $(wildcard $(VENDOR_LIB)))
MTK_BT_A2DP_VENDOR_ENABLE =  n
MTK_BT_A2DP_VENDOR_BC_ENABLE = n
endif



# BT A2DP codec vendor 1 support
MTK_BT_A2DP_VENDOR_1_ENABLE = n
MTK_BT_A2DP_VENDOR_1_USE_PIC = n

# Load Clk Skew Lib From Source Code or Object File
CLKSKEW_LIB = $(strip $(ROOTDIR))/middleware/MTK/dspalg/clk_skew_protected/module.mk
ifeq ($(CLKSKEW_LIB), $(wildcard $(CLKSKEW_LIB)))
MTK_BT_CLK_SKEW_LOAD_ENABLE =  y
else
MTK_BT_CLK_SKEW_LOAD_ENABLE =  n
endif

# BT A2DP codec SRC support
MTK_SRC_ENABLE = n

# avm direct feature
MTK_AVM_DIRECT                       = y

# BT Dual mode
MTK_BT_DUO_ENABLE = y

# bt module enable
MTK_BT_ENABLE                       = y
MTK_BLE_ONLY_ENABLE                 = n
MTK_BT_HFP_ENABLE                   = y
MTK_BT_HFP_FORWARDER_ENABLE         = y
MTK_BT_AVRCP_ENABLE                 = y
MTK_BT_AVRCP_ENH_ENABLE             = y
MTK_BT_A2DP_ENABLE                  = y
MTK_BT_PBAP_ENABLE                  = n
MTK_BT_SPP_ENABLE                   = y
MTK_BT_AVM_SHARE_BUF                = y
# aws earbuds feature
MTK_AWS                             = n

#BT external timer
MTK_BT_TIMER_EXTERNAL_ENABLE = y
MTK_PORT_SERVICE_ENABLE               = y

#DSP Audio Message
MTK_DSP_AUDIO_MESSAGE_ENABLE        = y

# CM4 playback module
MTK_CM4_PLAYBACK_ENABLE = y

# CM4 record module
MTK_CM4_RECORD_ENABLE = y

# WWE module
ifneq ($(wildcard $(strip $(ROOTDIR))/prebuilt/middleware/MTK/dspalg/wwe/ama/),)
MTK_WWE_ENABLE = y
MTK_WWE_AMA_ENABLE = y
MTK_WWE_AMA_USE_PIC = n
endif

ifneq ($(wildcard $(strip $(ROOTDIR))/prebuilt/middleware/MTK/dspalg/wwe/gsound/),)
MTK_WWE_ENABLE = y
MTK_WWE_GSOUND_ENABLE = y
MTK_WWE_GSOUND_USE_PIC = n
endif

# LC3
LC3_CODEC_LIB = $(strip $(ROOTDIR))/middleware/MTK/dspalg/lc3_codec/module.mk
ifeq ($(LC3_CODEC_LIB), $(wildcard $(LC3_CODEC_LIB)))
AIR_BT_CODEC_BLE_ENABLED =  y
endif

# Voice Prompt module
MTK_PROMPT_SOUND_ENABLE = y

# I2S Slave module
MTK_I2S_SLAVE_ENABLE = n

# ANC module
MTK_ANC_ENABLE = y
ifeq ($(MTK_ANC_ENABLE), y)
ANC_SOURCE_CODE = $(strip $(ROOTDIR))/middleware/MTK/dspfw/anc_protected/port/chip/ab156x/module.mk
ifeq ($(ANC_SOURCE_CODE), $(wildcard $(ANC_SOURCE_CODE)))
MTK_ANC_LIB_ENABLE =  n
else
MTK_ANC_LIB_ENABLE =  y
endif
endif

# PEQ module
MTK_PEQ_ENABLE = y
AIR_VOICE_DRC_ENABLE = y

# linein playback
MTK_LINEIN_PLAYBACK_ENABLE = y
# LINEIN PEQ module
MTK_LINEIN_PEQ_ENABLE = y
AIR_DRC_ENABLE = y
# LINEIN INS module
MTK_LINEIN_INS_ENABLE = y

# SIDETONE module
AIR_SIDETONE_ENABLE = y

# AUDIO LOOPBACK TEST module
MTK_AUDIO_LOOPBACK_TEST_ENABLE = y
MTK_AUDIO_TRANSMITTER_ENABLE = y

# Audio Dump
MTK_AUDIO_DUMP_BY_CONFIGTOOL = y

# AirDump module
MTK_AIRDUMP_EN = y

# For 1568 ALG interface (compander)
MTK_BT_HFP_SPE_ALG_V2 = y

# AGC module
MTK_VOICE_AGC_ENABLE = n

# For IGO NR
ifneq ($(wildcard $(strip $(ROOTDIR))/prebuilt/middleware/third_party/dspalg/igo_nr/),)
    AIR_IGO_NR_ENABLE = y
    MTK_3RD_PARTY_NR = y
    AIR_IGO_NR_PLUS_1_ENABLE = n
else
    MTK_3RD_PARTY_NR = n
    AIR_IGO_NR_PLUS_1_ENABLE = n
endif

# Voice PLC module
AIR_VOICE_PLC_ENABLE = y

# Mute smoother module
AIR_MUTE_SMOOTHER_ENABLE = y

PRELOADER_ENABLE = y
        DSP0_PISPLIT_DEMO_LIBRARY = n
        MTK_BT_A2DP_AAC_USE_PIC = y
        MTK_BT_A2DP_SBC_USE_PIC = y
        MTK_BT_A2DP_MSBC_USE_PIC = y
        MTK_BT_A2DP_CVSD_USE_PIC = n
        MTK_BT_A2DP_CPD_USE_PIC = y
        MTK_BT_A2DP_ECNR_USE_PIC = y
        MTK_BT_CLK_SKEW_USE_PIC = y
ifeq ($(MTK_BT_A2DP_VENDOR_ENABLE),y)
		MTK_BT_A2DP_VENDOR_USE_PIC = y
endif
ifeq ($(MTK_PEQ_ENABLE),y)
        MTK_BT_PEQ_USE_PIC = y
endif
ifeq ($(MTK_LINEIN_PEQ_ENABLE),y)
        MTK_BT_PEQ_USE_PIC = y
endif
ifeq ($(AIR_VOICE_PLC_ENABLE),y)
        MTK_PLC_USE_PIC = y
endif
    PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1 = n
        DSP1_PISPLIT_DEMO_LIBRARY = n
ifeq ($(AIR_BT_CODEC_BLE_ENABLED),y)
        MTK_BT_A2DP_LC3_USE_PIC = y
        MTK_BT_A2DP_LC3_USE_LIGHT_PIC = n
endif

# Low power level
MTK_LOWPOWER_LEVEL = 2

# HWSRC for DL1
ENABLE_HWSRC_ON_MAIN_STREAM = y

# AMP delay timer
ENABLE_AMP_TIMER = y

# SideTone Gain Ramp
ENABLE_SIDETONE_RAMP_TIMER = y
# DEFAULT ENABLE DSP HW LOOPBACK
AB1568_BRING_UP_DSP_DEFAULT_HW_LOOPBACK = n

# 2a2d
MTK_AUDIO_SUPPORT_MULTIPLE_MICROPHONE = y

# Use HWSRC to do clkskew
ENABLE_HWSRC_CLKSKEW = y

# Auto Adaptvie ANC
MTK_ANC_SURROUND_MONITOR_ENABLE = n

#User triggered adaptive ANC
ifeq ($(MTK_ANC_ENABLE), y)
MTK_USER_TRIGGER_FF_CODE = $(strip $(ROOTDIR))/middleware/MTK/dspalg/user_trigger_ff/user_trigger_adaptive_ff_v2/module.mk
ifeq ($(MTK_USER_TRIGGER_FF_CODE), $(wildcard $(MTK_USER_TRIGGER_FF_CODE)))
MTK_USER_TRIGGER_FF_ENABLE     = y
endif
endif

#ANC wind detection
AIR_ANC_WIND_DETECTION_ENABLE = n
AIR_WIND_DETECTION_USE_PIC = n

# Gaming Headset Detachable mic / Fixed mic switch
AIR_AUDIO_DETACHABLE_MIC_ENABLE = y

# Software gain enable
AIR_SOFTWARE_GAIN_ENABLE = y

#Wired audio support
AIR_WIRED_AUDIO_ENABLE = y
AIR_SOFTWARE_CLK_SKEW_ENABLE = y
AIR_SOFTWARE_MIXER_ENABLE = y
AIR_SOFTWARE_BUFFER_ENABLE =y
# Software SRC enable
AIR_SOFTWARE_SRC_ENABLE = y
# Wired audio game chat volume smart balance enable
AIR_VOLUME_ESTIMATOR_ENABLE = y
AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE = y

