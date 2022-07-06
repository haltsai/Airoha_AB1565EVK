/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef USB_CUSTOM_H
#define USB_CUSTOM_H

typedef struct {
    uint16_t bcdUSB;
    uint8_t  class;
    uint8_t  sub_class;
    uint8_t  protocol;
} usb_custom_class_info_t;

typedef struct {
    uint16_t vender_id;
    uint16_t product_id;
    uint16_t bcd_version;
} usb_custom_product_info_t;

typedef struct {
    bool     self_power;
    bool     remote_wakeup;
    uint16_t maxpower;
} usb_custom_power_info_t;

/* NVKEYID_USB_SETTING_0 */
#define USB_NEKEY_HS_ENABLE    0x01
#define USB_NEKEY_SPK2_FEATURE 0x02

/**
 * @brief usage of usb_string
 *
 * NOTE:
 *   Can't modify the value or remove item,
 *   because the values and items are linked with Airoha Tool and NVkeys.
 *   Can add a new item with a unique value, for new feature use.
 *
 * TODO: move to usb.h
 */
typedef enum {
    USB_STRING_USAGE_LANGSUPP        = 0,
    USB_STRING_USAGE_VENDOR          = 1,
    USB_STRING_USAGE_PRODUCT         = 2,
    USB_STRING_USAGE_SERIAL          = 3,
    USB_STRING_USAGE_AUDIO1          = 4,
    USB_STRING_USAGE_AUDIO2          = 5,
    USB_STRING_USAGE_HID             = 6,
    USB_STRING_USAGE_ACM_COMM        = 7,
    USB_STRING_USAGE_ACM_DATA        = 8,
    USB_STRING_USAGE_MSTEAMS_UCQ     = 10,
    USB_STRING_USAGE_XBOX            = 11,
} usb_string_usage_t;

typedef struct
{
    usb_string_usage_t usage;
    uint8_t id;
    const char *str;
} usb_custom_string_t;

/* TEAMS - UCQ string index 0x(33)*/
#define TEAMS_UCQ_STRING_WVALUE 0x21
#define TEAMS_UCQ_STRING_LENGTH 0x24

/* USB Logging */
void USB_Set_LoggingMode(bool mode);
bool USB_Get_LoggingMode(void);

/* XBOX - MS OS string, index 0xEE */
#ifdef MTK_USB_XBOX_ENABLE
#define XBOX_MS_OS_STRIG_WVALUE 0xEE
#define XBOX_MS_OS_STRIG_LENGTH 0x12

#define bMS_Vendor_Code         0x90
#define EXTENDED_COMPATIBLE_ID  0x04
#define EXTENDED_COMPATIBLE     0x05
#endif

/* Get device PID and VID */
uint16_t USB_GetDeviceVID(void);
uint16_t USB_GetDevicePID(void);

const usb_custom_string_t* USB_Custom_Get_String_List(uint8_t *length);
const usb_custom_class_info_t *USB_Custom_Get_Class_Info(void);
const usb_custom_product_info_t *USB_Custom_Get_Product_Info(void);
const usb_custom_power_info_t *USB_Custom_Get_Power_Info(void);

/**
 * Below section is summy code.
 * It's not used currently, only for reference.
 */
/* custom parameter for mass storage device*/
typedef struct
{
    const uint16_t    desc_product;
    const uint8_t     *inquire_data;
    const uint8_t     inquire_size;
    const uint16_t    *interface_string;
    const uint8_t     interface_size;
}USB_MS_PARAM;


#endif
