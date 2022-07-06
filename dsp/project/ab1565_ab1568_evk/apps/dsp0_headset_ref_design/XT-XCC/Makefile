# Copyright Statement:
#
# (C) 2020  Airoha Technology Corp. All rights reserved.
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

#------------------------------------------------------------------------------
# See README.TXT for more information.
#------------------------------------------------------------------------------

TARGET ?= FLASH
################################################################################
# Project Configuration
################################################################################
PWD          :=  $(shell pwd)
ROOTDIR      :=  ../../../../..
PROJ_PATH    :=  $(patsubst $(abspath $(PWD)/$(ROOTDIR))/%,%,$(abspath $(dir $(PWD))))
FEATURE      ?=  feature.mk
PROJ_NAME    :=  $(notdir $(shell cd ../ ; pwd))
BOARD        :=  $(firstword $(subst /, ,$(patsubst $(abspath $(ROOTDIR))/project/%,%,$(PWD))))
FEATURE_BASENAME      :=  $(basename $(FEATURE))
DEFAULT_BIN_PATH      :=  $(abspath $(strip $(ROOTDIR))/prebuilt/dsp0_bin/$(strip $(BOARD))/$(strip $(PROJ_NAME))/$(strip $(FEATURE_BASENAME)))

OUTDIR       :=  $(abspath $(strip $(ROOTDIR))/out/$(strip $(BOARD))/$(strip $(PROJ_NAME))/$(strip $(FEATURE_BASENAME)))
LIBDIR       :=  $(strip $(ROOTDIR))/prebuilt/dsp0/standard/ab156x
LIBDIR2      :=  $(strip $(ROOTDIR))/prebuilt/middleware/MTK/dspalg
LIBDIR3      :=  $(strip $(ROOTDIR))/prebuilt/middleware/third_party/dspalg
BUILD_LIBDIR :=  $(strip $(OUTDIR))/lib
LOGDIR       :=  $(strip $(OUTDIR))/log
ERR_LOG      :=  $(strip $(LOGDIR))/err.log
BUILD_LOG    :=  $(strip $(LOGDIR))/build.log

IMAGE        :=  $(strip $(OUTDIR))/$(strip $(PROJ_NAME)).out
BINARY       :=  $(strip $(OUTDIR))/$(strip $(PROJ_NAME)).bin
HEX          :=  $(strip $(OUTDIR))/$(strip $(PROJ_NAME)).hex
DEASM        :=  $(strip $(OUTDIR))/$(strip $(PROJ_NAME)).asm
MAP          :=  $(strip $(OUTDIR))/$(strip $(PROJ_NAME)).map


################################################################################
# Include Compiler and Commands Configuration
################################################################################
LDFLAG      +=  -wrap=malloc
LDFLAG      +=  -wrap=free
LDFLAG      +=  -wrap=calloc
LDFLAG      +=  -wrap=realloc
LDFLAG      +=  -wrap=_Assert
LDFLAG      +=  -wrap=_exit

include $(FEATURE)
include $(ROOTDIR)/.config.mk
include $(ROOTDIR)/config/chip/$(IC_CONFIG)/chip.mk

# Because the variables in the feature.mk file will be needed later,
# the entire logic code needs to be placed behind.
ifeq ($(TARGET), IRAM)
LSCRIPT      :=  dsp0_iram.lcf
endif
ifeq ($(TARGET), PSRAM)
LSCRIPT      :=  dsp0_psram.lcf
endif
ifeq ($(TARGET), FLASH)

ifdef LINKER_SCRIPT_FILE
LSCRIPT      :=  $(LINKER_SCRIPT_FILE)
else
LSCRIPT      :=  dsp0_flash.lcf
endif

endif

################################################################################
# Include Module Configuration
################################################################################
include $(ROOTDIR)/driver/chip/$(IC_CONFIG)/module.mk
include $(ROOTDIR)/kernel/service/exception_handler/module.mk
include $(ROOTDIR)/kernel/service/mux/module.mk
include $(ROOTDIR)/kernel/service/syslog/module.mk
include $(ROOTDIR)/kernel/service/pre_libloader/dsp0/module.mk
include $(ROOTDIR)/kernel/service/ccci/module.mk
include $(ROOTDIR)/kernel/service/platform/module.mk
include $(ROOTDIR)/kernel/service/heapsize_guard/module.mk
include $(ROOTDIR)/kernel/service/context_switch/$(IC_CONFIG)/module.mk
include $(ROOTDIR)/kernel/service/swla/module.mk
include $(ROOTDIR)/kernel/service/airo_clib/module.mk
include $(ROOTDIR)/kernel/rtos/FreeRTOS/Source/module.mk
#include $(ROOTDIR)/kernel/rtos/FreeRTOS_test/module.mk
include $(ROOTDIR)/middleware/MTK/dspfw/port/chip/$(IC_CONFIG)/module.mk
include $(ROOTDIR)/middleware/MTK/stream/module.mk
include $(ROOTDIR)/middleware/MTK/dspalg/src/module.mk
#include $(ROOTDIR)/middleware/third_party/dspalg/aac_decoder/module.mk
include $(ROOTDIR)/middleware/MTK/dspalg/aac_decoder/module.mk
include $(ROOTDIR)/middleware/MTK/dspalg/sbc_decoder/module.mk
include $(ROOTDIR)/middleware/MTK/dspalg/cvsd_decoder/module.mk
include $(ROOTDIR)/middleware/MTK/dspalg/msbc_decoder/module.mk
include $(ROOTDIR)/middleware/MTK/dspalg/compander/module.mk
include $(ROOTDIR)/middleware/MTK/dspalg/ec_nr/module.mk
include $(ROOTDIR)/middleware/MTK/dspalg/clk_skew/module.mk

ifeq ($(AIR_ANC_WIND_DETECTION_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/wind_detection/module.mk
endif

ifeq ($(AIR_ANC_USER_UNAWARE_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/user_unaware/module.mk
endif

ifeq ($(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/env_detect/module.mk
endif

ifneq ($(AIR_BT_CLK_SKEW_ENABLE), n)
ifeq ($(MTK_BT_CLK_SKEW_LOAD_ENABLE), y)
include $(ROOTDIR)/middleware/MTK/dspalg/clk_skew_protected/module.mk
else
include $(ROOTDIR)/prebuilt/middleware/MTK/dspalg/clk_skew/$(IC_CONFIG)/module.mk
endif
endif

include $(ROOTDIR)/middleware/MTK/dspalg/voice_plc/module.mk
ifeq ($(AIR_BT_CODEC_BLE_ENABLED), y)
include $(ROOTDIR)/middleware/MTK/dspalg/lc3_codec/module.mk
endif
ifeq ($(AIR_BT_CODEC_BLE_V2_ENABLED), y)
include $(ROOTDIR)/middleware/MTK/dspalg/lc3_codec/module.mk
endif
include $(ROOTDIR)/middleware/MTK/dspalg/peq/module.mk
include $(ROOTDIR)/middleware/MTK/dspalg/ins/module.mk
ifeq ($(MTK_BT_A2DP_VENDOR_ENABLE),y)
include $(ROOTDIR)/middleware/third_party/dspalg/vendor_decoder/module.mk
endif
ifeq ($(MTK_BT_A2DP_VENDOR_1_ENABLE),y)
include $(ROOTDIR)/middleware/third_party/dspalg/vendor_1_decoder/module.mk
endif
ifeq ($(MTK_ANC_ENABLE), y)
ifeq ($(MTK_ANC_LIB_ENABLE), y)
include $(ROOTDIR)/prebuilt/middleware/MTK/dspfw/anc/$(IC_CONFIG)/module.mk
else
include $(ROOTDIR)/middleware/MTK/dspfw/anc_protected/port/chip/$(IC_CONFIG)/module.mk
endif
endif
ifeq ($(MTK_WWE_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/wwe/module.mk
endif
ifeq ($(MTK_LEAKAGE_DETECTION_ENABLE), y)
include $(ROOTDIR)/middleware/MTK/dspalg/leakage_compensation/module.mk
endif
ifeq ($(MTK_USER_TRIGGER_FF_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/user_trigger_ff/user_trigger_adaptive_ff_v2/module.mk
endif
ifeq ($(MTK_AUDIO_LOOPBACK_TEST_ENABLE), y)
include $(ROOTDIR)/middleware/MTK/dspalg/audio_loopback_test/module.mk
endif

ifeq ($(MTK_BT_A2DP_AIRO_CELT_ENABLE),y)
include $(ROOTDIR)/middleware/third_party/dspalg/celt_decoder/module.mk
endif

ifeq ($(AIR_CELT_ENC_ENABLE),y)
include $(ROOTDIR)/middleware/third_party/dspalg/celt_encoder/module.mk
endif

ifeq ($(AIR_SOFTWARE_SRC_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/sw_src/module.mk
endif

ifeq ($(AIR_SOFTWARE_GAIN_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/sw_gain/module.mk
endif

ifeq ($(AIR_SOFTWARE_MIXER_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/sw_mixer/module.mk
endif

ifeq ($(AIR_SOFTWARE_BUFFER_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/sw_buffer/module.mk
endif

ifeq ($(AIR_IGO_NR_ENABLE),y)
include $(ROOTDIR)/prebuilt/middleware/third_party/dspalg/igo_nr/module.mk
endif
ifeq ($(AIR_IGO_NR_PLUS_1_ENABLE),y)
include $(ROOTDIR)/prebuilt/middleware/third_party/dspalg/igo_nr/module.mk
endif

ifeq ($(AIR_VOLUME_ESTIMATOR_ENABLE),y)
include $(ROOTDIR)/middleware/MTK/dspalg/volume_estimator/module.mk
endif

ifeq ($(AIR_FIXED_RATIO_SRC),y)
include $(ROOTDIR)/middleware/MTK/dspalg/src_fixed_ratio/module.mk
endif

ifeq ($(AIR_ADVANCED_PASSTHROUGH_ENABLE),y)
ifeq ($(AIR_ADVANCED_PASSTHROUGH_LIB_ENABLE),y)
include $(ROOTDIR)/prebuilt/middleware/MTK/dspfw/psap/$(IC_CONFIG)/module.mk
else
include $(ROOTDIR)/middleware/MTK/psap_protected/port/chip/$(IC_CONFIG)/module.mk
endif
endif

ifeq ($(AIR_AFC_ENABLE),y)
ifeq ($(AIR_AFC_LIB_ENABLE),y)
include $(ROOTDIR)/prebuilt/middleware/MTK/dspalg/afc/$(IC_CONFIG)/module.mk
else
include $(ROOTDIR)/middleware/MTK/dspalg/afc_protected/module.mk
endif
endif

ifeq ($(AIR_LD_NR_ENABLE),y)
ifeq ($(AIR_LD_NR_LIB_ENABLE),y)
include $(ROOTDIR)/prebuilt/middleware/MTK/dspalg/ld_nr/$(IC_CONFIG)/module.mk
else
include $(ROOTDIR)/middleware/MTK/dspalg/ld_nr_protected/module.mk
endif
endif

ifeq ($(AIR_AT_AGC_ENABLE),y)
ifeq ($(AIR_AT_AGC_LIB_ENABLE),y)
include $(ROOTDIR)/prebuilt/middleware/MTK/dspalg/at_agc/$(IC_CONFIG)/module.mk
else
include $(ROOTDIR)/middleware/MTK/dspalg/at_agc_protected/module.mk
endif
endif
################################################################################
# Project Sources/Define/Include Rule List
################################################################################
PROJ_C_SRC   +=  $(strip $(PROJ_PATH))/src/main.c
PROJ_C_SRC   +=  $(strip $(PROJ_PATH))/src/regions_init.c
PROJ_C_SRC   +=  $(strip $(PROJ_PATH))/src/sys_init.c
PROJ_C_SRC   +=  $(strip $(PROJ_PATH))/src/syscalls.c
PROJ_C_SRC   +=  $(strip $(PROJ_PATH))/src/preloader_pisplit_configure.c
PROJ_ASM_SRC +=  $(strip $(PROJ_PATH))/src/STARTUP.S
PROJ_INC     +=  $(strip $(PROJ_PATH))/inc/

ASM_SRC      +=  $(PROJ_ASM_SRC)
C_SRC        +=  $(PROJ_C_SRC)
DEFINE       +=  $(PROJ_DEFINE)
INC          +=  $(PROJ_INC)

# .o objects & .d dependency list
OBJ  =  $(ASM_SRC:%.S=$(OUTDIR)/%.o) $(C_SRC:%.c=$(OUTDIR)/%.o)
DEP  =  $(ASM_SRC:%.S=$(OUTDIR)/%.d) $(C_SRC:%.c=$(OUTDIR)/%.d)

################################################################################
# PIC lib, also can defined in module's module.mk
################################################################################
#PIC     += $(ROOTDIR)/project/xxxxx/templates/dsp_pisplit_library_build_demo/dsp0/Build/lib/dsp0_pisplit_library_build_demo.o
#PIC     += $(ROOTDIR)/project/xxxxx/templates/dsp_pisplit_library_build_demo/dsp1/Build/lib/dsp1_pisplit_library_build_demo.o
################################################################################
# Libraries Configuration
################################################################################

XTENSA_STANDARD_LIBS +=  $(strip $(LIBDIR))/libhal.a
XTENSA_STANDARD_LIBS +=  $(strip $(LIBDIR))/libc.a
XTENSA_STANDARD_LIBS +=  $(strip $(LIBDIR))/libgcc.a
XTENSA_STANDARD_LIBS +=  $(strip $(LIBDIR))/libhandlers-board.a
XTENSA_STANDARD_LIBS +=  $(strip $(LIBDIR))/libgloss.a
XTENSA_STANDARD_LIBS +=  $(strip $(LIBDIR))/libm.a

LIBS_BUILD  +=  $(strip $(BUILD_LIBDIR))/libctxsw.a

################################################################################
# Make Target
################################################################################
.PHONY: all clean
all : clean_log checkoption $(IMAGE)

dbg :
	make all CCFLAG='-g -W -O0' LDFLAG='--gc-sections' DEFINE='BUILD_DEBUG BUILD_IODBG BUILD_CATCH BUILD_UARTDUMP BUILD_USR_ALLOC CONFIG_OS_FREERTOS'

clean :
	rm -rf $(OUTDIR)

symtab :
	$(NM)  -gn > null
	perl symtabgen.pl null > $(SYMBOLTAB)

prebuilt_install:
	mkdir -p $(DEFAULT_BIN_PATH)/
	cp -rf $(BINARY) $(DEFAULT_BIN_PATH)/
	cp -rf $(IMAGE) $(DEFAULT_BIN_PATH)/
	cp -rf $(HEX) $(DEFAULT_BIN_PATH)/
	cp -rf $(MAP) $(DEFAULT_BIN_PATH)/
	cp -rf $(DEASM) $(DEFAULT_BIN_PATH)/
	cp -rf $(strip $(OUTDIR))/dsp*_log_str.bin $(DEFAULT_BIN_PATH)/

checkoption:
# AMA WWE : check the dependency of AMA WWE
ifeq ($(MTK_WWE_AMA_ENABLE), y)
ifeq ($(wildcard $(strip $(ROOTDIR))/prebuilt/middleware/MTK/dspalg/wwe/ama/$(IC_CONFIG)/),)
	$(warning "Error: [Addon error] AMA WWE lib is missing, please conntect with Amazon for the lib, and then refrence to dsp/prebuilt/middleware/MTK/dspalg/wwe/ama/pic_lib_generator/readme.txt to install the lib to the SDK")
	@echo "Error: [Addon error] AMA WWE lib is missing, please conntect with Amazon for the lib, and then refrence to dsp/prebuilt/middleware/MTK/dspalg/wwe/ama/pic_lib_generator/readme.txt to install the lib to the SDK";
	@set -e; exit 2;
endif
endif

MO_EXTRA = OUTDIR="$(OUTDIR)" PROJ_CCFLAG="$(CCFLAG)" PROJ_ASFLAG="$(ASFLAG)" PROJ_INC="$(INC)"

$(LIBS_BUILD) :
	make -C $(ROOTDIR)/kernel/service/context_switch/$(IC_CONFIG)/ $(MO_EXTRA)
$(IMAGE) : $(OBJ) $(LIBS) $(LIBS_BUILD)
	@echo Linking... $@
	@cp $(LSCRIPT) $(OUTDIR)
	@echo Libraries in link stage: $(LIBS) >> $(BUILD_LOG)

# Detect multiple definition:
# @$(LD) -o $@ $(OBJ) $(PIC) -Map $(MAP) -T $(LSCRIPT) $(LDFLAG) --start-group --whole-archive $(LIBS_BUILD) `echo $(LIBS) | sed 's/ /\n/g' | sort -u` $(XTENSA_STANDARD_LIBS) --no-whole-archive --end-group  2>> $(ERR_LOG)

	@$(LD) -o $@ $(OBJ) $(OUTDIR)/middleware/MTK/verno/verno.o $(PIC) \
		-Map $(MAP) -T $(LSCRIPT) $(LDFLAG) --start-group $(LIBS_BUILD) \
		--whole-archive `echo $(LIBS) | sed 's/ /\n/g' | sort -u` --no-whole-archive \
		$(XTENSA_STANDARD_LIBS) --end-group  2>> $(ERR_LOG)
	@$(OBJDUMP) -d $@ > $(DEASM)
	@$(OBJCOPY) -O ihex $@ $(HEX)
	@$(OBJCOPY) -O binary --remove-section=.log_str $@ $(BINARY)
	@$(OBJCOPY) -O binary --only-section=.log_str $@ $(OUTDIR)/dsp0_log_str.bin
	@echo Building Completed

################################################################################
# Include Rules
################################################################################
#Settings for the xtensa tool chain path & license for the project,
#the settings are used by $(ROOTDIR)/rule.mk

#XTENSA_ROOT =
#XTENSA_VER =
#XTENSA_LICENSE_FILE =
include $(ROOTDIR)/.rule.mk
-include $(DEP)


