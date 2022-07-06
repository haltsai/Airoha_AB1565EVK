# Copyright Statement:
#
# (C) 2005-2016  MediaTek Inc. All rights reserved.
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
# Without the prior written permission of MediaTek and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
# You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
# if you have agreed to and been bound by the applicable license agreement with
# MediaTek ("License Agreement") and been granted explicit permission to do so within
# the License Agreement ("Permitted User").  If you are not a Permitted User,
# please cease any access or use of MediaTek Software immediately.
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#

FEATURE ?= feature.mk
include $(FEATURE)


AIR_USB_AUDIO_V1_ENABLE ?= $(MTK_USB_AUDIO_V1_ENABLE)
AIR_USB_AUDIO_V2_ENABLE ?= $(MTK_USB_AUDIO_V2_ENABLE)
AIR_USB_AUDIO_MICROPHONE_ENABLE ?= $(MTK_USB_AUDIO_MICROPHONE)
AIR_USB_AUDIO_2ND_SPEAKER_ENABLE ?= $(MTK_USB_AUDIO_2ND_SPEAKER)
AIR_USB_AUDIO_HID_ENABLE ?= $(MTK_USB_AUDIO_HID_ENABLE)



USB_CDC_ACM_ENABLE = y

ifeq ($(USB_CDC_ACM_ENABLE), y)
CFLAGS += -DUSB_CDC_ACM_ENABLE
endif

ifeq ($(IC_CONFIG), $(filter $(IC_CONFIG),ab155x am255x ag3335 mt2822 ab156x))
USB_MASS_STORAGE_ENABLE = n
else
USB_MASS_STORAGE_ENABLE = y
endif

ifeq ($(MTK_USB_MS_ENABLED), y)
USB_MASS_STORAGE_ENABLE = y
CFLAGS += -DMTK_USB_BOOT_MSC
endif

###################################################
# Sources
USB_SRC = middleware/MTK/usb/src

USB_SRC_FILES += $(USB_SRC)/_common/usb_main.c \
             $(USB_SRC)/_common/usb_resource.c \
             $(USB_SRC)/_common/usb_custom.c \
             $(USB_SRC)/_common/usb.c \
             $(USB_SRC)/_common/usb_case.c \
             $(USB_SRC)/_common/usb_host_detect.c \
             $(USB_SRC)/_common/usb_acti.c


#########################################################
# AIR_USB_AUDIO_VERSION is used in dongle project       #
#########################################################
ifeq ($(AIR_USB_AUDIO_VERSION), v1)
CFLAGS += -DMTK_USB_AUDIO_V1_ENABLE
USB_SRC_FILES += $(USB_SRC)/audio/usbaudio_drv.c
ifeq ($(AIR_USB_AUDIO_MICROPHONE_ENABLE), y)
CFLAGS += -DMTK_USB_AUDIO_MICROPHONE
endif

else ifeq ($(AIR_USB_AUDIO_VERSION), v2)
CFLAGS += -DMTK_USB_AUDIO_V2_ENABLE
USB_SRC_FILES += $(USB_SRC)/audio/usbaudio_drv.c
ifeq ($(AIR_USB_AUDIO_MICROPHONE_ENABLE), y)
CFLAGS += -DMTK_USB_AUDIO_MICROPHONE
endif

else
# AIR_USB_AUDIO_VERSION = none
endif

#########################################################
# AIR_USB_AUDIO_V1_ENABLE and AIR_USB_AUDIO_V2_ENABLE   #
# are used in earbuds / headset project                 #
#########################################################
ifeq ($(AIR_USB_AUDIO_V1_ENABLE), y)
CFLAGS += -DMTK_USB_AUDIO_V1_ENABLE
ifeq ($(AIR_USB_AUDIO_MICROPHONE_ENABLE), y)
CFLAGS += -DMTK_USB_AUDIO_MICROPHONE
endif
USB_SRC_FILES += $(USB_SRC)/audio/usbaudio_drv.c
endif

ifeq ($(AIR_USB_AUDIO_V2_ENABLE), y)
CFLAGS += -DMTK_USB_AUDIO_V2_ENABLE
ifeq ($(AIR_USB_AUDIO_MICROPHONE_ENABLE), y)
CFLAGS += -DMTK_USB_AUDIO_MICROPHONE
endif
USB_SRC_FILES += $(USB_SRC)/audio/usbaudio_drv.c
endif

ifeq ($(AIR_USB_AUDIO_HID_ENABLE), y)
CFLAGS += -DMTK_USB_AUDIO_HID_ENABLE
USB_SRC_FILES += $(USB_SRC)/audio/usbaudio_drv.c
endif


ifeq ($(MTK_USB_HID_ENABLE), y)
USB_SRC_FILES += $(USB_SRC)/hid/usbhid_adap.c \
                 $(USB_SRC)/hid/usbhid_drv.c
endif

ifeq ($(USB_CDC_ACM_ENABLE), y)
USB_SRC_FILES += $(USB_SRC)/cdc/usbacm_drv.c \
             $(USB_SRC)/cdc/usbacm_adap.c
endif

ifeq ($(AIR_USB_XBOX_ENABLE), y)
CFLAGS += -DMTK_USB_XBOX_ENABLE
USB_SRC_FILES += $(USB_SRC)/xbox/usb_xbox.c
endif

ifeq ($(USB_MASS_STORAGE_ENABLE), y)
CFLAGS += -D__USB_MASS_STORAGE_ENABLE__
USB_SRC_FILES += $(USB_SRC)/msc/usbms_adap.c \
                 $(USB_SRC)/msc/usbms_drv.c \
                 $(USB_SRC)/msc/usbms_state.c \
                 $(USB_SRC)/msc/usbms_msdc.c \
                 $(USB_SRC)/msc/USBMS_snand.c
endif

CFLAGS += -DMTK_USB_ENABLED

ifeq ($(IC_CONFIG), mt2625)
CFLAGS += -DMTK_USB11
CFLAGS += -DMTK_USB2UART_DMA_THRU
endif

ifeq ($(MTK_USB_HID_ENABLE), y)
CFLAGS += -DMTK_USB_HID_ENABLE
endif

ifeq ($(AIR_USB_AUDIO_2ND_SPEAKER_ENABLE), y)
CFLAGS += -DMTK_USB_AUDIO_2ND_SPEAKER
endif


ifeq ($(IC_CONFIG), $(filter $(IC_CONFIG),ab155x am255x ag3335 mt2822 ab156x))
CFLAGS += -DUSB_LOW_MEM_REQ
endif

CFLAGS += -D__MTK_TARGET__

C_FILES += $(USB_SRC_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/usb/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/serial_nand/inc
CFLAGS += -I$(SOURCE_DIR)/driver/chip/inc
