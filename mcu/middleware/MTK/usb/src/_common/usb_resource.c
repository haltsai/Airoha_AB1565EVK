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

#ifdef MTK_USB_DEMO_ENABLED

/* C library */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "assert.h"

/* USB Middleware includes */
#include "usb.h"
#include "usb_main.h"
#include "usb_resource.h"
#include "usb_custom.h"
#include "usb_custom_def.h"

#ifdef MTK_USB_XBOX_ENABLE
#include "usb_xbox.h"
#endif

/* Other Middleware includes */
#include "nvkey.h"
#include "nvkey_id_list.h"

/* Hal includes */
#include "hal_usb.h"

/* Macros */
/* TODO: move to right place */
#define USB_LANGUAGUE_ID            0x0409
#define USB_NVKEY_ADDR_START        0x3B00
#define USB_NVKEY_ADDR_SETTING_0    0x3B00
#define USB_NVKEY_ADDR_BASICINFO    0x3B10
#define USB_NVKEY_ADDR_STRING_NUM   0x3B11
#define USB_NVKEY_ADDR_STRING       (USB_NVKEY_ADDR_START + 0x00E0)

/* Static functions */
static void USB_Basic_Info_Init(void);
static void USB_Basic_Info_Init_Class_Info(const usb_custom_class_info_t *info);
static void USB_Basic_Info_Init_Product_Info(const usb_custom_product_info_t *info);
static void USB_Basic_Info_Init_Power_Info(const usb_custom_power_info_t *info);
static void USB_String_Init(void);
static void USB_String_Create_Language_Id(void);
static void USB_String_Init_From_Custom_List(void);

/* Syslog create module for usb_resource.c */
#include "syslog.h"
log_create_module_variant(USB_RESOURCE, DEBUG_LOG_ON, PRINT_LEVEL_INFO);

#ifdef USB_LOW_MEM_REQ
    #ifndef MTK_USB_AUDIO_MICROPHONE
        static uint8_t USB_TEST_Buffer[1024];
    #else
        static uint8_t USB_TEST_Buffer[1024];  // Need 259 bytes
    #endif
#else
    static uint8_t USB_TEST_Buffer[1024];
#endif

/* Standard Device Descriptor */
static const uint8_t devdscr[] = {
    USB_DEVDSC_LENGTH,                  //bLength
    USB_DEVICE,                         //bDescriptorType
    0x00,                               //bcdUSB
    0x00,                               //bcdUSB
    0x00,                               //bDeviceClass
    0x00,                               //bDeviceSubClass
    0x00,                               //bDeviceProtocol
    HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0, //bMaxPacketSize0
    0x00,                               //idVendor
    0x00,                               //idVendor
    0x00,                               //idProduct
    0x00,                               //idProduct
    0x00,                               //bcdDevice
    0x00,                               //bcdDevice
    0x00,                               //iManufacturer
    0x00,                               //iProduct
    0x00,                               //iSerialNumber
    0x01                                //bNumConfigurations
};

/* Initialize the descriptors for main configuration 1 */
static const uint8_t cfgdscr[] = {
    USB_CFGDSC_LENGTH,                  //bLength
    USB_CONFIG,                         //bDescriptorType
#if defined(MTK_USB_HID_ENABLE)
    0x09,                               //wTotalLength
#else
    0x00,                               //wTotalLength
#endif
    0x00,                               //wTotalLength
    0x00,                               //bNumInterfaces
    /* the value = (cfg index+1), select this config's number */
    0x01,                               //bConfigurationValue
    0x00,                               //iConfiguration

    /* bmAttributes */
#if defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_HID_ENABLE)
#ifdef AIR_USB_DONGLE_PROJECT_ENABLE
    USB_CFGDSC_ATTR_NATURE,             //bmAttributes
#else
    USB_CFGDSC_ATTR_NATURE | USB_CFGDSC_ATTR_SELFPOWER,             //bmAttributes
#endif
#else
    USB_CFGDSC_ATTR_NATURE,             //bmAttributes
#endif

    /* bMaxPower */
#if defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_HID_ENABLE)
    20/2                                //bMaxPower,20mA
#else
    USB_CFGDSC_MAXPOWER                 //bMaxPower
#endif
};

/* Initialize the Standard Device_Qualiifier Descriptor */
static const uint8_t dev_qual_dscr[] = {
    USB_DEV_QUA_DSC_LENGTH,             //bLength
    USB_DEVICE_QUALIFIER,               //bDescriptorType
    0x00, /* USB spec rev is 2.0 */     //bcdUSB
    0x02, /* USB spec rev is 2.0 */     //bcdUSB
    0x00,                               //bDeviceClass
    0x00,                               //bDeviceSubClass
    0x00,                               //bDeviceProtocol
    HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0, //bMaxPacketSize0
    0x00,                               //bNumConfigurations
    0x00                                //bReserved
};

/* Initialize the descriptors for other speed configuration */
static const uint8_t other_speed_cfg_dscr[] = {
    USB_OTHER_CFGDSC_LENGTH,            //bLength
    USB_OTHER_SPEED,                    //bDescriptorType
    0x00,                               //wTotalLength
    0x00,                               //wTotalLength
    0x00,                               //bNumInterfaces
    /*the value = (cfg index+1), select this config's number*/
    0x01,                               //bConfigurationValue
    0x00,                               //iConfiguration
    0x00,                               //bmAttributes
    0x00                                //MaxPower
};

/* interface specific create and init functions */
static Usb_IfCreate_Info usb_ifcreate_tbl[USB_MAX_INTERFACE];
static uint8_t usb_ifcreate_number = 0;

/* device code */
static Usb_Device_Code usb_device_code;

/* static functions */
static void USB_Resource_Reset(void);
#ifndef __USB_COMPOSITE_NO_SERIAL__
#endif

/* Register class specific create functions, class specific create function
   should create interface descriptor */
void USB_Register_CreateFunc(char *if_name,
                             usb_create_if_func_ptr if_create_func,
                             usb_void_func if_init_func,
                             usb_void_func if_enable_func,
                             usb_speed_if_func_ptr if_speed_func,
                             usb_void_func if_resume_func)
{
    if ((usb_ifcreate_number >= USB_MAX_INTERFACE) ||
        (if_create_func == NULL) ||
        (if_init_func == NULL)   ||
        (if_enable_func == NULL) ||
        (if_speed_func == NULL)) {
        LOG_MSGID_E(USB_RESOURCE, "USB_Register_CreateFunc : Fail", 0);
        return;
    }

    usb_ifcreate_tbl[usb_ifcreate_number].if_name_ptr = if_name;
    usb_ifcreate_tbl[usb_ifcreate_number].if_create_func = if_create_func;
    usb_ifcreate_tbl[usb_ifcreate_number].if_init_func = if_init_func;
    usb_ifcreate_tbl[usb_ifcreate_number].if_enable_func = if_enable_func;
    usb_ifcreate_tbl[usb_ifcreate_number].if_speed_func = if_speed_func;
    usb_ifcreate_tbl[usb_ifcreate_number].if_resume_func = if_resume_func;
    usb_ifcreate_number++;
}

/* Deregister class specific create functions, class specific device
   and product code should be called when cable plug out */
void USB_DeRegister_CreateFunc(void)
{
    usb_ifcreate_number = 0;
    usb_device_code.b_registerd = false;
}

static void USB_Check_Ep_Number_ErrorLog()
{
    LOG_MSGID_E(USB_RESOURCE, "USB_Check_Ep_Number intr_tx[%x] intr_rx[%x] iso_tx[%x] iso_rx[%x] bulk_tx[%x] bulk_rx[%x]", 6,
                gUsbDevice.resource_ep_intr_tx_number, gUsbDevice.resource_ep_intr_rx_number,
                gUsbDevice.resource_ep_iso_tx_number,  gUsbDevice.resource_ep_iso_rx_number,
                gUsbDevice.resource_ep_bulk_tx_number, gUsbDevice.resource_ep_bulk_rx_number);
}

static void USB_Check_Ep_Number()
{
    uint8_t ep_tx = gUsbDevice.resource_ep_intr_tx_number + gUsbDevice.resource_ep_iso_tx_number + gUsbDevice.resource_ep_bulk_tx_number;
    uint8_t ep_rx = gUsbDevice.resource_ep_intr_rx_number + gUsbDevice.resource_ep_iso_rx_number + gUsbDevice.resource_ep_bulk_rx_number;

    /* Check the valid endpoint number */
    if(ep_tx > HAL_USB_MAX_NUMBER_ENDPOINT_TX){
       LOG_MSGID_E(USB_RESOURCE, "USB_Check_Ep_Number ep_tx[%x]>MAX EP TX[%x]", 2, ep_tx, HAL_USB_MAX_NUMBER_ENDPOINT_TX);
       USB_Check_Ep_Number_ErrorLog();
    }

    if(ep_rx > HAL_USB_MAX_NUMBER_ENDPOINT_RX){
       LOG_MSGID_E(USB_RESOURCE, "USB_Check_Ep_Number ep_rx[%x]>MAX EP RX[%x]", 2, ep_rx, HAL_USB_MAX_NUMBER_ENDPOINT_RX);
       USB_Check_Ep_Number_ErrorLog();
    }

    /* Check the type of endpoint number for Usb_Ep_Info */
    if ((gUsbDevice.resource_ep_intr_tx_number > USB_MAX_EP_INTR_TX) ||
        (gUsbDevice.resource_ep_intr_rx_number > USB_MAX_EP_INTR_RX) ||
        (gUsbDevice.resource_ep_iso_tx_number  > USB_MAX_EP_ISO_TX)  ||
        (gUsbDevice.resource_ep_iso_rx_number  > USB_MAX_EP_ISO_RX)  ||
        (gUsbDevice.resource_ep_bulk_tx_number > USB_MAX_EP_BULK_TX) ||
        (gUsbDevice.resource_ep_bulk_rx_number > USB_MAX_EP_BULK_RX) ){
        LOG_MSGID_E(USB_RESOURCE, "USB_Check_Ep_Number type of endpoint number is invalid", 0);
        USB_Check_Ep_Number_ErrorLog();
    }
}

static void USB_Check_Descriptor()
{
    if ((gUsbDevice.resource_interface_number > USB_MAX_INTERFACE) ||
        (gUsbDevice.resource_string_number > USB_MAX_STRING)       ||
        (gUsbDevice.resource_iad_number > USB_MAX_IAD)) {
        LOG_MSGID_E(USB_RESOURCE, "USB_Check_Descriptor : Fail", 0);
    }
}

/* Get interrupt TX endpoint number and endpoint info pointer */
Usb_Ep_Info *USB_Get_Intr_Tx_Ep(uint8_t *p_num)
{
    /* Count from maxiumn of tx ep */
    *p_num = HAL_USB_MAX_NUMBER_ENDPOINT_TX - gUsbDevice.resource_ep_intr_tx_number;

    Usb_Ep_Info *p_ep = &gUsbDevice.ep_intr_tx_info[gUsbDevice.resource_ep_intr_tx_number];
    gUsbDevice.resource_ep_intr_tx_number++;

    USB_Check_Ep_Number();
    return p_ep;
}

/* Get interrupt RX endpoint number and endpoint info pointer */
Usb_Ep_Info *USB_Get_Intr_Rx_Ep(uint8_t *p_num)
{
    /* Count from maxiumn of rx ep */
    *p_num = HAL_USB_MAX_NUMBER_ENDPOINT_RX - gUsbDevice.resource_ep_intr_rx_number;

    Usb_Ep_Info *p_ep = &gUsbDevice.ep_intr_rx_info[gUsbDevice.resource_ep_intr_rx_number];
    gUsbDevice.resource_ep_intr_rx_number++;

    USB_Check_Ep_Number();
    return p_ep;
}

/* Get bulk TX endpoint number and endpoint info pointer */
Usb_Ep_Info *USB_Get_Bulk_Tx_Ep(uint8_t *p_num)
{
    Usb_Ep_Info *p_ep = &gUsbDevice.ep_bulk_tx_info[gUsbDevice.resource_ep_bulk_tx_number];
    gUsbDevice.resource_ep_bulk_tx_number++;

    *p_num = gUsbDevice.resource_ep_iso_tx_number + gUsbDevice.resource_ep_bulk_tx_number;

    USB_Check_Ep_Number();
    return p_ep;
}

/* Get bulk RX endpoint number and endpoint info pointer */
Usb_Ep_Info *USB_Get_Bulk_Rx_Ep(uint8_t *p_num)
{
    Usb_Ep_Info *p_ep = &gUsbDevice.ep_bulk_rx_info[gUsbDevice.resource_ep_bulk_rx_number];
    gUsbDevice.resource_ep_bulk_rx_number++;

    *p_num = gUsbDevice.resource_ep_bulk_rx_number + gUsbDevice.resource_ep_iso_rx_number;

    USB_Check_Ep_Number();
    return p_ep;
}

/* Get iso TX endpoint number and endpoint info pointer */
Usb_Ep_Info *USB_Get_Iso_Tx_Ep(uint8_t *p_num)
{
    Usb_Ep_Info *p_ep = &gUsbDevice.ep_iso_tx_info[gUsbDevice.resource_ep_iso_tx_number];
    gUsbDevice.resource_ep_iso_tx_number++;

    *p_num = gUsbDevice.resource_ep_bulk_tx_number + gUsbDevice.resource_ep_iso_tx_number;

    USB_Check_Ep_Number();
    return p_ep;
}

/* Get iso RX endpoint number and endpoint info pointer */
Usb_Ep_Info *USB_Get_Iso_Rx_Ep(uint8_t *p_num)
{
    Usb_Ep_Info *p_ep = &gUsbDevice.ep_iso_rx_info[gUsbDevice.resource_ep_iso_rx_number];
    gUsbDevice.resource_ep_iso_rx_number++;

    *p_num = gUsbDevice.resource_ep_bulk_rx_number + gUsbDevice.resource_ep_iso_rx_number;

    USB_Check_Ep_Number();
    return p_ep;
}

/* Get interface number and interface info pointer */
Usb_Interface_Info *USB_Get_Interface_Number(uint8_t *p_num)
{
    gUsbDevice.resource_interface_number++;
    USB_Check_Descriptor();
    *p_num = gUsbDevice.resource_interface_number - 1;
    return (&gUsbDevice.if_info[gUsbDevice.resource_interface_number - 1]);
}

Usb_IAD_Dscr *USB_Get_IAD_Number(void)
{
    gUsbDevice.resource_iad_number++;
    USB_Check_Descriptor();
    return (&gUsbDevice.iad_desc[gUsbDevice.resource_iad_number - 1]);
}

/* Create all descriptors, including device and class specific */
void USB_Software_Create(void)
{
    uint32_t  index_if = 0;
#if defined(__USB_SUPPORT_ISO_PIPE__)||defined(__MBIM_SUPPORT__)||defined(__ECM_SUPPORT__)
    uint32_t  index_if_alt = 0;
#endif
    uint32_t  index_ep;
    uint32_t  if_len = 0;
    /* initial resource number */
    USB_Resource_Reset();

    /**
     * String Descriptor Init
     * String Resource init first, cause basic info and interface will
     * reference to string.
     * e.g.
     *   - Device descriptor reference to Product and Vendor string.
     *   - Interface reference to each interface's name string.
     */
    USB_String_Init();

    /* check class has registered create function and device code */
    if ((usb_ifcreate_number == 0) || (usb_device_code.b_registerd == false)) {
        LOG_MSGID_E(USB_RESOURCE, "USB_Software_Create : check registered function Fail", 0);
    }

    /* call the create function for each class */
    for (index_if = 0; index_if < usb_ifcreate_number; index_if++) {
        if (usb_ifcreate_tbl[index_if].if_create_func != NULL) {
            usb_ifcreate_tbl[index_if].if_create_func(usb_ifcreate_tbl[index_if].if_name_ptr);
        }

        if ((gUsbDevice.if_info[index_if].ifdscr_size == 0) || (usb_ifcreate_tbl[index_if].if_create_func == NULL)) {
            LOG_MSGID_E(USB_RESOURCE, "USB_Software_Create : create function Fail", 0);
        }
    }

    /* Initialize the Standard Device Descriptor */
    memcpy(&(gUsbDevice.devdscr), devdscr, USB_DEVDSC_LENGTH);
    memcpy(&(gUsbDevice.cfgdscr), cfgdscr, USB_CFGDSC_LENGTH);
    USB_Basic_Info_Init();

    /* Base on different AP, overwrite current basic info*/
    #ifdef MTK_USB_XBOX_ENABLE
    if(Get_USB_Host_Type() == USB_HOST_TYPE_XBOX) {
        memcpy(&(gUsbDevice.devdscr), xbox_dev_descr, USB_DEVDSC_LENGTH);
        memcpy(&(gUsbDevice.cfgdscr), xbox_cfg_descr, USB_CFGDSC_LENGTH);
    }
    #endif

#if defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_HID_ENABLE) || defined(MTK_USB_XBOX_ENABLE)
    gUsbDevice.devdscr.iManufacturer = USB_String_Get_Id_By_Usage(USB_STRING_USAGE_VENDOR);
    gUsbDevice.devdscr.iProduct = USB_String_Get_Id_By_Usage(USB_STRING_USAGE_PRODUCT);
    gUsbDevice.devdscr.iSerialNumber = USB_String_Get_Id_By_Usage(USB_STRING_USAGE_SERIAL);
#else
    gUsbDevice.devdscr.iSerialNumber = 0x00; /* No serial number */
#endif
    /* Initialize the descriptors for main configuration 1 */
    gUsbDevice.cfgdscr.bNumInterfaces = gUsbDevice.resource_interface_number;

    /* Configuration length */
    for (index_if = 0; index_if < gUsbDevice.resource_iad_number; index_if++) {
        if_len += gUsbDevice.iad_desc[index_if].bLength;
    }

    for (index_if = 0; index_if < gUsbDevice.resource_interface_number; index_if++) {
        if_len += gUsbDevice.if_info[index_if].ifdscr_size;

        for (index_ep = 0; index_ep < gUsbDevice.if_info[index_if].ifdscr.stdif.bNumEndpoints; index_ep++) {
            if_len += gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size;
        }

#if defined(__USB_SUPPORT_ISO_PIPE__)||defined(__MBIM_SUPPORT__)||defined(__ECM_SUPPORT__)
        for (index_if_alt = 0; index_if_alt < USB_MAX_INTERFACE_ALTERNATE_NUM; index_if_alt++) {
            if(gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr_size > 0){
                if_len += gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr_size;

                for (index_ep = 0; index_ep < gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr.stdif.bNumEndpoints; index_ep++) {
                    if_len += gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ep_info[index_ep]->epdscr_size;
                }
            }
        }
#endif
    }
#if defined(MTK_USB_HID_ENABLE)
    /*add HID dscr length*/
    gUsbDevice.cfgdscr.wTotalLength = USB_CFGDSC_LENGTH + (USB_CFGDSC_LENGTH + if_len);
#else
    gUsbDevice.cfgdscr.wTotalLength = USB_CFGDSC_LENGTH + if_len;
#endif

    if(gUsbDevice.cfgdscr.wTotalLength > sizeof(USB_TEST_Buffer)){
        LOG_MSGID_E(USB_RESOURCE, "USB_Software_Create : cfgdscr.wTotalLength=%d bytes > sizeof(USB_TEST_BUFFER)=%d bytes Fail", 2, gUsbDevice.cfgdscr.wTotalLength, sizeof(USB_TEST_Buffer));
        while(1);
    }

    /* allocate configuration descriptor memory */
    if (gUsbDevice.conf == NULL) {
        gUsbDevice.conf = (uint8_t *)USB_Get_Memory(1024);//(uint8_t *)USB_TEST_Buffer;
        //gUsbDevice.conf = (uint8_t *)USB_TEST_Buffer;
    }

    /* Initialize the Standard Device_Qualiifier Descriptor */
    memcpy(&(gUsbDevice.dev_qual_dscr), dev_qual_dscr, USB_DEV_QUA_DSC_LENGTH);

    gUsbDevice.dev_qual_dscr.bDeviceClass = gUsbDevice.devdscr.bDeviceClass;
    gUsbDevice.dev_qual_dscr.bDeviceSubClass = gUsbDevice.devdscr.bDeviceSubClass;
    gUsbDevice.dev_qual_dscr.bDeviceProtocol = gUsbDevice.devdscr.bDeviceProtocol;
    gUsbDevice.dev_qual_dscr.bNumConfigurations = gUsbDevice.devdscr.bNumConfigurations;

    /* Initialize the descriptors for other speed configuration */
    memcpy(&(gUsbDevice.other_speed_cfg_dscr), other_speed_cfg_dscr, USB_OTHER_CFGDSC_LENGTH);

    gUsbDevice.other_speed_cfg_dscr.wTotalLength = gUsbDevice.cfgdscr.wTotalLength;
    gUsbDevice.other_speed_cfg_dscr.bNumInterfaces = gUsbDevice.cfgdscr.bNumInterfaces;
    gUsbDevice.other_speed_cfg_dscr.iConfiguration = 0;
    gUsbDevice.other_speed_cfg_dscr.bmAttributes = gUsbDevice.cfgdscr.bmAttributes;
    gUsbDevice.other_speed_cfg_dscr.MaxPower = gUsbDevice.cfgdscr.bMaxPower;

    USB_Software_Speed_Init(false);
}

#if defined(MTK_USB_HID_ENABLE)
extern uint8_t hid_dscr[];
#endif

void USB_Software_Speed_Init(bool b_other_speed)
{
    uint32_t  index_if = 0;
#if defined(__USB_SUPPORT_ISO_PIPE__)||defined(__MBIM_SUPPORT__)||defined(__ECM_SUPPORT__)
    uint32_t  index_if_alt = 0;
#endif
    uint32_t  index_ep;
    uint32_t  index_iad;
    uint32_t  Conf_index;
#ifndef MTK_USB11
    uint32_t  index;
#endif

    if (usb_ifcreate_number == 0) {
        LOG_MSGID_E(USB_RESOURCE, "USB_Software_Speed_Init : ifcreate_number = 0 Fail", 0);
    }

    if (gUsbDevice.conf == NULL) {
        assert(0);
    }

#ifndef MTK_USB11
    /* decide EP is high speed or full speed descriptors */
    for (index = 0; index < usb_ifcreate_number; index++) {
        if (usb_ifcreate_tbl[index].if_speed_func != NULL) {
            usb_ifcreate_tbl[index].if_speed_func(b_other_speed);
        } else {
            LOG_MSGID_E(USB_RESOURCE, "USB_Software_Speed_Init: usb_ifcreate_tbl is NULL Fail", 0);
        }
    }
#endif

    /* Re-construct configuration descriptor */
    if (b_other_speed == false) {
        memcpy((uint8_t *)(gUsbDevice.conf), (uint8_t *)&gUsbDevice.cfgdscr, USB_CFGDSC_LENGTH);
        Conf_index = USB_CFGDSC_LENGTH;
    } else {
        memcpy((uint8_t *)(gUsbDevice.conf), (uint8_t *)&gUsbDevice.other_speed_cfg_dscr, USB_OTHER_CFGDSC_LENGTH);
        Conf_index = USB_OTHER_CFGDSC_LENGTH;
    }

    if (gUsbDevice.resource_iad_number == 0) {
        /* No IAD, not VIDEO nor composite, either */
        for (index_if = 0; index_if < gUsbDevice.resource_interface_number; index_if++) {
            memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *)&gUsbDevice.if_info[index_if].ifdscr.stdif, gUsbDevice.if_info[index_if].ifdscr_size);
            Conf_index += gUsbDevice.if_info[index_if].ifdscr_size;

#if defined(MTK_USB_HID_ENABLE)
            memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *)hid_dscr, USB_IFDSC_LENGTH);
            Conf_index += USB_IFDSC_LENGTH;
#endif

            for (index_ep = 0; index_ep < gUsbDevice.if_info[index_if].ifdscr.stdif.bNumEndpoints; index_ep++) {
                memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *) & (gUsbDevice.if_info[index_if].ep_info[index_ep]->epdesc.stdep),
                       gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size);
                Conf_index += gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size;
            }

#if defined(__USB_SUPPORT_ISO_PIPE__)||defined(__MBIM_SUPPORT__)||defined(__ECM_SUPPORT__)
            for (index_if_alt = 0; index_if_alt < USB_MAX_INTERFACE_ALTERNATE_NUM; index_if_alt++) {
                if(gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr_size > 0){
                    memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *)&gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr.stdif, gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr_size);
                    Conf_index += gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr_size;

                    for (index_ep = 0; index_ep < gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr.stdif.bNumEndpoints; index_ep++) {
                        memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *) & (gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ep_info[index_ep]->epdesc.stdep), gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ep_info[index_ep]->epdscr_size);
                        Conf_index += gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ep_info[index_ep]->epdscr_size;
                    }
                }
            }
#endif
        }
    } else {
        for (index_iad = 0; index_iad < gUsbDevice.resource_iad_number; index_iad++) {
            memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *)&gUsbDevice.iad_desc[index_iad], gUsbDevice.iad_desc[index_iad].bLength);
            Conf_index += gUsbDevice.iad_desc[index_iad].bLength;

            for (index_if = gUsbDevice.iad_desc[index_iad].bFirstInterface; index_if < (gUsbDevice.iad_desc[index_iad].bFirstInterface + gUsbDevice.iad_desc[index_iad].bInterfaceCount); index_if++) {
                memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *)&gUsbDevice.if_info[index_if].ifdscr.stdif, gUsbDevice.if_info[index_if].ifdscr_size);
                Conf_index += gUsbDevice.if_info[index_if].ifdscr_size;

                for (index_ep = 0; index_ep < gUsbDevice.if_info[index_if].ifdscr.stdif.bNumEndpoints; index_ep++) {
                    memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *) & (gUsbDevice.if_info[index_if].ep_info[index_ep]->epdesc.stdep), gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size);
                    Conf_index += gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size;
                }

#if defined(__USB_SUPPORT_ISO_PIPE__)||defined(__MBIM_SUPPORT__)||defined(__ECM_SUPPORT__)
                for (index_if_alt = 0; index_if_alt < USB_MAX_INTERFACE_ALTERNATE_NUM; index_if_alt++) {
                    if(gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr_size > 0){
                        memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *)&gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr.stdif, gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr_size);
                        Conf_index += gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr_size;

                        for (index_ep = 0; index_ep < gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ifdscr.stdif.bNumEndpoints; index_ep++) {
                            memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *) & (gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ep_info[index_ep]->epdesc.stdep), gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ep_info[index_ep]->epdscr_size);
                            Conf_index += gUsbDevice.if_info[index_if].alternate_if_info[index_if_alt].ep_info[index_ep]->epdscr_size;
                        }
                    }
                }
#endif
            }
        }

    }

    if (Conf_index != gUsbDevice.cfgdscr.wTotalLength) {
        LOG_MSGID_E(USB_RESOURCE, "USB_Software_Speed_Init : wTotalLength %d is not match Conf_index:%d Fail", 2, gUsbDevice.cfgdscr.wTotalLength, Conf_index);
        while (1);
    }
}

/* init EP of each interface (class specific) */
void USB_Software_Init(void)
{
    uint32_t index;

    for (index = 0; index < usb_ifcreate_number; index++) {
        if (usb_ifcreate_tbl[index].if_init_func != NULL) {
            usb_ifcreate_tbl[index].if_init_func();
        } else {
            LOG_MSGID_E(USB_RESOURCE, "USB_Software_Init Fail", 0);
        }
    }
}

/* Resume EP of each interface (class specific) */
void USB_Software_Resume(void)
{
    uint32_t index;

    for (index = 0; index < usb_ifcreate_number; index++) {
        if (usb_ifcreate_tbl[index].if_resume_func != NULL) {
            usb_ifcreate_tbl[index].if_resume_func();
        }
    }
}


/* Init EP of each interface (class specific) */
void USB_Software_Enable(void)
{
    uint32_t index;

    for (index = 0; index < usb_ifcreate_number; index++) {
        if (usb_ifcreate_tbl[index].if_enable_func != NULL) {
            usb_ifcreate_tbl[index].if_enable_func();
        } else {
            LOG_MSGID_E(USB_RESOURCE, "USB_Software_Enable Fail", 0);
        }
    }
}

/* Reset the resource maintain in this file */
static void USB_Resource_Reset(void)
{
    uint32_t temp_addr, temp_addr2;

    temp_addr = (uint32_t)&gUsbDevice.resource_ep_bulk_tx_number;
    temp_addr2 = (uint32_t)&gUsbDevice.resource_iad_number;

    /* Reset part of gUsbDevice */
    memset(&gUsbDevice.resource_ep_bulk_tx_number, 0, temp_addr2 - temp_addr);
}

/************************************************************
    USB Basic Info Init
*************************************************************/
#define USB_BASICINFO_NVKEY_SIZE                18
#define USB_NVKEY_BASICINFO_OFFSET_USE_CLASS    0
#define USB_NVKEY_BASICINFO_OFFSET_USE_PRODUCT  1
#define USB_NVKEY_BASICINFO_OFFSET_USE_POWER    2
#define USB_NVKEY_BASICINFO_OFFSET_CLASS_INFO   3
#define USB_NVKEY_BASICINFO_OFFSET_PRODUCT_INFO 8
#define USB_NVKEY_BASICINFO_OFFSET_POWER_INFO   14
/**
 * The NVKEY layout of basic info shows in below image.
 * And its total length is 18 bytes.
 * First 3 bytes.
 * ------------------------------------------------
 * |     Use Nvkey(1) or Custom(0) value          |
 * | UseClassInfo | UseProductInfo | UsePowerInfo |
 * | 1 byte       | 1 byte         | 1 byte       |
 * | 0            | 1              | 2            |
 * ------------------------------------------------
 * And append 15 bytes.
 * ---------------------------------------------------------------------------
 * |       Class Info               | Product Info  |      Power Info        |
 * |bcdUSB |Class |SubClass|Protocol|VID|PID|version|selfPw|remoteWk|MaxPower|
 * |2 bytes|1 byte|1 byte  |1 byte  | 2 | 2 | 2bytes| 1    | 1      | 2      |
 * | 3     | 5    | 6      | 7      | 8 | 10| 12    | 14   | 15     | 16     |
 * ---------------------------------------------------------------------------
 */

static void USB_Basic_Info_Init(void)
{
    const usb_custom_class_info_t   *class_info = NULL;
    const usb_custom_product_info_t *product_info = NULL;
    const usb_custom_power_info_t   *power_info = NULL;

#if !defined(AG3335)
    nvkey_status_t status;
    uint32_t nvkey_size;
    uint8_t basic_info[USB_BASICINFO_NVKEY_SIZE];
    nvkey_size = sizeof(basic_info);
    status = nvkey_read_data(USB_NVKEY_ADDR_BASICINFO, basic_info, &nvkey_size);
    LOG_MSGID_I(USB_RESOURCE, "USB_Basic_Info_Init nvkey read status=%d", 1, status);
    LOG_MSGID_I(USB_RESOURCE, "USB_Basic_Info_Init USE_CLASS  =%d", 1, basic_info[USB_NVKEY_BASICINFO_OFFSET_USE_CLASS]);
    LOG_MSGID_I(USB_RESOURCE, "USB_Basic_Info_Init USE_PRODUCT=%d", 1, basic_info[USB_NVKEY_BASICINFO_OFFSET_USE_PRODUCT]);
    LOG_MSGID_I(USB_RESOURCE, "USB_Basic_Info_Init USE_POWER  =%d", 1, basic_info[USB_NVKEY_BASICINFO_OFFSET_USE_POWER]);

    if (status == NVKEY_STATUS_OK) {
        /* USB_NVKEY_BASICINFO_OFFSET_USE_CLASS is not open */
        /*
        if(basic_info[USB_NVKEY_BASICINFO_OFFSET_USE_CLASS]) {
            class_info = (usb_custom_class_info_t*)&basic_info[USB_NVKEY_BASICINFO_OFFSET_CLASS_INFO];
        }
        */
        if(basic_info[USB_NVKEY_BASICINFO_OFFSET_USE_PRODUCT]) {
            product_info = (usb_custom_product_info_t*)&basic_info[USB_NVKEY_BASICINFO_OFFSET_PRODUCT_INFO];
        }
        if(basic_info[USB_NVKEY_BASICINFO_OFFSET_USE_POWER]) {
            power_info = (usb_custom_power_info_t*)&basic_info[USB_NVKEY_BASICINFO_OFFSET_POWER_INFO];
        }
    }
#endif

    /* If not use info in NVKEY, use the info of custom. */
    if (class_info == NULL) {
        class_info = USB_Custom_Get_Class_Info();
    }
    if (product_info == NULL) {
        product_info = USB_Custom_Get_Product_Info();
    }
    if (power_info == NULL) {
        power_info = USB_Custom_Get_Power_Info();
    }

    USB_Basic_Info_Init_Class_Info(class_info);
    USB_Basic_Info_Init_Product_Info(product_info);
    USB_Basic_Info_Init_Power_Info(power_info);
}

static void USB_Basic_Info_Init_Class_Info(const usb_custom_class_info_t *info)
{
    gUsbDevice.devdscr.bcdUSB          = info->bcdUSB;
    gUsbDevice.devdscr.bDeviceClass    = info->class;
    gUsbDevice.devdscr.bDeviceSubClass = info->sub_class;
    gUsbDevice.devdscr.bDeviceProtocol = info->protocol;
}

static void USB_Basic_Info_Init_Product_Info(const usb_custom_product_info_t *info)
{
    gUsbDevice.devdscr.idProduct = info->product_id;
    gUsbDevice.devdscr.idVendor  = info->vender_id;
    gUsbDevice.devdscr.bcdDevice = info->bcd_version;
}

static void USB_Basic_Info_Init_Power_Info(const usb_custom_power_info_t *info)
{
    gUsbDevice.self_powered = info->self_power;
    gUsbDevice.remoteWk = info->remote_wakeup;

    gUsbDevice.cfgdscr.bmAttributes = USB_CFGDSC_ATTR_NATURE;
    if (info->self_power) {
        gUsbDevice.cfgdscr.bmAttributes |= USB_CFGDSC_ATTR_SELFPOWER;
    }
    if (info->remote_wakeup) {
        gUsbDevice.cfgdscr.bmAttributes |= USB_CFGDSC_ATTR_REMOTEWAKEUP;
    }

    /* Device max power = bMaxPower * 2 */
    gUsbDevice.cfgdscr.bMaxPower = info->maxpower/2;
}

/************************************************************
    USB String Functions
*************************************************************/
// This is a temp function
void USB_String_Init(void)
{
    uint8_t nvkey_string_num = 0;

#if !defined(AG3335)
    nvkey_status_t status;
    uint32_t nekey_size;
    nekey_size = sizeof(nvkey_string_num);
    status = nvkey_read_data(USB_NVKEY_ADDR_STRING_NUM , &nvkey_string_num, &nekey_size);
    LOG_MSGID_I(USB_RESOURCE, "USB_String_Init nvkey status=%d", 1, status);
#endif
    LOG_MSGID_I(USB_RESOURCE, "USB_String_Init nvkey_string_num=%d", 1, nvkey_string_num);
    if (nvkey_string_num) {
        USB_String_Nvkey_Read();
    }
    else {
        USB_String_Init_From_Custom_List();
    }
    USB_String_Create_Language_Id();
}

static void USB_String_Init_From_Custom_List(void)
{
    uint8_t numstr = 0;
    const usb_custom_string_t *custom_strlist;
    const usb_custom_string_t *s;
    int i;
    uint8_t auto_id = 1;

    custom_strlist = USB_Custom_Get_String_List(&numstr);
    for (i = 0; i < numstr; i++) {
        s = &custom_strlist[i];

        if (s->id == 0) {
            USB_String_Create(s->usage, auto_id, s->str);
            auto_id++;
        }
        else {
            USB_String_Create(s->usage, s->id, s->str);
        }
    }
}

void USB_String_Reset(void)
{
    gUsbDevice.resource_string_number = 0;
}

uint8_t USB_String_Create(usb_string_usage_t usage, uint8_t id, const char *str)
{
    int i = 0;
    usb_string_t* usbstr = &gUsbDevice.string[gUsbDevice.resource_string_number];

    usbstr->usage = usage;
    usbstr->id = id;

    for (i = 0; i < USB_STRING_MAX_LENGTH; i++) {
        if (str[i] == '\0') {
            break;
        }
        usbstr->desc.word[i] = str[i];
    }

    /**
     * length = 1 byte id + 1 byte desc_type + 2 bytes word * (i)
     *        = 2 + i * 2
     */
    usbstr->desc.length = 2 + i * 2;
    usbstr->desc.desc_type = USB_DEST_TYPE_STRING;
    gUsbDevice.resource_string_number++;
    return 0;
}

uint8_t USB_String_Get_Id_By_Usage(usb_string_usage_t usage)
{
    int i;
    uint8_t id = 0;
    uint8_t string_num = gUsbDevice.resource_string_number;

    for (i = 0; i < string_num; i++) {
        if (gUsbDevice.string[i].usage == usage) {
            id = gUsbDevice.string[i].id;
            break;
        }
    }
    return id;
}

usb_string_t* USB_String_Get_By_Usage(usb_string_usage_t usage)
{
    int i;
    usb_string_t* ret = NULL;
    uint8_t string_num = gUsbDevice.resource_string_number;

    for (i = 0; i < string_num; i++) {
        if (gUsbDevice.string[i].usage == usage) {
            ret = &(gUsbDevice.string[i]);
            break;
        }
    }

    if (ret == NULL) {
        LOG_MSGID_E(USB_RESOURCE, "USB_String_Get_By_Usage can't find matched USB_String", 0);
        assert(0);
    }

    return ret;
}

usb_string_t* USB_String_Get_By_Id(uint8_t id)
{
    int i;
    usb_string_t* ret = NULL;
    uint8_t string_num = gUsbDevice.resource_string_number;

    for (i = 0; i < string_num; i++) {
        if (gUsbDevice.string[i].id == id) {
            ret = &(gUsbDevice.string[i]);
            break;
        }
    }

    return ret;
}

#define USB_STRING_NVKEY_SIZE 34
/**
 * @brief Write one usb string to NVKEY.
 *
 * The NVKEY layout of usb string shows in below image.
 * And its total length is 2 + 1 + 31 = 34 bytes.
 * ----------------------------------
 * | uasge   | id     | char string |
 * | 2 bytes | 1 byte | 31 bytes    |
 * ----------------------------------
 */
#if !defined(AG3335)
static int USB_String_Nvkey_Write_One(uint16_t nvkey, usb_string_t *usbstr)
{
    int i;
    uint8_t data[USB_STRING_NVKEY_SIZE];

    data[0] = usbstr->usage & 0xFF;
    data[1] = usbstr->usage >> 8;
    data[2] = usbstr->id;

    for (i = 0; i < USB_MAX_STRING; i++) {
        data[3 + i] = usbstr->desc.word[i] & 0xFF;
    }

    return nvkey_write_data(nvkey, data, USB_STRING_NVKEY_SIZE);
}

void USB_String_Nvkey_Write(void)
{
    nvkey_status_t status;
    int i;
    uint8_t string_num;
    uint32_t nekey_size;

    string_num = gUsbDevice.resource_string_number;
    nekey_size = sizeof(string_num);
    status =  nvkey_write_data(USB_NVKEY_ADDR_STRING_NUM, &string_num, nekey_size);
    LOG_MSGID_I(USB_RESOURCE, "USB_String_Nvkey_Write status=%d, string_num=%d", 2, status, string_num);

    for (i = 0; i < string_num; i++) {
        status = USB_String_Nvkey_Write_One(USB_NVKEY_ADDR_STRING + i, &gUsbDevice.string[i]);
    }
}

void USB_String_Nvkey_Read(void)
{
    nvkey_status_t status;
    uint32_t nekey_size;
    int i;

    uint8_t string_num = 0;
    uint8_t data[USB_STRING_NVKEY_SIZE];

    nekey_size = sizeof(string_num);
    status =  nvkey_read_data(USB_NVKEY_ADDR_STRING_NUM , &string_num, &nekey_size);
    LOG_MSGID_I(USB_RESOURCE, "USB_String_Nvkey_Read status=%d, string_num=%d", 2, status, string_num);

    for (i = 0; i < string_num; i++) {
        nekey_size = USB_STRING_NVKEY_SIZE;
        status =  nvkey_read_data(USB_NVKEY_ADDR_STRING + i, data, &nekey_size);
        USB_String_Create(data[0] + (data[1] << 8), data[2], (char*)&data[3]);
    }
}
#else
static int USB_String_Nvkey_Write_One(uint16_t nvkey, usb_string_t *usbstr) {
    return 0;
}

void USB_String_Nvkey_Write(void) {
    return;
}

void USB_String_Nvkey_Read(void) {
    return;
}
#endif

static void USB_String_Create_Language_Id(void)
{
    USB_String_Create(USB_STRING_USAGE_LANGSUPP, 0, "");
    USB_String_Get_By_Usage(USB_STRING_USAGE_LANGSUPP)->desc.word[0] = USB_LANGUAGUE_ID;
    /* length = 1 byte id + 1 byte desc_type + 2 bytes LANGUAGUE_ID = 4 bytes */
    USB_String_Get_By_Usage(USB_STRING_USAGE_LANGSUPP)->desc.length = 0x04;
}

#endif //MTK_USB_DEMO_ENABLED
