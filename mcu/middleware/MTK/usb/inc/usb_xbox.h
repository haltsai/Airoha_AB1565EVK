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


#ifndef USB_XBOX_H
#define USB_XBOX_H

#include "usb.h"

/* Device Descriptor */
#define USB_XBOX_DEVICE_CODE               0xFF
#define USB_XBOX_SUBCLASS_CODE             0x47
#define USB_XBOX_PROTOCOL_CODE             0xD0

/* Buffer Definition */
#define USB_XBOX_CTRL_RX_BUFFER_LEN          64
#define USB_XBOX_CTRL_TX_BUFFER_LEN          64
#define USB_XBOX_AUDIO_RX_BUFFER_LEN        228
#define USB_XBOX_AUDIO_TX_BUFFER_LEN         64

#define USB_UNUSAGED_IF_ID                 0xFF

/* XBOX Status */
typedef enum {
    USB_XBOX_STATUS_OK              =  0,
    USB_XBOX_CHARGER_DETECT_ERROR   =  1,
    USB_XBOX_CTRL_TX_LEN_IS_ZERO    =  2,
    USB_XBOX_CTRL_TX_LEN_TOO_LARGE  =  3,
    USB_XBOX_AUDIO_TX_LEN_IS_ZERO   =  4,
    USB_XBOX_AUDIO_TX_LEN_TOO_LARGE =  5,
    USB_XBOX_CTRL_SEND_DATA_ERROR1  =  6,
    USB_XBOX_CTRL_SEND_DATA_ERROR2  =  7,
    USB_XBOX_OTHERS_FAIL            =  8,
} USB_XBOX_STATUS_t;

/* Callback Definition*/
typedef void (*XBOX_CTRL_RX_FUNC)(uint8_t *data_buf, uint8_t data_len);
typedef void (*XBOX_CTRL_TX_FUNC)(void);
typedef void (*XBOX_AUDIO_RX_FUNC)(uint8_t *data_buf, uint8_t data_len);
typedef void (*XBOX_AUDIO_TX_FUNC)(void);

/* XBOX Callback Functions */
typedef struct {
    bool                  initialized;
    XBOX_CTRL_RX_FUNC     ctrl_rx_cb;
    XBOX_CTRL_TX_FUNC     ctrl_tx_cb;
    XBOX_AUDIO_RX_FUNC    audio_rx_cb;
    XBOX_AUDIO_TX_FUNC    audio_tx_cb;
} Usb_XBOX_Struct_CB;

/* XBOX Control & Audio Information Structure*/
typedef struct {
    /* ID */
    uint8_t  if_id;
    uint8_t  ep_in_id;
    uint8_t  ep_out_id;
    /* Infomation*/
    Usb_Interface_Info  *if_info;
    Usb_Ep_Info         *ep_in_info;
    Usb_Ep_Info         *ep_out_info;
} Usb_XBOX_Struct_Info;

/* XBOX Gobal Variable */
extern Usb_XBOX_Struct_Info g_Usb_XBOX_Ctrl;
extern Usb_XBOX_Struct_Info g_Usb_XBOX_Audio;
extern Usb_XBOX_Struct_CB g_Usb_XBOX_CB;

extern const uint8_t xbox_dev_descr[];
extern const uint8_t xbox_cfg_descr[];
extern const uint8_t xbox_ext_com_ID_dscr[];
extern const uint8_t xbox_ext_com_dscr[];

/* Initialize and Rlease Function */
void USB_Init_XBOX_Status(void);
void USB_Release_XBOX_Status(void);

/* Register Control & Audio callback */
void USB_XBOX_Control_Register_Rx_Callback(XBOX_CTRL_RX_FUNC rx_cb);
void USB_XBOX_Audio_Register_Rx_Callback(XBOX_AUDIO_RX_FUNC rx_cb);
void USB_XBOX_Audio_Register_Tx_Callback(XBOX_AUDIO_TX_FUNC tx_cb);

/* Send Control & Audio Data */
uint32_t USB_XBOX_Control_TX_SendData(uint8_t *data_buf, uint8_t data_len);
uint32_t USB_XBOX_Audio_TX_SendData(uint8_t *data_buf, uint8_t data_len);

/* Control Interface Function */
void USB_XBOX_ControlIf_Create(void *if_name);
void USB_XBOX_ControlIf_Enable(void);
void USB_XBOX_ControlIf_Reset(void);
void USB_XBOX_ControlIf_Speed_Reset(bool b_other_speed);

/* Audio Interface function */
void USB_XBOX_AudioIf_Create(void *if_name);
void USB_XBOX_AudioIf_Enable(void);
void USB_XBOX_AudioIf_Reset(void);
void USB_XBOX_AudioIf_Speed_Reset(bool b_other_speed);

#endif // USB_XBOX_H
