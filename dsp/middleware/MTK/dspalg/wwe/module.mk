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


###################################################
WWE_FUNC_PATH = middleware/MTK/dspalg/wwe

ifeq ($(MTK_WWE_AMA_ENABLE),y)
LIBS += $(strip $(LIBDIR2))/wwe/inhouse/$(IC_CONFIG)/libwwe68_bf.a
else
ifeq ($(MTK_WWE_GSOUND_ENABLE),y)
LIBS += $(strip $(LIBDIR2))/wwe/inhouse/$(IC_CONFIG)/libwwe68_bf.a
endif
endif

ifeq ($(MTK_WWE_AMA_ENABLE),y)
	LIBS += $(strip $(LIBDIR2))/wwe/xtensa/$(IC_CONFIG)/lib_xt_dsp_1568.a
ifneq ($(MTK_WWE_AMA_USE_PIC),y)
	LIBS += $(strip $(LIBDIR2))/wwe/ama/$(IC_CONFIG)/libpryon_lite-PRL1000.a
else
    PIC   += $(strip $(LIBDIR2))/wwe/ama/$(IC_CONFIG)/pisplit/pisplit_library_pryon1000.o
endif

endif

ifeq ($(MTK_WWE_GSOUND_ENABLE),y)
ifneq ($(MTK_WWE_GSOUND_USE_PIC),y)
	LIBS += $(strip $(LIBDIR2))/wwe/gsound/$(IC_CONFIG)/libab1568_google_hotword_dsp_multi_bank_api.a
else
	PIC   += $(strip $(LIBDIR2))/wwe/gsound/$(IC_CONFIG)/pisplit/pisplit_google_hotword_dsp_multi_bank.o
endif

endif

C_SRC += $(WWE_FUNC_PATH)/src/wwe_interface.c
C_SRC += $(WWE_FUNC_PATH)/src/hwvad.c
C_SRC += $(WWE_FUNC_PATH)/src/preroll.c
#Temp for UT
C_SRC += $(WWE_FUNC_PATH)/src/dsp_nvkey_vad_comm.c
C_SRC += $(WWE_FUNC_PATH)/src/dsp_nvkey_vad_para.c

C_SRC += $(WWE_FUNC_PATH)/portable/wwe_portable.c

###################################################
# include path

INC += $(WWE_FUNC_PATH)/inc
INC += $(WWE_FUNC_PATH)/portable