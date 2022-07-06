/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */


#ifdef MTK_USB_DEMO_ENABLED
#ifdef MTK_USB_XBOX_ENABLE

/* C library */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Middleware includes */
#include "usb_xbox.h"
#include "usb_custom.h"
#include "usb_custom_def.h"

/* Hal includes */
#include "hal_pmu.h"

/* Syslog create module for usb_xbox.c */
#include "syslog.h"
log_create_module_variant(USB_XBOX, DEBUG_LOG_ON, PRINT_LEVEL_INFO);

/*******************************************************
*   XBOX Gobal variable
*******************************************************/
Usb_XBOX_Struct_Info g_Usb_XBOX_Ctrl;
Usb_XBOX_Struct_Info g_Usb_XBOX_Audio;
Usb_XBOX_Struct_CB   g_Usb_XBOX_CB;

/*******************************************************
*   XBOX Descriptor
*******************************************************/
#ifdef AIR_USB_DONGLE_PROJECT_ENABLE
#define XBOX_DEV_VID 0x0003
#else
#define XBOX_DEV_VID 0x0809
#endif
#define XBOX_DEV_BCDDEVICE 0x0100

/* Device Descriptor */
const uint8_t xbox_dev_descr[] = {
    USB_DEVDSC_LENGTH,                  // bLength
    USB_DEVICE,                         // bDescriptorType
    0x00, /* USB spec rev is 2.0 */     // bcdUSB
    0x02, /* USB spec rev is 2.0 */     // bcdUSB
    0xFF, /* Vendor specific */         // bDeviceClass
    0x47, /* Vendor specific */         // bDeviceSubClass
    0xD0, /* Vendor specific */         // bDeviceProtocol
    HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0, // bMaxPacketSize0
   (uint8_t)(USB_VID),                  // idVendor
   (uint8_t)(USB_VID >> 8),             // idVendor
   (uint8_t)(XBOX_DEV_VID),             // idProduct
   (uint8_t)(XBOX_DEV_VID >> 8),        // idProduct
   (uint8_t)(XBOX_DEV_BCDDEVICE),       // bcdDevice
   (uint8_t)(XBOX_DEV_BCDDEVICE >> 8),  // bcdDevice
    0x00,                               // iManufacturer
    0x00,                               // iProduct
    0x00,                               // iSerialNumber
    0x01                                // bNumConfigurations
};

/* Configuration Descriptor */
const uint8_t xbox_cfg_descr[] = {
    USB_CFGDSC_LENGTH,                  // bLength
    USB_CONFIG,                         // bDescriptorType
    0x00,                               // wTotalLength
    0x00,                               // wTotalLength
    0x02, /* 0x01:w/o 0x02:w/ audio */  // bNumInterfaces
    0x01, /* value to use this cfg */   // bConfigurationValue
    0x00,                               // iConfiguration
    USB_CFGDSC_ATTR_NATURE,             // bmAttributes
    USB_CFGDSC_MAXPOWER /* 20mA*/       // bMaxPower
};

/* GIP Data Interface Descriptor */
const uint8_t xbox_control_if_dscr[] =
{
    USB_IFDSC_LENGTH,                   // bLength
    USB_INTERFACE,                      // bDescriptorType
    0x00,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    0x02, /* Control TXRX */            // bNumEndpoints
    0xFF, /* Vendor specific */         // bInterfaceClass
    0x47, /* GIP */                     // bInterfaceSubClass
    0xD0, /* GIP */                     // bInterfaceProtocol
    0x00                                // iInterface
};

/* GIP Data Interrupt OUT Endpoint Descriptor */
const uint8_t xbox_control_ep_out_dscr[] =
{
    USB_EPDSC_LENGTH,                   // bLength
    USB_ENDPOINT,                       // bDescriptorType
    0x00,                               // bEndpointAddress
    USB_EP_INTR, /* Interrupt */        // bmAttributes
    0x40, /* 64 byte */                 // wMaxPacketSize[0]
    0x00,                               // wMaxPacketSize[1]
    0x04                                // bInterval
};

/* GIP Data Interrupt IN Endpoint Descriptor */
const uint8_t xbox_control_ep_in_dscr[] =
{
    USB_EPDSC_LENGTH,                   // bLength
    USB_ENDPOINT,                       // bDescriptorType
    0x00,                               // bEndpointAddress
    USB_EP_INTR, /* Interrupt */        // bmAttributes
    0x40, /* 64 byte */                 // wMaxPacketSize[0]
    0x00,                               // wMaxPacketSize[1]
    0x04                                // bInterval
};

/* GIP Audio Interface Descriptor */
const uint8_t xbox_audio_if_dscr[] =
{
    USB_IFDSC_LENGTH,                   // bLength
    USB_INTERFACE,                      // bDescriptorType
    0x00,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    0x00,                               // bNumEndpoints
    0xFF, /* Vendor specific */         // bInterfaceClass
    0x47, /* GIP */                     // bInterfaceSubClass
    0xD0, /* GIP */                     // bInterfaceProtocol
    0x00                                // iInterface
};

/* GIP Audio Interface Descriptor Alternate */
const uint8_t xbox_audio_if_alt_dscr[] =
{
    USB_IFDSC_LENGTH,                   // bLength
    USB_INTERFACE,                      // bDescriptorType
    0x00,                               // bInterfaceNumber
    0x01,                               // bAlternateSetting
    0x02, /* Audio TXRX */              // bNumEndpoints
    0xFF, /* Vendor specific */         // bInterfaceClass
    0x47, /* GIP */                     // bInterfaceSubClass
    0xD0, /* GIP */                     // bInterfaceProtocol
    0x00                                // iInterface
};

/* GIP Audio Isochronous OUT Endpoint Descriptor */
const uint8_t xbox_audio_ep_out_dscr[] =
{
    USB_EPDSC_LENGTH,                   // bLength
    USB_ENDPOINT,                       // bDescriptorType
    0x00,                               // bEndpointAddress
    USB_EP_ISO, /* Isochronous */       // bmAttributes
    0xE4, /* 228 byte */                // wMaxPacketSize[0]
    0x00,                               // wMaxPacketSize[1]
    0x04                                // bInterval
};

/* GIP Audio Isochronous IN Endpoint Descriptor */
const uint8_t xbox_audio_ep_in_dscr[] =
{
    USB_EPDSC_LENGTH,                   // bLength
    USB_ENDPOINT,                       // bDescriptorType
    0x00,                               // bEndpointAddress
    USB_EP_ISO, /* Isochronous */       // bmAttributes
    0x40, /* 64 byte */                 // wMaxPacketSize[0]
    0x00,                               // wMaxPacketSize[1]
    0x04                                // bInterval
};

/* Windows OS Descriptor */
/* Extended Compatible ID Descriptor */
const uint8_t xbox_ext_com_ID_dscr[] =
{
    0x28, 0x00, 0x00, 0x00,             // dwLength   : Descriptor length in bytes
    0x00, 0x01,                         // bcdVersion : Version 1.0
    0x04, 0x00,                         // wIndex     : Extended Configuration Descriptor
    0x01,                               // bCount     : Total number of Function Sections that follow the Header Section
    0x00, 0x00, 0x00, 0x00,             // RESERVED   : Reserved
    0x00, 0x00, 0x00,
    0x00,                               // bFirstInterfaceNumber : Starting Interface Number for this function.
    0x02,                               // bNumInterfaces  : 0x01: GIP game controller without audio; 0x02: GIP game controller with audio
    0x58, 0x47, 0x49, 0x50,             // compatibleID    : 'XGIP10' designates Xbox One compatible device. Pad to 8 bytes with 0x00.
    0x31, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,             // subCompatibleID : Secondary compatible ID (none)
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,             // RESERVED   : Reserved
    0x00, 0x00
};

/* Dummy Request to Determine Packet Length */
const uint8_t xbox_ext_com_dscr[] =
{
    0x0A, 0x00, 0x00, 0x00,         // dwLength   : Descriptor length in bytes
    0x00, 0x01,                     // bcdVersion : Version 1.0
    0x04, 0x00,                     // wIndex     : extended properties OS descriptor
    0x00, 0x00                      // wCount     : The number of custom property sections that follow the header section
};

/*******************************************************
*   Initialize and Rlease Function
*******************************************************/
void USB_Init_XBOX_Status(void)
{
    LOG_MSGID_I(USB_XBOX, "USB_Init_XBOX_Status", 0);

    g_Usb_XBOX_Ctrl.if_id  = USB_UNUSAGED_IF_ID;
    g_Usb_XBOX_Audio.if_id = USB_UNUSAGED_IF_ID;

    g_Usb_XBOX_CB.initialized = true;
}

void USB_Release_XBOX_Status(void)
{
    USB_Init_XBOX_Status();
}

/*******************************************************
*   Register Control & Audio Callback
*******************************************************/
void USB_XBOX_Control_Register_Rx_Callback(XBOX_CTRL_RX_FUNC rx_cb)
{
    if (rx_cb != NULL) {
        LOG_MSGID_I(USB_XBOX, "USB_XBOX_Control_Register_Rx_Callback rx_cb[%x]", 1, rx_cb);
        g_Usb_XBOX_CB.ctrl_rx_cb = rx_cb;
    } else {
        LOG_MSGID_E(USB_XBOX, "USB_XBOX_Control_Register_Rx_Callback rx_cb is NULL!!",0);
        g_Usb_XBOX_CB.ctrl_rx_cb = NULL;
    }
}

void USB_XBOX_Audio_Register_Rx_Callback(XBOX_AUDIO_RX_FUNC rx_cb)
{
    if (rx_cb != NULL) {
        LOG_MSGID_I(USB_XBOX, "USB_XBOX_Audio_Register_Rx_Callback rx_cb[%x]", 1, rx_cb);
        g_Usb_XBOX_CB.audio_rx_cb = rx_cb;
    } else {
        LOG_MSGID_E(USB_XBOX, "USB_XBOX_Audio_Register_Rx_Callback rx_cb is NULL!!",0);
        g_Usb_XBOX_CB.audio_rx_cb = NULL;
    }
}

void USB_XBOX_Audio_Register_Tx_Callback(XBOX_AUDIO_TX_FUNC tx_cb)
{
    if (tx_cb != NULL) {
        LOG_MSGID_I(USB_XBOX, "USB_XBOX_Audio_Register_Tx_Callback tx_cb[%x]", 1, tx_cb);
        g_Usb_XBOX_CB.audio_tx_cb = tx_cb;
    } else {
        LOG_MSGID_E(USB_XBOX, "USB_XBOX_Audio_Register_Tx_Callback tx_cb is NULL!!",0);
        g_Usb_XBOX_CB.audio_tx_cb = NULL;
    }
}

/*******************************************************
*   Control & Audio Endpoint Handler
*******************************************************/
uint8_t ctrl_rx_buf[USB_XBOX_CTRL_RX_BUFFER_LEN] = {0};
/* EP Intr Out interrupt handler, called by EP interrupt */
void USB_XBOX_Ctrl_IntrOut_Hdr(void)
{
    uint32_t i, len;

    /* Get data length from EP */
    len = hal_usb_get_rx_packet_length(g_Usb_XBOX_Ctrl.ep_out_id);

    LOG_MSGID_I(USB_XBOX, "USB_XBOX_Ctrl_IntrOut_Hdr len[%x] callback[%x]", 2, len, g_Usb_XBOX_CB.ctrl_rx_cb);

    /* Get data from EP */
    hal_usb_read_endpoint_fifo(g_Usb_XBOX_Ctrl.ep_out_id, len, ctrl_rx_buf);
    hal_usb_set_endpoint_rx_ready(g_Usb_XBOX_Ctrl.ep_out_id);

    for(i=0; i<len; i+=8){
        LOG_MSGID_I(USB_XBOX, "USB XBOX Ctrl RX [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", 8,
                               ctrl_rx_buf[i+0], ctrl_rx_buf[i+1], ctrl_rx_buf[i+2], ctrl_rx_buf[i+3],
                               ctrl_rx_buf[i+4], ctrl_rx_buf[i+5], ctrl_rx_buf[i+6], ctrl_rx_buf[i+7]);
    }

    if(g_Usb_XBOX_CB.ctrl_rx_cb != NULL){
        g_Usb_XBOX_CB.ctrl_rx_cb(ctrl_rx_buf, len);
    } else {
        LOG_MSGID_E(USB_XBOX, "USB_XBOX_Ctrl_IntrOut_Hdr callback is NULL!!", 0);
    }

    /* Reset control rx buffer */
    memset(ctrl_rx_buf, 0, USB_XBOX_CTRL_RX_BUFFER_LEN);
}

/* EP Intr In interrupt handler, called by EP interrupt */
void USB_XBOX_Ctrl_IntrIn_Hdr(void)
{
    /* Print debug log */
    LOG_MSGID_I(USB_DEBUG, "USB_XBOX_Ctrl_IntrIn_Hdr", 0);

    /* no thing to do... */
}

/* EP Iso Out interrupt handler, called by EP interrupt */
uint8_t audio_rx_buf[USB_XBOX_AUDIO_RX_BUFFER_LEN] = {0};
void USB_XBOX_Audio_IsoOut_Hdr(void)
{
    /* Get data length from EP */
    uint32_t len;
    len = hal_usb_get_rx_packet_length(g_Usb_XBOX_Audio.ep_out_id);

    /* Print debug log */
    LOG_MSGID_I(USB_DEBUG, "USB_XBOX_Audio_IsoOut_Hdr len[%x] callback[%x]", 2, len, g_Usb_XBOX_CB.ctrl_rx_cb);

    /* Get data from EP */
    hal_usb_read_endpoint_fifo(g_Usb_XBOX_Audio.ep_out_id, len, audio_rx_buf);
    hal_usb_set_endpoint_rx_ready(g_Usb_XBOX_Audio.ep_out_id);

    /* Print debug log */
    LOG_MSGID_I(USB_DEBUG, "USB XBOX Audio RX [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", 8,
                            audio_rx_buf[0], audio_rx_buf[1], audio_rx_buf[2], audio_rx_buf[3],
                            audio_rx_buf[4], audio_rx_buf[5], audio_rx_buf[6], audio_rx_buf[7]);

    if(g_Usb_XBOX_CB.audio_rx_cb != NULL){
        g_Usb_XBOX_CB.audio_rx_cb(audio_rx_buf, len);
    } else {
        LOG_MSGID_E(USB_XBOX, "USB_XBOX_Ctrl_IntrOut_Hdr callback is NULL", 0);
    }
}

/* EP Iso In interrupt handler, called by EP interrupt */
void USB_XBOX_Audio_IsoIn_Hdr(void)
{
    if(g_Usb_XBOX_CB.audio_tx_cb != NULL){
        /* Print debug log */
        LOG_MSGID_I(USB_DEBUG, "USB_XBOX_Audio_IsoIn_Hdr callback[%x]", g_Usb_XBOX_CB.audio_tx_cb);
        g_Usb_XBOX_CB.audio_tx_cb();
    } else {
        LOG_MSGID_E(USB_XBOX, "USB_XBOX_Audio_IsoIn_Hdr callback is NULL", 0);
    }
}

/* EP Intr Out reset handler */
void USB_XBOX_Ctrl_IntrOut_Reset(void)
{
}

/* EP Iso Out reset handler */
void USB_XBOX_Audio_IsoOut_Reset(void)
{
}

/* EP Intr Out reset handler */
void USB_XBOX_Ctrl_IntrIn_Reset(void)
{
}

/* EP Iso Out reset handler */
void USB_XBOX_Audio_IsoIn_Reset(void)
{
}

/*******************************************************
*   Send/Receive Control & Audio Data API
*******************************************************/
/* Send control data API by endpoint */
uint32_t USB_XBOX_Control_TX_SendData(uint8_t *data, uint8_t len)
{
    bool chrger_detect = FALSE;
    uint8_t ep_in_id;
    uint32_t i, usb_wait;
    uint32_t ret = len;

    /* Check the charger (Vbus) is ready for USB which means the USB cable is connected or not */
    chrger_detect = pmu_get_chr_detect_value();

    /* Check the charger (Vbus) is ready for USB which means the USB cable is connected or not */
    if(chrger_detect != PMU_OK){
        LOG_MSGID_E(USB_XBOX, "Detect Charger FAIL!", 0);
        return 0;
    }

    /* Get ep_in_id */
    ep_in_id = g_Usb_XBOX_Ctrl.ep_in_info->ep_status.epin_status.byEP;

    LOG_MSGID_I(USB_XBOX, "USB_XBOX_Control_TX_SendData ep_in_id[%x] data[%x] len[%x]", 3, ep_in_id, data, len);

    /* Check the length is valid*/
    if(len == 0) {
        LOG_MSGID_E(USB_XBOX, "Data length is 0", 0);
        return 0;
    }

    if(len > USB_XBOX_CTRL_TX_BUFFER_LEN) {
        LOG_MSGID_E(USB_XBOX, "Data length [%x] is larger than [%x]byte", 2, len, USB_XBOX_CTRL_TX_BUFFER_LEN);
        return 0;
    }

    /* Send data by endpoint*/
    hal_usb_clear_tx_endpoint_fifo(ep_in_id, HAL_USB_EP_TRANSFER_INTR, true);

    usb_wait = 0;
    while (!hal_usb_is_endpoint_tx_empty(ep_in_id)) {
        /* If USB TX can't sent data successfully, it wouldn't block IRQ */
        if (usb_wait > 0x1000) {
            LOG_MSGID_E(USB_XBOX, "Send Data ERROR1!", 0);
            ret = 0;
            break;
        }
        usb_wait++;
    }

    hal_usb_write_endpoint_fifo(ep_in_id, len, data);
    hal_usb_set_endpoint_tx_ready_toggle(ep_in_id);

    usb_wait = 0;
    while (!hal_usb_is_endpoint_tx_empty(ep_in_id)) {
        if (usb_wait > 0x1000) {
            LOG_MSGID_E(USB_XBOX, "Send Data ERROR2!", 0);
            ret = 0;
            break;
        }
        usb_wait++;
    }

    for(i=0; i<len; i+=8){
        LOG_MSGID_I(USB_XBOX, "USB XBOX Ctrl TX [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]",8 ,
                               data[i+0], data[i+1], data[i+2], data[i+3], data[i+4], data[i+5], data[i+6], data[i+7], data[i+8]);
    }

    return ret;
}

/* Send audio data API by endpoint */
uint32_t USB_XBOX_Audio_TX_SendData(uint8_t *data, uint8_t len)
{
    bool chrger_detect = FALSE;
    uint8_t ep_in_id;
    uint32_t usb_wait;
    uint32_t ret = len;

    /* Check the charger (Vbus) is ready for USB which means the USB cable is connected or not */
    chrger_detect = pmu_get_chr_detect_value();
    if(chrger_detect != PMU_OK){
        LOG_MSGID_E(USB_XBOX, "Detect Charger FAIL!", 0);
        return 0;
    }

    /* Get ep_in_id */
    ep_in_id = g_Usb_XBOX_Audio.ep_in_info->ep_status.epin_status.byEP;

    /* Print debug log */
    LOG_MSGID_I(USB_DEBUG, "USB_XBOX_Audio_TX_SendData ep_in_id[%x] data[%x] len[%x]", 3, ep_in_id, data, len);

    /* Check the length is valid */
    if(len == 0) {
        LOG_MSGID_E(USB_XBOX, "Data length is 0", 0);
        return 0;
    }

    if(len > USB_XBOX_AUDIO_TX_BUFFER_LEN) {
        LOG_MSGID_E(USB_XBOX, "Data length [%x] is larger than [%x]byte", 2, len, USB_XBOX_AUDIO_TX_BUFFER_LEN);
        return 0;
    }

    /* Set data from share buffer into USB FIFO */
    hal_usb_write_endpoint_fifo(ep_in_id, len, data);

    /* Send usb data */
    hal_usb_set_endpoint_tx_ready(ep_in_id);

    /* Print debug log */
    LOG_MSGID_I(USB_DEBUG, "USB XBOX Audio TX [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", 8,
                           data[0] ,data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

    return ret;
}

/*******************************************************
*   Control Interface Function
*******************************************************/
void USB_XBOX_ControlIf_Create(void *if_name)
{
    uint8_t if_id, ep_in_id, ep_out_id;

    /* Get resource number and register to gUsbDevice */
    g_Usb_XBOX_Ctrl.if_info = USB_Get_Interface_Number(&if_id);
    g_Usb_XBOX_Ctrl.if_id = if_id;

    /* Get ep out id */
    g_Usb_XBOX_Ctrl.ep_out_info = USB_Get_Intr_Rx_Ep(&ep_out_id);
    g_Usb_XBOX_Ctrl.ep_out_id  = ep_out_id;

    /* Get ep in id */
    g_Usb_XBOX_Ctrl.ep_in_info = USB_Get_Intr_Tx_Ep(&ep_in_id);
    g_Usb_XBOX_Ctrl.ep_in_id = ep_in_id;

    LOG_MSGID_I(USB_XBOX, "USB_XBOX_ControlIf_Create if_id[%x] ep_out_id[%x] ep_in_id[%x]", 3, if_id, ep_out_id, ep_in_id);

    /* Record interface name and interface descriptor length */
    g_Usb_XBOX_Ctrl.if_info->interface_name_ptr = (char *) if_name;
    g_Usb_XBOX_Ctrl.if_info->ifdscr_size = sizeof(xbox_control_if_dscr);
    g_Usb_XBOX_Ctrl.ep_out_info->epdscr_size = sizeof(xbox_control_ep_out_dscr);
    g_Usb_XBOX_Ctrl.ep_in_info->epdscr_size = sizeof(xbox_control_ep_in_dscr);

    /* Related endpoint info and class specific command handler */
    g_Usb_XBOX_Ctrl.if_info->ep_info[0] = g_Usb_XBOX_Ctrl.ep_out_info;
    g_Usb_XBOX_Ctrl.if_info->ep_info[1] = g_Usb_XBOX_Ctrl.ep_in_info;

    /* Descriptor */
    memcpy((uint32_t *) &(g_Usb_XBOX_Ctrl.if_info->ifdscr.stdif), xbox_control_if_dscr, sizeof(xbox_control_if_dscr));
    memcpy((uint32_t *) &(g_Usb_XBOX_Ctrl.ep_out_info->epdesc.stdep), xbox_control_ep_out_dscr, sizeof(xbox_control_ep_out_dscr));
    memcpy((uint32_t *) &(g_Usb_XBOX_Ctrl.ep_in_info->epdesc.stdep), xbox_control_ep_in_dscr, sizeof(xbox_control_ep_in_dscr));

    /* Standard interface descriptor */
    g_Usb_XBOX_Ctrl.if_info->ifdscr.stdif.bInterfaceNumber = g_Usb_XBOX_Ctrl.if_id;

    /* Reset handler */
    g_Usb_XBOX_Ctrl.ep_out_info->ep_reset = USB_XBOX_Ctrl_IntrOut_Reset;
    g_Usb_XBOX_Ctrl.ep_in_info->ep_reset = USB_XBOX_Ctrl_IntrIn_Reset;

    /* Endpoint handler */
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_RX, ep_out_id, USB_XBOX_Ctrl_IntrOut_Hdr);
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_in_id, USB_XBOX_Ctrl_IntrIn_Hdr);

    /* Endpoint descriptor */
    g_Usb_XBOX_Ctrl.ep_out_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_OUT | ep_out_id); /* Out Pipe */
    g_Usb_XBOX_Ctrl.ep_out_info->ep_status.epout_status.byEP = ep_out_id;

    g_Usb_XBOX_Ctrl.ep_in_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_IN | ep_in_id);    /* In Pipe */
    g_Usb_XBOX_Ctrl.ep_in_info->ep_status.epin_status.byEP = ep_in_id;
}

void USB_XBOX_ControlIf_Reset(void)
{
}

void USB_XBOX_ControlIf_Enable(void)
{
    LOG_MSGID_I(USB_XBOX, "USB_XBOX_ControlIf_Enable", 0);

    /*Non-DMA*/
    hal_usb_enable_tx_endpoint(g_Usb_XBOX_Ctrl.ep_in_id, HAL_USB_EP_TRANSFER_INTR, HAL_USB_EP_USE_NO_DMA, false);
    hal_usb_enable_rx_endpoint(g_Usb_XBOX_Ctrl.ep_out_id, HAL_USB_EP_TRANSFER_INTR, HAL_USB_EP_USE_NO_DMA, false);
}

void USB_XBOX_ControlIf_Speed_Reset(bool b_other_speed)
{
    LOG_MSGID_I(USB_XBOX, "USB_XBOX_ControlIf_Speed_Reset", 0);

    /* One 64-byte packet capable interrupt IN/OUT endpoint */
    uint32_t max_size = USB_XBOX_CTRL_RX_BUFFER_LEN;

    g_Usb_XBOX_Ctrl.ep_out_info->epdesc.stdep.wMaxPacketSize[0] = max_size & 0xff;
    g_Usb_XBOX_Ctrl.ep_out_info->epdesc.stdep.wMaxPacketSize[1] = (max_size >> 8) & 0xff;

    g_Usb_XBOX_Ctrl.ep_in_info->epdesc.stdep.wMaxPacketSize[0] = max_size & 0xff;
    g_Usb_XBOX_Ctrl.ep_in_info->epdesc.stdep.wMaxPacketSize[1] = (max_size >> 8) & 0xff;

    /* Polling interval in milliseconds: must be >= 4 */
    g_Usb_XBOX_Ctrl.ep_out_info->epdesc.stdep.bInterval = 4;
    g_Usb_XBOX_Ctrl.ep_in_info->epdesc.stdep.bInterval  = 4;
}

/*******************************************************
*   Audio Interface Function
*******************************************************/
void USB_XBOX_AudioIf_Create(void *if_name)
{
    uint8_t if_id, ep_in_id, ep_out_id;

    /* Get resource number and register to gUsbDevice */
    g_Usb_XBOX_Audio.if_info = USB_Get_Interface_Number(&if_id);
    g_Usb_XBOX_Audio.if_id = if_id;

    /* Get ep out id */
    g_Usb_XBOX_Audio.ep_out_info = USB_Get_Iso_Rx_Ep(&ep_out_id);
    g_Usb_XBOX_Audio.ep_out_id  = ep_out_id;
    g_Usb_XBOX_Audio.if_info ->alternate_if_info[0].ep_info[0] = g_Usb_XBOX_Audio.ep_out_info;

    /* Get ep in id */
    g_Usb_XBOX_Audio.ep_in_info = USB_Get_Iso_Tx_Ep(&ep_in_id);
    g_Usb_XBOX_Audio.ep_in_id = ep_in_id;
    g_Usb_XBOX_Audio.if_info->alternate_if_info[0].ep_info[1] = g_Usb_XBOX_Audio.ep_in_info;

    LOG_MSGID_I(USB_XBOX, "USB_XBOX_AudioIf_Create if_id[%x] ep_out_id[%x] ep_in_id[%x]", 3, if_id, ep_out_id, ep_in_id);

    /* Record interface name and interface descriptor length */
    g_Usb_XBOX_Audio.if_info->interface_name_ptr = (char *) if_name;
    g_Usb_XBOX_Audio.if_info->ifdscr_size = sizeof(xbox_audio_if_dscr);
    g_Usb_XBOX_Audio.if_info->alternate_if_info[0].ifdscr_size = sizeof(xbox_audio_if_alt_dscr);
    g_Usb_XBOX_Audio.if_info->alternate_if_info[0].ep_info[0]->epdscr_size = sizeof(xbox_audio_ep_out_dscr);
    g_Usb_XBOX_Audio.if_info->alternate_if_info[0].ep_info[1]->epdscr_size = sizeof(xbox_audio_ep_in_dscr);

    /* Related endpoint info and class specific command handler*/
    g_Usb_XBOX_Audio.if_info->alternate_if_info[0].ep_info[0] = g_Usb_XBOX_Audio.ep_out_info;
    g_Usb_XBOX_Audio.if_info->alternate_if_info[0].ep_info[1] = g_Usb_XBOX_Audio.ep_in_info;

    /* Descriptor */
    memcpy((uint32_t *) &(g_Usb_XBOX_Audio.if_info->ifdscr.stdif), xbox_audio_if_dscr, sizeof(xbox_audio_if_dscr));
    memcpy((uint32_t *) &(g_Usb_XBOX_Audio.if_info->alternate_if_info[0].ifdscr.stdif), xbox_audio_if_alt_dscr, sizeof(xbox_audio_if_alt_dscr));
    
    memcpy((uint32_t *) &(g_Usb_XBOX_Audio.ep_out_info->epdesc.stdep), xbox_audio_ep_out_dscr, sizeof(xbox_audio_ep_out_dscr));
    memcpy((uint32_t *) &(g_Usb_XBOX_Audio.ep_in_info->epdesc.stdep), xbox_audio_ep_in_dscr, sizeof(xbox_audio_ep_in_dscr));

    /* Standard interface descriptor */
    g_Usb_XBOX_Audio.if_info->ifdscr.stdif.bInterfaceNumber = g_Usb_XBOX_Audio.if_id;
    g_Usb_XBOX_Audio.if_info->alternate_if_info[0].ifdscr.stdif.bInterfaceNumber = g_Usb_XBOX_Audio.if_id;

    /* Endpoint handler */
    g_Usb_XBOX_Audio.ep_out_info->ep_reset = USB_XBOX_Audio_IsoOut_Reset;
    g_Usb_XBOX_Audio.ep_in_info->ep_reset = USB_XBOX_Audio_IsoIn_Reset;

    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_RX, ep_out_id, USB_XBOX_Audio_IsoOut_Hdr);
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_in_id, USB_XBOX_Audio_IsoIn_Hdr);

    /* Endpoint descriptor */
    g_Usb_XBOX_Audio.ep_out_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_OUT | ep_out_id); /* Out Pipe */
    g_Usb_XBOX_Audio.ep_out_info->ep_status.epout_status.byEP = ep_out_id;

    g_Usb_XBOX_Audio.ep_in_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_IN | ep_in_id);    /* In Pipe */
    g_Usb_XBOX_Audio.ep_in_info->ep_status.epin_status.byEP = ep_in_id;
}

void USB_XBOX_AudioIf_Reset(void)
{
}

void USB_XBOX_AudioIf_Enable(void)
{
    LOG_MSGID_I(USB_XBOX, "USB_XBOX_AudioIf_Enable", 0);

    /*Non-DMA*/
    hal_usb_enable_tx_endpoint(g_Usb_XBOX_Audio.ep_in_id, HAL_USB_EP_TRANSFER_ISO, HAL_USB_EP_USE_NO_DMA, false);
    hal_usb_enable_rx_endpoint(g_Usb_XBOX_Audio.ep_out_id, HAL_USB_EP_TRANSFER_ISO, HAL_USB_EP_USE_NO_DMA, false);
}

void USB_XBOX_AudioIf_Speed_Reset(bool b_other_speed)
{
    uint32_t max_size;

    LOG_MSGID_I(USB_XBOX, "USB_XBOX_AudioIf_Speed_Reset", 0);

    /* One 228-byte packet capable isochronous OUT endpoint */
    max_size = USB_XBOX_AUDIO_RX_BUFFER_LEN;
    g_Usb_XBOX_Audio.ep_out_info->epdesc.stdep.wMaxPacketSize[0] =  max_size & 0xff;
    g_Usb_XBOX_Audio.ep_out_info->epdesc.stdep.wMaxPacketSize[1] = (max_size >> 8) & 0xff;

    /* One 64-byte packet capable isochronous IN endpoint  */
    max_size = USB_XBOX_AUDIO_TX_BUFFER_LEN;
    g_Usb_XBOX_Audio.ep_in_info->epdesc.stdep.wMaxPacketSize[0]  =  max_size & 0xff;
    g_Usb_XBOX_Audio.ep_in_info->epdesc.stdep.wMaxPacketSize[1]  = (max_size >> 8) & 0xff;

    /* Polling interval / rate is up to 1 ms / 250 Hz */
    g_Usb_XBOX_Audio.ep_out_info->epdesc.stdep.bInterval = 1;
    g_Usb_XBOX_Audio.ep_in_info->epdesc.stdep.bInterval  = 1;
}

#endif /* MTK_USB_XBOX_ENABLE */
#endif /* MTK_USB_DEMO_ENABLED */

