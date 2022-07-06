/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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

#ifndef USBHID_DRV_H
#define USBHID_DRV_H

#include "usb.h"
#include "usb_comm.h"
#include "usb_custom.h"
#include "bmd.h"

#ifdef MTK_PORT_SERVICE_ENABLE
#include "serial_port.h"
#endif

/***********************************************
    hid class specific command definition
************************************************/

#define USB_HID_DEVICE_CODE              0x00
#define USB_HID_SUBCLASS_CODE            0x00
#define USB_HID_PROTOCOL_CODE            0x00

#define USB_HID_INTERFACE_CLASS_CODE     0x03
#define USB_HID_INTERFACE_SUBCLASS_CODE  0x01
#define USB_HID_INTERFACE_PROTOCOL_CODE  0x02 //0x01  // 1 -- Keyboard  2 -- mouse


#define USB_HID_IF_LENGTH                 9   /* standard class */

/* Class-Specfic Codes ,EP0*/
#define USB_HID_SEND_ENCAPSULATED_COMMAND    0x00
#define USB_HID_GET_ENCAPSULATED_RESPONSE    0x01
#define USB_HID_SET_COMM_FEATURE             0x02
#define USB_HID_GET_COMM_FEATURE             0x03
#define USB_HID_CLEAR_COMM_FEATURE           0x04
#define USB_HID_SET_LINE_CODING              0x20
#define USB_HID_GET_LINE_CODING              0x21
#define USB_HID_SET_CONTROL_LINE_STATE       0x22
#define USB_HID_SEND_BREAK                   0x23


/*Notifications, interrupt pipe*/
#define USB_HID_NOTIF_NETWORK_CONNECTION    0x00
#define USB_HID_NOTIF_RESPONSE_AVAILABLE    0x01
#define USB_HID_NOTIF_SERIAL_STATE          0x20


/***********************************************
    Implement definition
************************************************/

#define USB_HID_IF_NUMBER             1

#define USB_HID_EP_NUMBER             1  /*interrupt pipe*/


typedef struct {
    uint8_t    bLength;
    uint8_t    bDescriptorType;
    uint8_t    bInterfaceNumber;
    uint8_t    bAlternateSetting;
    uint8_t    bNumEndpoints;
    uint8_t    bInterfaceClass;
    uint8_t    bInterfaceSubClass;
    uint8_t    bInterfaceProtocol;
    uint8_t    iInterface;

    uint8_t    HFbFunctionLength;        /*Header Functional Descriptor*/
    uint8_t    HFbDescriptorType;
    uint8_t    HFbDescriptorSubtype;
    uint16_t   bcdCDC;

    uint8_t    HIDFbFunctionLength;      /*Abstract Control Management Functional Descriptor*/
    uint8_t    HIDFbDescriptorType;
    uint8_t    HIDFbDescriptorSubtype;
    uint8_t    HIDFbmCapabilities;

    uint8_t    UFbFunctionLength;        /*Union Functional Descriptor*/
    uint8_t    UFbDescriptorType;
    uint8_t    UFbDescriptorSubtype;
    uint8_t    bMasterInterface;
    uint8_t    bSlaveInterface0;

    uint8_t    CMFbFunctionLength;        /*Call Management Descriptor*/
    uint8_t    CMFbDescriptorType;
    uint8_t    CMFbDescriptorSubtype;
    uint8_t    CMFbmCapabilities;
    uint8_t    bDataInterface;
} Usb_Hid_If_Dscr;

/* HID interrupt packet structure */
typedef struct {
    uint8_t     bmRequestType;
    uint8_t     bRequest;
    uint16_t    wValue;
    uint16_t    wIndex;
    uint16_t    wLength;
    uint16_t    Data;
} UsbHid_Intr_Pkt;

typedef enum {
    HID_INTR_IDLE = 0,
    HID_INTR_SEND_ONE_PKT,
    HID_INTR_SEND_LAST_PKT
} USB_HID_INTR_PIPE_STATE;


/* Current HID driver owner application */
typedef enum {
    USB_HID_OWNER_UART = 0,
    USB_HID_OWNER_FT
} USB_HID_OWNER_TYPE;


typedef void (*usb_hid_rx_ptr)(void);

/* HID device structure */
typedef struct {
    uint8_t        data_interface_id;
    bool           send_Txilm;
    bool           send_Rxilm;
    bool           send_UARTilm;
    bool           config_send_Txilm;
    bool           ring_buffer_timer_counting;       /* flag to check ring buffer timer running or not */
    bool           threshold_enable;                 /* enable ring buffer threshold and time out mechanism */
    bool           setup_dma;                        /* a flag to record DMA is triggered or not */
    bool           is_intr_pending_pkt;              /* interrupt pipe has pending packet */
    bool           dma_txcb_just_done;               /*DMA TX Callback Function just done. It's clear by putbyte funtion*/
    bool           break_detect;
    bool           put_start;
    uint16_t       dmaf_setmember;                   /* Who set dma_false after putbyte and it does not finsih*/
    uint16_t       line_state;
    UsbHid_Intr_Pkt        intr_pkt;
    uint32_t       handle;                /*GPT handle*/
    uint32_t       dma_handle;            /*for DMA timeout handle*/
    uint32_t       ring_buffer_handle;    /* ring buffer's GPT handle */
    uint32_t       break_number;

    Usb_EpBIn_Status     *txpipe;        /* bulk in EP, Data interface */
    Usb_EpBOut_Status    *rxpipe;        /* bulk out EP, Data interface */
    Usb_EpBIn_Status     *intrpipe;      /* interrupt EP, Communication Interface */

    USB_HID_INTR_PIPE_STATE    intr_state;                /* interrupt pipe state machine */
    Usb_Ep_Info                *comm_ep_intr_info;
    Usb_Ep_Info                *data_ep_in_info;
    Usb_Ep_Info                *data_ep_out_info;
    Usb_Interface_Info         *comm_if_info;
    Usb_Interface_Info         *data_if_info;

    Usb_IAD_Dscr               *iad_desc;

#ifdef  __USB_HIGH_SPEED_COM_PORT_ENABLE__
    uint32_t       data_addr;
    uint32_t       rx_buf_data_len[USB_HID_RX_BUFFER_NUM];
    uint16_t       tx_count;          /* To store the TX count, debug only*/
    bool           b_data_left;       // left data due to 512N
    bool           b_is_high_speed_enable;
    bool           b_rc_index_move;
    uint16_t       b_rc_get_count;
    uint8_t        rx_buf_rc_index;  /* read clear pointer */
    uint8_t        rx_buf_r_index;   /* read pointer */
    uint8_t        rx_buf_w_index;   /* write pointer */
    uint8_t        rx_buf_count;     /* the buffer count with data */
#ifdef DRV_USB_DMA_QUEUE_SUPPORT
    bool        ppp_txdmaq_first;
    uint32_t    ppp_rdmaq_idx;
    uint32_t    ppp_tdmaq_idx;
#endif

//#ifdef __USB_TETHERING__
    uint8_t         *rx_buf_addr[RNDIS_RX_BUFFER_NUM + 1];    //9 : 0 ~ 8
    uint32_t        isd_rx_buf_data_len[RNDIS_RX_BUFFER_NUM + 1];
    uint8_t         *rx_flc_buf_addr[RNDIS_RX_FLC_BUFFER_NUM + 1];     //3 //3:  0~2
    uint32_t        rx_flc_buf_data_len[RNDIS_RX_FLC_BUFFER_NUM + 1];
    uint16_t        tx_flc_count;          /* To store the TX count, debug only*/
    uint8_t         *tx_flc_data_addr;
    bool            rx_full_data;
    bool            halt_flag;
    bool            ctrl_data_done;
    bool            send_ctrl_Txilm;  // Ctrl pipe --> TX message
    bool            send_ctrl_Rxilm;  // ctrl pipe --> RX message

//    bool        dma_rx_running;  //use  FIFO Read first
//    bool        dma_tx_running; //the dame as setup_dma

    uint8_t        rx_full_buff_index;      // receive Data
    uint8_t        rx_empty_buff_index;     // empty buffer
    uint8_t        rx_flc_full_buff_index;  // receive Data
    uint8_t        rx_flc_empty_buff_index; // empty buffer
//    bool         b_flc_rc_index_move;
    uint8_t        rx_flc_buf_rc_index;   /* read clear pointer */
    uint8_t        rx_flc_buf_r_index;    /* read pointer */
    uint8_t        rx_flc_buf_w_index;    /* write pointer: data write (bulk out handler) */
    uint8_t        rx_flc_buf_count;      /* the buffer count with data */
    uint8_t        rx_flc_temp_length;    /* for first time PC send data just before buffer push  (initial message) */
    bool           rx_flc_temp;           /* for first time PC send data just before buffer push (initial message) */
    uint8_t        *rx_flc_temp_buf_addr;
#endif

    bool        b_is_tx_direct_path_enable;
} UsbHid_Struct;

/***********************************************
    function and global variable
************************************************/
//extern UsbUARTStruct USB2UARTPort[MAX_USB_PORT_NUM];
extern UsbHid_Struct g_UsbHID;

extern void USB_Hid_Rx_ClrFifo(uint8_t port);
extern void USB_Hid_CommIf_Create(void *ifname);
extern void USB_Hid_CommIf_Reset(void);
extern void USB_Hid_CommIf_Enable(void);
extern void USB_Hid_CommIf_Speed_Reset(bool b_other_speed);
extern void USB_Hid_DataIf_Reset(uint8_t port);
extern void USB_Hid_DataIf_Enable(uint8_t port);
extern void USB_Hid_DataIf_Speed_Reset(uint8_t port, bool b_other_speed);
extern void USB_Init_Hid_Status(void);
extern void USB_Release_Hid_Status(void);
extern void USB_UnMask_COM_Intr(uint32_t port);

#endif /* USBHID_DRV_H */

