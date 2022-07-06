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

#######################################################
# Project vairables assignment

SOURCE_DIR = ../../../../..
PWD := $(shell pwd)
SDK_PATH := $(abspath $(PWD)/$(SOURCE_DIR))
FEATURE ?= feature.mk
# Include Module
include $(FEATURE)
DEBUG = 0
FLOAT_TYPE = hard
BUILD_DIR = $(PWD)/build

# Project name
PROJ_NAME := $(shell basename $(dir $(PWD)))
PROJ_PATH := $(abspath $(dir $(PWD)))/GCC
OUTPATH := $(PWD)/build

# LD Flags
LDFLAGS = $(ALLFLAGS) $(FPUFLAGS) --specs=nano.specs -lnosys -nostartfiles
LDFLAGS += -Wl,-wrap=malloc -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=free
LDFLAGS += -Wl,--undefined=uxTopUsedPriority

ifeq ($(MTK_DEBUG_LEVEL),warning)
CFLAGS += -Wno-unused-but-set-variable
CFLAGS += -Wno-unused-variable
endif

TARGET ?= FLASH
#Booting method
ifeq ($(TARGET), SYSRAM)
LDFLAGS += -Wl,-Tab156x_sysram.ld
endif
ifeq ($(TARGET), FLASH)
ifeq ($(FLASH_SIZE_8M), y)
    ifeq ($(MTK_FOTA_TEST_MEMORY_LAYOUT_MODIFICATION), y)
        LDFLAGS += -Wl,-Tab156x_flash_8m_fota_test.ld
        LINKER_SCRIPT_FILE = $(SOURCE_DIR)/$(APP_PATH)/GCC/ab156x_flash_8m_fota_test.ld
    else
        ifndef LINKER_SCRIPT_FILE
            LDFLAGS += -Wl,-Tab156x_flash_8m.ld
            LINKER_SCRIPT_FILE = $(SOURCE_DIR)/$(APP_PATH)/GCC/ab156x_flash_8m.ld
        else
            LDFLAGS += -Wl,-T$(LINKER_SCRIPT_FILE)
            LINKER_SCRIPT_FILE = $(SOURCE_DIR)/$(APP_PATH)/GCC/$(LINKER_SCRIPT_FILE)
        endif
    endif
    NVDM_PARTITION_SIZE = 96
else
    ifeq ($(MTK_VA_MODEL_MANAGER_ENABLE),y)
        LDFLAGS += -Wl,-Tab156x_flash.ld
        LINKER_SCRIPT_FILE = $(SOURCE_DIR)/$(APP_PATH)/GCC/ab156x_flash.ld
    endif

    ifeq ($(AIR_3RD_PARTY_NR_ENABLE),y)
        LDFLAGS += -Wl,-Tab156x_flash_3rd_nr.ld
        LINKER_SCRIPT_FILE = $(SOURCE_DIR)/$(APP_PATH)/GCC/ab156x_flash_3rd_nr.ld
    endif

    ifndef LINKER_SCRIPT_FILE
        LDFLAGS += -Wl,-Tab156x_flash_no_lm.ld
        LINKER_SCRIPT_FILE = $(SOURCE_DIR)/$(APP_PATH)/GCC/ab156x_flash_no_lm.ld
    endif
    NVDM_PARTITION_SIZE = 64
endif
endif

ifeq ($(DEBUG_IO), 1)
LDFLAGS += --specs=rdimon.specs -lrdimon
CFLAGS += -DDEBUG_IO
endif

#######################################################
# Gloabl Config
-include $(SOURCE_DIR)/.config
# IC Config
-include $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk
# Board Config
#-include $(SOURCE_DIR)/config/board/$(BOARD_CONFIG)/board.mk
# HAL driver files
include $(SOURCE_DIR)/driver/chip/$(IC_CONFIG)/module.mk
# EPT Config
include $(SOURCE_DIR)/driver/board/$(BOARD_CONFIG)/ept/module.mk
# HW Resource Assignment Config
include $(SOURCE_DIR)/driver/board/$(BOARD_CONFIG)/hw_resource_assignment/module.mk
# RTOS source files
include $(SOURCE_DIR)/kernel/rtos/FreeRTOS/module.mk
# kernel service files
include $(SOURCE_DIR)/kernel/service/module.mk
# NVDM files
ifeq ($(MTK_NVDM_ENABLE),y)
include $(SOURCE_DIR)/middleware/MTK/nvdm/module.mk
endif
# add USB CLASS files
ifeq ($(MTK_USB_DEMO_ENABLED), y)
include $(SOURCE_DIR)/middleware/MTK/usb/module.mk
endif

ifeq ($(MTK_LAYOUT_PARTITION_ENABLE),y)
# layout partition
include $(SOURCE_DIR)/kernel/service/layout_partition/module.mk
endif

#ifeq ($(AIR_MULTI_POINT_ENABLE), y)
#ifeq ($(AIR_LE_AUDIO_ENABLE), y)
#$(error "======Not support EMP and LEAUDIO co-exist======")
#endif
#endif

ifeq ($(REBOOT_AFTER_EXCEPTION), y)
CFLAGS += -DREBOOT_AFTER_EXCEPTION
endif
ifeq ($(LOG_OUTPUT_FROM_USB), y)
CFLAGS += -DLOG_OUTPUT_FROM_USB
endif
ifeq ($(SPECIAL_DEFINE_FOR_MINI_BOARD), y)
CFLAGS += -DSPECIAL_DEFINE_FOR_MINI_BOARD
endif

ifeq ($(MTK_RACE_CMD_ENABLE), y)
ifeq ($(MTK_RACE_FIND_ME_ENABLE), y)
CFLAGS += -DRACE_FIND_ME_ENABLE
endif
endif
ifeq ($(MTK_BT_PKA_IDS_ENABLE),y)
CFLAGS += -DMTK_BT_PKA_IDS_ENABLE
endif
ifeq ($(EINT_KEY_ENABLED), y)
include $(SOURCE_DIR)/driver/board/component/bsp_eint_key/module.mk
endif

ifeq ($(GSENSOR_KEY_ENABLED), y)
include $(SOURCE_DIR)/driver/board/component/bsp_gsensor_key/module.mk
endif

ifeq ($(MTK_IN_EAR_FEATURE_ENABLE), y)
include $(SOURCE_DIR)/driver/board/component/bsp_inear/module.mk
endif
ifeq ($(SUPPORT_ROLE_HANDOVER_SERVICE), y)
CFLAGS += -DSUPPORT_ROLE_HANDOVER_SERVICE
# bt bt_role_handover_service
include $(SOURCE_DIR)/middleware/MTK/bt_role_handover/module.mk
endif

ifeq ($(MTK_AWS_MCE_ROLE_RECOVERY_ENABLE), y)
CFLAGS += -DMTK_AWS_MCE_ROLE_RECOVERY_ENABLE
endif

ifeq ($(AIR_MCSYNC_SHARE_ENABLE), y)
include $(SOURCE_DIR)/middleware/MTK/bt_mcsync_share/module.mk
endif

# BT Ultra Low Latency
ifeq ($(AIR_BT_ULTRA_LOW_LATENCY_ENABLE), y)
include $(SOURCE_DIR)/middleware/MTK/bt_ultra_low_latency/module.mk
endif

# Bluetooth module
include $(SOURCE_DIR)/middleware/MTK/bluetooth/module.mk

# BT callback manager
include $(SOURCE_DIR)/middleware/MTK/bt_callback_manager/module.mk

# BT connection manager
include $(SOURCE_DIR)/middleware/MTK/bt_connection_manager/module.mk

# bt components le module
include $(SOURCE_DIR)/middleware/MTK/bluetooth_service/module.mk

# ble battery service module
#include $(SOURCE_DIR)/middleware/MTK/ble_bas/module.mk

# IAS service module
ifeq ($(MTK_BLE_IAS), y)
include $(SOURCE_DIR)/middleware/MTK/ble_ias/module.mk
endif

# DIS service module
include $(SOURCE_DIR)/middleware/MTK/ble_dis/module.mk

#AWS MCE Report
ifeq ($(MTK_AWS_MCE_ENABLE), y)
include $(SOURCE_DIR)/middleware/MTK/bt_aws_mce_report/module.mk
endif
# BT Sink service module
include $(SOURCE_DIR)/middleware/MTK/sink/module.mk

# ATCI module
include $(SOURCE_DIR)/middleware/MTK/atci/module.mk

# Audio module
include $(SOURCE_DIR)/middleware/MTK/audio/port/$(IC_CONFIG)/module.mk

# audio middleware files
include $(SOURCE_DIR)/middleware/MTK/audio/module.mk

# include the nanopb module.mk
include $(SOURCE_DIR)/middleware/third_party/nanopb/module.mk

# ANC module
ifeq ($(MTK_ANC_ENABLE), y)
ifeq ($(MTK_ANC_LIB_ENABLE), y)
include $(SOURCE_DIR)/prebuilt/middleware/MTK/audio/anc/module.mk
else
include $(SOURCE_DIR)/middleware/MTK/audio/anc_control_protected/GCC/module.mk
endif
endif

ifeq ($(MTK_PORT_SERVICE_ENABLE), y)
# add PORT SERVICE files
include $(SOURCE_DIR)/middleware/MTK/port_service/module.mk
endif

# fota related files
ifeq ($(MTK_FOTA_ENABLE), y)
include $(SOURCE_DIR)/middleware/MTK/fota/module.mk
endif

include $(SOURCE_DIR)/middleware/third_party/mbedtls/module.mk

ifeq ($(MTK_PORT_SERVICE_ENABLE), y)
#include $(SOURCE_DIR)/middleware/MTK/port_service/module.mk

ifeq ($(MTK_PORT_SERVICE_BT_ENABLE),y)
include $(SOURCE_DIR)/middleware/MTK/bt_air/GCC/module.mk
endif
endif


ifeq ($(MTK_MUX_BT_ENABLE),y)
CFLAGS += -DMTK_MUX_BT_ENABLE
endif

ifeq ($(MTK_RACE_CMD_ENABLE), y)
include $(SOURCE_DIR)/middleware/MTK/race_cmd/module.mk
endif

include $(SOURCE_DIR)/driver/board/component/bsp_flash/module.mk

include $(SOURCE_DIR)/middleware/MTK/key/module.mk

ifeq ($(findstring cell, $(BOARD_TYPE)), cell)
include $(SOURCE_DIR)/driver/board/component/bsp_psensor_key/module.mk
endif

ifeq ($(MTK_BATTERY_MANAGEMENT_ENABLE), y)
include $(SOURCE_DIR)/middleware/MTK/battery_management/module.mk
endif

ifeq ($(MTK_BATTERY_EOC_ENTER_RTC),y)
CFLAGS += -DMTK_BATTERY_EOC_ENTER_RTC
endif

# Smart Charger
ifeq ($(MTK_SMART_CHARGER_ENABLE), y)
CFLAGS += -DMTK_SMART_CHARGER_ENABLE
include $(SOURCE_DIR)/middleware/MTK/smart_charger/module.mk
endif

# Smart Charger 1wire
ifeq ($(MTK_SMART_CHARGER_1WIRE_ENABLE), y)
CFLAGS += -DMTK_SMART_CHARGER_1WIRE_ENABLE
ifeq ($(MTK_SMART_CHARGER_1WIRE_SAMPLE), y)
CFLAGS += -DMTK_SMART_CHARGER_1WIRE_SAMPLE
endif
ifeq ($(MTK_CHARGER_CASE_1WIRE_3PIN), y)
CFLAGS += -DMTK_CHARGER_CASE_1WIRE_3PIN
endif
ifeq ($(MTK_SMART_CHARGER_1WIRE_EVK), y)
CFLAGS += -DMTK_SMART_CHARGER_1WIRE_EVK
endif
ifeq ($(MTK_SMART_CHARGER_1WIRE_RACE_ENABLE), y)
CFLAGS += -DMTK_SMART_CHARGER_1WIRE_RACE_ENABLE
endif
ifeq ($(MTK_SMART_CHARGER_1WIRE_9600), y)
CFLAGS += -DMTK_SMART_CHARGER_1WIRE_9600
endif
endif

ifeq ($(MTK_HCI_LOG_MIX_WITH_SYSLOG), y)
CFLAGS += -DHCI_LOG_MIX_WITH_SYSLOG
endif

ifeq ($(MTK_BT_ENABLE),y)
include $(SOURCE_DIR)/middleware/third_party/micro_ecc/module.mk
endif

ifeq ($(MTK_BT_FAST_PAIR_ENABLE),y)
include $(SOURCE_DIR)/middleware/MTK/bt_fast_pair/module.mk
include $(SOURCE_DIR)/middleware/third_party/mbedtls/module.mk
endif

# ui shell
include $(SOURCE_DIR)/middleware/MTK/ui_shell/module.mk

# isink led files
include $(SOURCE_DIR)/driver/board/component/bsp_led/module.mk
CFLAGS += -DLED_ENABLE
# rofs
include $(SOURCE_DIR)/middleware/MTK/rofs/module.mk

ifeq ($(GSOUND_LIBRARY_ENABLE), y)
CFLAGS += -DGSOUND_LIBRARY_ENABLE
CFLAGS += -DGSOUND_AWS_REPORT
CFLAGS += -DGSOUND_SUPPORT_TWS

ifeq ($(GSOUND_SUPPORT_REDUCE_GESTURE), y)
CFLAGS += -DGSOUND_SUPPORT_REDUCE_GESTURE
endif

ifeq ($(GSOUND_HOTWORD_ENABLE), y)
CFLAGS += -DGSOUND_HOTWORD_ENABLE
endif

# ANCS module
include $(SOURCE_DIR)/prebuilt/middleware/MTK/ble_ancs/module.mk

# gsound module
include $(SOURCE_DIR)/middleware/third_party/gsound/module.mk
endif

ifeq ($(MTK_INEAR_ENHANCEMENT), y)
CFLAGS += -DMTK_INEAR_ENHANCEMENT
else ifeq ($(MTK_DUALMIC_INEAR), y)
CFLAGS += -DMTK_DUALMIC_INEAR
endif

ifeq ($(MTK_AMA_ENABLE),y)
include $(SOURCE_DIR)/middleware/MTK/ama/module.mk
endif

ifeq ($(MTK_VA_MODEL_MANAGER_ENABLE),y)
include $(SOURCE_DIR)/middleware/MTK/va_model_manager/module.mk
CFLAGS += -DMTK_VA_MODEL_MANAGER_ENABLE
endif

# Add for ms_cfu
# MS CFU.
MS_CFU_CODE = $(strip $(SOURCE_DIR))/middleware/MTK/ms_cfu/module.mk
ifneq ($(MS_CFU_CODE), $(wildcard $(MS_CFU_CODE)))
AIR_CFU_ENABLE = n
endif
ifeq ($(AIR_CFU_ENABLE), y)
include $(SOURCE_DIR)/middleware/MTK/ms_cfu/module.mk
endif

# Add for xiaowei VA
ifeq ($(MTK_VA_XIAOWEI_ENABLE),y)
include $(SOURCE_DIR)/middleware/MTK/xiaowei/module.mk
CFLAGS += -DMTK_VA_XIAOWEI_ENABLE
endif

# Add for ms_teams
ifeq ($(AIR_MS_TEAMS_ENABLE), y)
include $(SOURCE_DIR)/middleware/MTK/ms_teams/module.mk
endif

# Add for xiaoai VA
ifeq ($(MTK_VA_XIAOAI_ENABLE),y)
include $(SOURCE_DIR)/middleware/MTK/xiaoai/module.mk
CFLAGS += -DMTK_VA_XIAOAI_ENABLE
ifeq ($(MULTI_VA_SUPPORT_COMPETITION),y)
CFLAGS += -DMTK_VA_XIAOAI_MULTI_VA_ENABLE
endif
ifeq ($(MTK_XIAOAI_WWE_ENABLE),y)
CFLAGS += -DMTK_XIAOAI_WWE_ENABLE
endif
ifeq ($(MTK_XIAOAI_PTT_ENABLE),y)
CFLAGS += -DMTK_XIAOAI_PTT_ENABLE
endif
endif

ifeq ($(MTK_TICKLESS_USE_SW_GPT),y)
CFLAGS += -DMTK_TICKLESS_USE_SW_GPT
endif

ifeq ($(BT_ROLE_HANDOVER_WITH_SPP_BLE), y)
CFLAGS += -DBT_ROLE_HANDOVER_WITH_SPP_BLE
endif

ifeq ($(AIR_MULTI_POINT_ENABLE),y)
CFLAGS += -DAIR_MULTI_POINT_ENABLE
endif

ifeq ($(MTK_BT_HID_ENABLE),y)
CFLAGS += -DMTK_BT_HID_ENABLE
endif
# LE Audio
ifeq ($(AIR_LE_AUDIO_ENABLE), y)
CFLAGS += -DBT_LE_AUDIO_ENABLE
CFLAGS += -DBT_EATT_ENABLE
CFLAGS += -DAIR_LE_AUDIO_ENABLE
CFLAGS += -DAIR_LE_AUDIO_CIS_ENABLE
CFLAGS += -DAIR_BT_CODEC_BLE_ENABLED
CFLAGS += -DAPPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE

include $(SOURCE_DIR)/middleware/MTK/bt_le_audio/module.mk

ifeq ($(AIR_LE_AUDIO_MULTIPOINT_ENABLE), y)
CFLAGS += -DAIR_LE_AUDIO_MULTIPOINT_ENABLE
endif

ifeq ($(AIR_LE_AUDIO_BIS_ENABLE), y)
CFLAGS += -DAIR_LE_AUDIO_BIS_ENABLE
endif
endif

ifeq ($(MTK_GATT_OVER_BREDR_ENABLE),y)
CFLAGS += -DMTK_GATT_OVER_BREDR_ENABLE
endif

# PSAP module
ifeq ($(AIR_ADVANCED_PASSTHROUGH_ENABLE), y)
ifeq ($(AIR_ADVANCED_PASSTHROUGH_LIB_ENABLE), y)
include $(SOURCE_DIR)/prebuilt/middleware/MTK/psap/module.mk
else
include $(SOURCE_DIR)/middleware/MTK/audio/psap_protected/GCC/module.mk
endif
endif

#######################################################
# Main APP files
APP_PATH        := $(patsubst $(SDK_PATH)/%,%,$(abspath $(dir $(PWD))))
APP_PATH_SRC    := $(APP_PATH)/src

include $(SOURCE_DIR)/$(APP_PATH_SRC)/apps/module.mk

# Config bin path
CONFIG_BIN_PATH := $(SOURCE_DIR)/$(APP_PATH)/config_bin/$(BOARD_TYPE)


##############################################################################
#
# SDK source files
#
##############################################################################
APP_FILES      += $(APP_PATH_SRC)/main.c
APP_FILES      += $(APP_PATH)/GCC/syscalls.c
APP_FILES      += $(APP_PATH_SRC)/regions_init.c
APP_FILES      += $(APP_PATH_SRC)/sys_init.c
APP_FILES      += $(APP_PATH_SRC)/boards/$(BOARD_TYPE)/ept_gpio_var.c
APP_FILES      += $(APP_PATH_SRC)/boards/$(BOARD_TYPE)/ept_eint_var.c
APP_FILES      += $(APP_PATH_SRC)/task_def.c
APP_FILES      += $(APP_PATH_SRC)/bt_init.c
APP_FILES      += $(APP_PATH_SRC)/bt_app_utility/hci_log.c
APP_FILES      += $(APP_PATH_SRC)/dcxo_capid.c
APP_FILES      += $(APP_PATH_SRC)/nvdm_config.c
APP_FILES      += $(APP_PATH_SRC)/nvdm_config_factory_reset.c
APP_FILES      += $(APP_PATH_SRC)/bt_app_utility/bt_power_on_config.c

APP_FILES      += $(APP_PATH_SRC)/bt_app_utility/gatt_service.c \
                  $(APP_PATH_SRC)/bt_app_utility/bt_gattc_discovery.c \
                  $(APP_PATH_SRC)/bt_app_utility/bt_app_common.c \
                  $(APP_PATH_SRC)/bt_app_utility/bt_app_common_at_cmd.c \
                  $(APP_PATH_SRC)/bt_app_utility/bt_app_dis.c \
                  $(APP_PATH_SRC)/bt_app_utility/bt_customer_config.c\
                  $(APP_PATH_SRC)/bt_app_utility/ut_app_hid.c

ifeq ($(MTK_RACE_CMD_ENABLE), y)
APP_FILES      += $(APP_PATH_SRC)/race/race_app_aws_event_hdl.c \
                  $(APP_PATH_SRC)/race/race_app_race_event_hdl.c \
                  $(APP_PATH_SRC)/race/race_app_bt_event_hdl.c
endif

SYS_FILES      += $(APP_PATH_SRC)/system_ab156x.c

C_FILES        += $(APP_FILES) $(SYS_FILES)
S_FILES        += $(APP_PATH)/GCC/startup_ab156x.s

APP_FILES      += $(APP_PATH_SRC)/bt_app_utility/low_power_demo/bt_at_cmd.c

ifeq ($(GSOUND_LIBRARY_ENABLE), y)
C_FILES        += $(APP_PATH_SRC)/bt_app_utility/ble_ancs_ios_notification/ble_ancs_app.c
C_FILES        += $(APP_PATH_SRC)/bt_app_utility/ble_ancs_ios_notification/bt_ancs_common.c
ifeq ($(BT_ROLE_HANDOVER_WITH_SPP_BLE), y)
C_FILES        += $(APP_PATH_SRC)/bt_app_utility/ble_ancs_ios_notification/bt_ancs_rho.c
endif
endif

# Add IA2P profile support
ifeq ($(MTK_IAP2_PROFILE_ENABLE),y)
CFLAGS += -DMTK_IAP2_PROFILE_ENABLE

APP_FILES      += $(APP_PATH_SRC)/iap2/iAP2_config.c
include $(SOURCE_DIR)/middleware/MTK/iap2/GCC/module.mk
include $(SOURCE_DIR)/middleware/MTK/mfi_coprocessor/module.mk

ifeq ($(MTK_IAP2_VIA_MUX_ENABLE),y)
CFLAGS += -DMTK_IAP2_VIA_MUX_ENABLE
endif
endif


#C_FILES += $(APP_PATH_SRC)/ble_bas_app/ble_bas_app.c
#C_FILES += $(APP_PATH_SRC)/ble_find_me_server/ble_find_me_server.c


##############################################################################
#
# SDK object files
#
##############################################################################


C_OBJS          = $(C_FILES:%.c=$(BUILD_DIR)/%.o)
CXX_OBJS        = $(CXX_FILES:%.cpp=$(BUILD_DIR)/%.o)
S_OBJS          = $(S_FILES:%.s=$(BUILD_DIR)/%.o)
CXX_OBJS        = $(CXX_FILES:%.cpp=$(BUILD_DIR)/%.o)

#######################################################
# Include path

CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Include
CFLAGS += -I$(SOURCE_DIR)/driver/CMSIS/Device/MTK/$(IC_CONFIG)/Include
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/bt_app_utility
#CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/ble_find_me_server
#CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/ble_bas_app
ifeq ($(MTK_RACE_CMD_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/race
endif
ifeq ($(MTK_PROMPT_SOUND_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/voice_tone
endif
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/boards/$(BOARD_TYPE)
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/src/boards/$(BOARD_TYPE)

ifeq ($(GSOUND_LIBRARY_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/bt_app_utility/ble_ancs_ios_notification
endif

ifeq ($(MTK_IAP2_PROFILE_ENABLE), y)
CFLAGS += -I$(SOURCE_DIR)/$(APP_PATH)/inc/iap2
endif

######################################################
# Transfer the feature option to the LM generator
# Check AMA and AMA WWE enabled or not
AMA_WWE_ENABLED = n
GVA_HOTWORD_ENABLED = n
ifeq ($(MTK_AMA_ENABLE)_$(AMA_TRIGGER_MODE_WWD_ENABLE),y_y)
AMA_WWE_ENABLED = y
endif
# Check GVA and GVA Hotword enabled or not
ifeq ($(GSOUND_LIBRARY_ENABLE)_$(GSOUND_HOTWORD_ENABLE),y_y)
GVA_HOTWORD_ENABLED = y
endif

#######################################################
# Rules

.PHONY: proj clean $(MODULE_PATH)

all: cleanlog checkoption proj
	@mkdir -p $(BUILD_DIR)
	@$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf
	@if [ -e $(CONFIG_BIN_PATH)/nvdm.bin ]; then cp -f $(CONFIG_BIN_PATH)/nvdm.bin $(OUTPATH)/; else $(SOURCE_DIR)/$(POSTBUILD) -f nv2bin $(NVDM_PARTITION_SIZE) $(OUTPATH)/nvdm.bin $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/nvkey.xml $(SOURCE_DIR)/$(APP_PATH)/config_bin/nvkey.xml $(CONFIG_BIN_PATH)/nvkey.xml; fi

	# For generate the language model binary file and copy the binary file to the output path.
	@if [ "$(MTK_VA_MODEL_MANAGER_ENABLE)" = "y" ]; then \
		echo; \
		echo "==============================================================="; \
		echo "= Start to generate language model binary"; \
		echo "==============================================================="; \
		$(SOURCE_DIR)/$(LM_BIN_GENERATOR) -i $(CONFIG_BIN_PATH)/language_model/config.xml -o $(OUTPATH) -l $(LINKER_SCRIPT_FILE) -a $(AMA_WWE_ENABLED) -g $(GVA_HOTWORD_ENABLED); \
		if [ "$$?" != "0" ]; then \
			echo; echo "error: Failed to generate language model binary" >&2; echo; \
			set -e; exit 1; \
		fi; \
		echo "==============================================================="; \
		echo "= Generate language model binary finished"; \
		echo "==============================================================="; \
		echo; \
	fi

	@cp -f $(CONFIG_BIN_PATH)/filesystem.bin $(OUTPATH)/
	@$(SOURCE_DIR)/$(FLASH_DOWNLOAD_CFG_GENERATOR) $(LINKER_SCRIPT_FILE) $(OUTPATH) $(IC_CONFIG) MCU_FW
	@$(SOURCE_DIR)/tools/scripts/build/copy_firmware.sh $(SOURCE_DIR) $(OUTPATH) $(IC_CONFIG) $(BOARD_CONFIG) $(PROJ_NAME).bin $(PWD) $(MTK_SECURE_BOOT_ENABLE)

checkoption:
# Add make error for AMA and AMA WWE feature enabled, but do not exist language model.
ifeq ($(MTK_AMA_ENABLE)_$(AMA_TRIGGER_MODE_WWD_ENABLE),y_y)
ifeq ($(wildcard $(SOURCE_DIR)/prebuilt/middleware/third_party/language_model/ama),)
	$(warning "Error: [Addon error] The AMA language model for wake word detection is not exist, please contact Amazon to get the language model. Then put into the folder prebuilt/middleware/third_party/language_model/ama")
	@echo "Error: [Addon error] The AMA language model for wake word detection is not exist, please contact Amazon to get the language model. Then put into the folder prebuilt/middleware/third_party/language_model/ama";
	@set -e; exit 2;
endif
endif

ifeq ($(MTK_SECURE_BOOT_ENABLE), y)
	@$(SOURCE_DIR)/tools/security/secure_boot/scripts/build/build_sboot_header.sh $(SOURCE_DIR) $(OUTPATH) $(BOARD_CONFIG) $(PROJ_NAME) $(IC_TYPE)
endif
# if enable XIAOAI, need build with name of the feature.mk
ifneq ($(findstring y,$(MTK_VA_XIAOAI_ENABLE)),y)
MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH)
else
MOD_EXTRA = BUILD_DIR=$(BUILD_DIR) OUTPATH=$(OUTPATH) PROJ_PATH=$(PROJ_PATH) fm=$(PROJ_PATH)/$(FEATURE)
endif

$(LIBS): $(MODULE_PATH)

$(MODULE_PATH):
	@+make -C $@ $(MOD_EXTRA) $($@_EXTRA)

proj: $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(C_OBJS) $(CXX_OBJS) $(S_OBJS) $(LIBS)
	@echo Linking...
	@cp $(LINKER_SCRIPT_FILE) $(OUTPATH)
	@if [ -e "$@" ]; then rm -f "$@"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).map" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).map"; fi
	@if [ -e "$(OUTPATH)/$(PROJ_NAME).bin" ]; then rm -f "$(OUTPATH)/$(PROJ_NAME).bin"; fi
	@echo $(patsubst $(SDK_PATH)/%,$(SOURCE_DIR)/%, $(LDFLAGS) -Wl,--start-group -Wl,--whole-archive $^ -Wl,--no-whole-archive -Wl,--end-group -Wl,-Map=$(patsubst $(SDK_PATH)/%,$(SOURCE_DIR)/%,$(OUTPATH)/$(PROJ_NAME).map) -lm -o $@) > $(OUTPATH)/link_option.tmp
	@$(CC) @$(OUTPATH)/link_option.tmp 2>>$(ERR_LOG)
	@rm -f $(OUTPATH)/link_option.tmp
	@$(OBJCOPY) -O binary --only-section=.log_str $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/cm4_log_str.bin
	@$(OBJCOPY) -O binary --only-section=.partition_table $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/partition_table.bin
	@$(OBJCOPY) -O binary --remove-section=.log_str --remove-section=.partition_table $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin
	@echo Done


ifneq ($(filter MINGW% MSYS%,$(OS_VERSION)),)
  POSTBUILD := tools/scripts/build/PostBuildTool/windows/PostBuildTool.exe
  LM_BIN_GENERATOR := tools/scripts/build/lm_bin_generator/windows/gen_lm_tool.exe
  FLASH_DOWNLOAD_CFG_GENERATOR := tools/scripts/build/AutoGenDownloadCfg/windows/AutoGenDownloadCfg.exe
else ifeq ($(OS_VERSION), Darwin)
  POSTBUILD := tools/scripts/build/PostBuildTool/mac/nvdm_post_build
  LM_BIN_GENERATOR := tools/scripts/build/lm_bin_generator/mac/gen_lm_tool
  FLASH_DOWNLOAD_CFG_GENERATOR := tools/scripts/build/AutoGenDownloadCfg/mac/AutoGenDownloadCfg
else
  POSTBUILD := tools/scripts/build/PostBuildTool/linux/nvdm_post_build
  LM_BIN_GENERATOR := tools/scripts/build/lm_bin_generator/linux/gen_lm_tool
  FLASH_DOWNLOAD_CFG_GENERATOR := tools/scripts/build/AutoGenDownloadCfg/linux/AutoGenDownloadCfg
endif

include $(SOURCE_DIR)/.rule.mk

clean:
	rm -rf $(OUTPATH)

.PHONY: options

options:
	@for opt in `sed 's/[()]/\n/g' $(SOURCE_DIR)/config/chip/$(IC_CONFIG)/chip.mk|sed 's/-D/ /'|sort|uniq|grep ^MTK_|grep _ENABLE$$`; do echo $$opt; done
print-%  : ; @echo $* = $($*)
