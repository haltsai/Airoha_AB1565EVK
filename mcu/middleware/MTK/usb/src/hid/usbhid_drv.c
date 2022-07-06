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

#ifdef MTK_USB_DEMO_ENABLED

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "memory_attribute.h"
#include "hal_usb.h"
#include "usb_comm.h"
#include "usb.h"
#include "usb_resource.h"
#include "usbhid_drv.h"
#include "usbhid_adap.h"
#include "syslog.h"

#ifdef MTK_PORT_SERVICE_ENABLE
#include "serial_port.h"
#endif


/* Global variables */

//UsbUARTStruct USB2UARTPort[MAX_USB_PORT_NUM];
UsbHid_Struct g_UsbHID;

#if 0
#ifdef MTK_PORT_SERVICE_ENABLE
uint8_t USB_PORT[SERIAL_PORT_DEV_MAX];
#else
uint8_t USB_PORT[2];
#endif
#endif

/* for DMA tx buffer and tx_isr buffer to differentiate */
//static uint32_t USB2Uart_WriteLength[MAX_USB_PORT_NUM];

/* Static functions */
static uint8_t USB_Hid_Get_DataIf_Num(void);
void USB_Hid_CommIf_Create(void *ifname);
void USB_Hid_CommIf_Reset(void);
void USB_Hid_CommIf_Enable(void);
void USB_Hid_CommIf_Speed_Reset(bool b_other_speed);
void USB_Hid_DataIf_Reset(uint8_t port);
void USB_Hid_DataIf_Enable(uint8_t port);
void USB_Hid_DataIf_Speed_Reset(uint8_t port, bool b_other_speed);
void USB_Hid_Break_Timeout(uint8_t port, void *parameter);
void USB_Hid_Ep0_SetLineCoding(uint8_t port, void *data);
void USB_Hid_Ep0_Command(uint8_t port, Usb_Ep0_Status *pep0state, Usb_Command *pcmd);
void USB_Hid_BulkOut_Hdr(uint8_t port);
void USB_Hid_BulkIn_Reset(uint8_t port);
void USB_Hid_BulkOut_Reset(uint8_t port);
void USB_Hid_IntrIn_Hdlr(uint8_t port);
void USB_Hid_IntrIn_Reset(uint8_t port);

static void USB_Hid1_Ep0_Command(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    USB_Hid_Ep0_Command(0, pep0state, pcmd);
}



static void USB_Hid1_IntrIn_Hdlr(void)
{
    USB_Hid_IntrIn_Hdlr(0);
}

static void USB_Hid1_IntrIn_Reset(void)
{
    USB_Hid_IntrIn_Reset(0);
}

static const uint8_t hid_if_dscr[] = {
    USB_IFDSC_LENGTH,    //bLength;
    USB_INTERFACE,    //bDescriptorType;
    0x01,    //bInterfaceNumber;
    0x00,    //bAlternateSetting;
    USB_HID_EP_NUMBER,    //bNumEndpoints;
    USB_HID_INTERFACE_CLASS_CODE,    //bInterfaceClass;
    USB_HID_INTERFACE_SUBCLASS_CODE,    //bInterfaceSubClass;
    USB_HID_INTERFACE_PROTOCOL_CODE,    //bInterfaceProtocol;
    0x00,    //iInterface;
};

const uint8_t hid_dscr[] = {
    USB_IFDSC_LENGTH,    //bLength;
    0x21,    //bDescriptorType;
    0x10,    //bcdHID;
    0x01,   //bcdHID;
    0x00,    //bContryCode;
    0x01,    //bNumDescriptors;
    0x22,   //bDescriptorType;
    0x34,    //wItemLength;
    0x00    //wItemLength;
};

const uint8_t hid_ep_dscr[] = {
    USB_EPDSC_LENGTH,    //bLength;
    USB_ENDPOINT,    //bDescriptorType;
    0x00,    //bEndpointAddress;   use endpoint_4, USB_Hid_CommIf_Create set this parameter
    USB_EP_INTR,    //bmAttributes;
    0x40,    //wMaxPacketSize[2];
    0x00,    //wMaxPacketSize[2];
    0x00    //bInterval; USB_Hid_CommIf_Create set this parameter
};

const uint8_t hid_report_dscr[] = {
    0x05, 0x01,     /* Usage Page(Generic Desktop Controls) */
    0x09, 0x02,     /* Usage(Mouse) */
    0xA1, 0x01,     /* Collection(Application) */
    0x09, 0x01,         /* Usage(Pointer) */
    0xA1, 0x00,         /* Collection(Physical) */
    0x05, 0x09,             /* Usage Page(Button) */
    0x19, 0x01,             /* Usage Minimum(0x1) */
    0x29, 0x03,             /* Usage Maximum(0x3) */
    0x15, 0x00,             /* Logical Minimum(0x0) */
    0x25, 0x01,             /* Logical Maximum(0x1) */
    0x75, 0x01,             /* Report Size(0x1) */
    0x95, 0x03,             /* Report Count(0x3) */
    0x81, 0x02,             /* Input(3 button bit) */
    0x75, 0x05,             /* Report Size(0x5) */
    0x95, 0x01,             /* Report Count(0x1) */
    0x81, 0x01,             /* Input(5 bit padding) */
    0x05, 0x01,             /* Usage Page(Generic Desktop Controls) */
    0x09, 0x30,             /* Usage(X) */
    0x09, 0x31,             /* Usage(Y) */
    0x09, 0x38,             /* Usage(Wheel) */
    0x15, 0x81,             /* Logical Minimum(0x81)(-127) */
    0x25, 0x7F,             /* Logical Maximum(0x7F)(127) */
    0x75, 0x08,             /* Report Size(0x8) */
    0x95, 0x03,             /* Report Count(0x3) */
    0x81, 0x06,             /* Input(1 byte wheel) */
    0xC0,               /* End Collection */
    0xC0,           /* End Collection */
};


/************************************************************
    Interface initialization functions
*************************************************************/
uint8_t USB_Hid_Get_DataIf_Num(void)
{
    uint8_t if_id;


    if (g_UsbHID.data_interface_id == 0xFF) {
        /* Get resource number and register to gUsbDevice */
        g_UsbHID.data_if_info = USB_Get_Interface_Number(&if_id);
        g_UsbHID.data_interface_id = if_id;
    }

    return g_UsbHID.data_interface_id;
}


/* Communication interface create function, prepare descriptor */
void USB_Hid_CommIf_Create(void *ifname)
{
    uint8_t ep_id;
    uint8_t if_id;

    /* Get resource number and register to gUsbDevice */
    g_UsbHID.comm_if_info = USB_Get_Interface_Number(&if_id);
    g_UsbHID.comm_ep_intr_info = USB_Get_Intr_Tx_Ep(&ep_id);

    /* Record interface name and interface descriptor length */
    g_UsbHID.comm_if_info->interface_name_ptr = (char *)ifname;
    g_UsbHID.comm_if_info->ifdscr_size = USB_HID_IF_LENGTH;
    g_UsbHID.comm_ep_intr_info->epdscr_size = USB_EPDSC_LENGTH;

    /* Related endpoint info and class specific command handler */
    g_UsbHID.comm_if_info->ep_info[0] = g_UsbHID.comm_ep_intr_info;
    g_UsbHID.comm_if_info->if_class_specific_hdlr = USB_Hid1_Ep0_Command;

    memcpy(&(g_UsbHID.comm_if_info->ifdscr.stdif), hid_if_dscr, USB_HID_IF_LENGTH);
    memcpy(&(g_UsbHID.comm_ep_intr_info->epdesc.stdep), hid_ep_dscr, USB_EPDSC_LENGTH);



    /* Standard interface descriptor */
    ((Usb_Hid_If_Dscr *)g_UsbHID.comm_if_info->ifdscr.classif)->bInterfaceNumber = if_id;
    ((Usb_Hid_If_Dscr *)g_UsbHID.comm_if_info->ifdscr.classif)->iInterface = 0;

    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_id, USB_Hid1_IntrIn_Hdlr);
    g_UsbHID.comm_ep_intr_info->ep_reset = USB_Hid1_IntrIn_Reset;

    /* Union Functional Descriptor */
    ((Usb_Hid_If_Dscr *)g_UsbHID.comm_if_info->ifdscr.classif)->bMasterInterface = if_id;
    ((Usb_Hid_If_Dscr *)g_UsbHID.comm_if_info->ifdscr.classif)->bSlaveInterface0 = USB_Hid_Get_DataIf_Num(); //g_UsbHID[port].data_interface_id;
    /* Call Management Descriptor */
    ((Usb_Hid_If_Dscr *)g_UsbHID.comm_if_info->ifdscr.classif)->bDataInterface = USB_Hid_Get_DataIf_Num(); //g_UsbHID[port].data_interface_id;

    /* Endpoint descriptor */
    g_UsbHID.comm_ep_intr_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_IN | ep_id);    /*InPipe*/
    g_UsbHID.comm_ep_intr_info->epdesc.stdep.bInterval = 3;
    g_UsbHID.comm_ep_intr_info->ep_status.epin_status.byEP = ep_id;
    g_UsbHID.comm_ep_intr_info->ep_status.epin_status.nBytesLeft = USB_EP_NODATA;

    g_UsbHID.intr_pkt.bmRequestType = USB_CMD_CLASSIFIN;
    g_UsbHID.intr_pkt.bRequest = USB_HID_NOTIF_SERIAL_STATE;
    g_UsbHID.intr_pkt.wValue = 0;
    g_UsbHID.intr_pkt.wIndex = if_id;
    g_UsbHID.intr_pkt.wLength = 2;
}


/* Communication interface reset function, should enable EP, but we do not use this interrupt EP so not enable it */
void USB_Hid_CommIf_Reset(void)
{
//    g_UsbHID[port].intrpipe = &g_UsbHID[port].comm_ep_intr_info->ep_status.epin_status;
}


void USB_Hid_CommIf_Enable(void)
{
    /* Non-DMA */
//    hal_usb_enable_tx_endpoint(g_UsbHID[port].intrpipe->byEP, HAL_USB_EP_TRANSFER_INTR, HAL_USB_EP_USE_NO_DMA, true);
}

/* Communication interface speed reset function, enable EP's speed-specific descriptor */
void USB_Hid_CommIf_Speed_Reset(bool b_other_speed)
{
#if 0
    g_UsbHID[port].comm_ep_intr_info->epdesc.stdep.wMaxPacketSize[0] = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_FULL_SPEED & 0xff;
    g_UsbHID[port].comm_ep_intr_info->epdesc.stdep.wMaxPacketSize[1] = 0x00;
#endif
}

/* Data interface reset function, enable EP */
void USB_Hid_DataIf_Reset(uint8_t port)
{
//    g_UsbHID[port].txpipe = &g_UsbHID[port].data_ep_in_info->ep_status.epin_status;
//    g_UsbHID[port].rxpipe = &g_UsbHID[port].data_ep_out_info->ep_status.epout_status;
}


void USB_Hid_DataIf_Enable(uint8_t port)
{
    /* DMA */
//    hal_usb_enable_tx_endpoint(g_UsbHID[port].txpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_ONLY_DMA, true);

    /*Non-DMA*/
//    hal_usb_enable_rx_endpoint(g_UsbHID[port].rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_NO_DMA, true);
}




/* data interface speed reset function, enable EP's speed-specific descriptor */
void USB_Hid_DataIf_Speed_Reset(uint8_t port, bool b_other_speed)
{
#if 0
    uint32_t    temp_max_size;
    temp_max_size = USB_Speed_Reset_Packet_Size((bool)b_other_speed);

    g_UsbHID[port].data_ep_in_info->epdesc.stdep.wMaxPacketSize[0] = temp_max_size & 0xff;
    g_UsbHID[port].data_ep_in_info->epdesc.stdep.wMaxPacketSize[1] = (temp_max_size >> 8) & 0xff;
    g_UsbHID[port].data_ep_out_info->epdesc.stdep.wMaxPacketSize[0] = temp_max_size & 0xff;
    g_UsbHID[port].data_ep_out_info->epdesc.stdep.wMaxPacketSize[1] = (temp_max_size >> 8) & 0xff;
#endif
}

/************************************************************
    global variable g_UsbHID initialize and release functions
*************************************************************/
void USB_Hid_Default_Setting(uint8_t port)
{
#if 0
    g_UsbHID[port].txpipe = NULL;
    g_UsbHID[port].rxpipe = NULL;
    g_UsbHID[port].intrpipe = NULL;
    g_UsbHID[port].data_interface_id = 0xFF;
    g_UsbHID[port].break_detect = false;
    g_UsbHID[port].break_number = 0;
    g_UsbHID[port].send_Txilm = false;
    g_UsbHID[port].send_Rxilm = true;
    g_UsbHID[port].send_UARTilm = false;
    g_UsbHID[port].intr_state = HID_INTR_IDLE;
    g_UsbHID[port].setup_dma = false;
    g_UsbHID[port].line_state = 0;
    if (g_UsbHID[port].put_start == true) {
        g_UsbHID[port].dmaf_setmember |= 0x04;
    }
    g_UsbHID[port].dma_txcb_just_done = false;
#endif
}

/* Initialize global variable g_UsbHID */
void USB_Init_Hid_Status(void)
{
    USB_Hid_Default_Setting(0);
}


/* Release global variable g_UsbHID */
void USB_Release_Hid_Status(void)
{
#if 0
    if (g_UsbHID[0].txpipe != NULL) {
        hal_usb_release_dma_channel(g_UsbHID[0].txpipe->byEP, 0, HAL_USB_EP_DIRECTION_TX, false);
    }
    if (g_UsbHID[1].txpipe != NULL) {
        hal_usb_release_dma_channel(g_UsbHID[1].txpipe->byEP, 0, HAL_USB_EP_DIRECTION_TX, false);
    }
#endif
    USB_Hid_Default_Setting(0);
//    USB_Hid_Default_Setting(1);

    if (gUsbDevice.conf != NULL) {
        USB_Free_Memory((void *)gUsbDevice.conf);
        gUsbDevice.conf = NULL;
    }

//    g_UsbHID[0].config_send_Txilm = false;
//    g_UsbHID[1].config_send_Txilm = false;
}

/************************************************************
    EP0 handle functions
************************************************************/
extern uint32_t hal_usb_ep0_pkt_len(void);

/* Set USB request line coding to UART DCB structure */
void USB_Hid_Ep0_SetLineCoding(uint8_t port, void *data)
{
    uint32_t nCount;
    bool stall = false;

    /* Read setting and set to UART structure */
    nCount = hal_usb_ep0_pkt_len();
    if (nCount == 7) {
//      hal_usb_read_endpoint_fifo(0, nCount, &g_UsbHID[port].line_coding);
    } else {
        stall = true;
    }

    //USB_Set_Endpoint0_State(USB_EP0_RX_STATUS);
    gUsbDevice.ep0_state = USB_EP0_RX_STATUS;
    hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, stall, true);
}


/* Parse class specific request */
void USB_Hid_Ep0_Command(uint8_t port, Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
#if 0
    bool  bError = false;

    switch (pcmd->bRequest) {
        case USB_HID_SET_LINE_CODING:
            /* register handler to handle the get data*/
            //USB_Register_EP0_RxHdlr(USB_Hid_Ep0_SetLineCoding);
            gUsbDevice.ep0_rx_handler = (usb_ep0_rx_ptr)USB_Hid_Ep0_SetLineCoding;
            //USB_Set_Endpoint0_State(USB_EP0_RX);
            gUsbDevice.ep0_state = USB_EP0_RX;
            break;
        case USB_HID_GET_LINE_CODING:
            /* tell host the current setting */
            USB_Generate_EP0Data(pep0state, pcmd, &g_UsbHID[port].line_coding, 7);
            break;
        case USB_HID_SET_CONTROL_LINE_STATE:
            g_UsbHID[port].line_state = pcmd->wValue;
            /* do not need to do anything */
            break;
        case USB_HID_SEND_BREAK:
            /* Break behavior */
            switch (pcmd->wValue) {
                case 0xffff:
                    g_UsbHID[port].break_detect = true;
                    break;
                case 0x0:
                    g_UsbHID[port].break_detect = false;
                    g_UsbHID[port].break_number++;
                    break;
                default:
                    g_UsbHID[port].break_detect = true;
                    break;
            }
            break;

        case USB_HID_SEND_ENCAPSULATED_COMMAND:
        case USB_HID_GET_ENCAPSULATED_RESPONSE:
        case USB_HID_SET_COMM_FEATURE:
        case USB_HID_GET_COMM_FEATURE:
        case USB_HID_CLEAR_COMM_FEATURE:
        default:
            bError = true;
            break;
    }
    /* Stall command if an error occured */
    USB_EP0_Command_Hdlr(bError);
#endif
}


#if 0
/*
    Used to retreive exception log, all interrupts are disabled
    Note that this is special for sending exception log since interrupt is disabled when exception occurs
    It must not be used in normal time
*/
void USB2UART_Polling_Recv_Data(uint32_t port)
{
    uint32_t usb_port = USB_PORT[port];

    /* Avoid sending ilm to UART owner*/
    g_UsbHID[usb_port].send_Txilm = false;
    g_UsbHID[usb_port].send_Rxilm = false;

    //hal_usb_polling_rx_data();
}
#endif

/************************************************************
    Bulk EP OUT handle functions
*************************************************************/
/* EP Bulk Out interrupt handler, called by EP interrupt */
// for echo test, dma
//ATTR_RWDATA_IN_NONCACHED_RAM static uint8_t  data[10];

void USB_Hid_BulkOut_Hdr(uint8_t port)
{
}


/* EP Bulk In reset handler */
void USB_Hid_BulkIn_Reset(uint8_t port)
{
//    g_UsbHID[port].txpipe = &g_UsbHID[port].data_ep_in_info->ep_status.epin_status;
}


/* EP Bulk Out reset handler */
void USB_Hid_BulkOut_Reset(uint8_t port)
{
//    g_UsbHID[port].rxpipe = &g_UsbHID[port].data_ep_out_info->ep_status.epout_status;
}

/* When PC return ACK, handle HID intr pipe state machine */
void USB_Hid_IntrIn_Hdlr(uint8_t port)
{
#if 0
    uint32_t len;
    uint32_t max_intr_pkt_size;

    if (gUsbDevice.nDevState != DEVSTATE_CONFIG) {
        /* USB reset */
        //USB_Dbg_Trace(USB_HID_INTERRUPT_RESET, 0, 0);
        g_UsbHID[port].intr_state = HID_INTR_IDLE;
        return;
    }

    max_intr_pkt_size = USB_Intr_Max_Packet_Size();

    switch (g_UsbHID[port].intr_state) {
        case HID_INTR_SEND_ONE_PKT:
            g_UsbHID[port].intr_state = HID_INTR_SEND_LAST_PKT;

            len = ((uint32_t)sizeof(UsbHid_Intr_Pkt)) - max_intr_pkt_size;

            /* send last short packet */
            hal_usb_write_endpoint_fifo(g_UsbHID[port].intrpipe->byEP, len, (uint8_t *)(uint32_t)&g_UsbHID[port].intr_pkt + max_intr_pkt_size);
            hal_usb_set_endpoint_tx_ready(g_UsbHID[port].intrpipe->byEP);
            break;

        case HID_INTR_SEND_LAST_PKT:
            g_UsbHID[port].intr_state = HID_INTR_IDLE;

            /* Send another intr pkt */
            if (g_UsbHID[port].is_intr_pending_pkt == true) {
                g_UsbHID[port].is_intr_pending_pkt = false;
            }
            break;

        case HID_INTR_IDLE:
        default:
            break;
    }
#endif
}


void USB_Hid_IntrIn_Reset(uint8_t port)
{
//  g_UsbHID[port].intr_state = HID_INTR_IDLE;
}

#endif //MTK_USB_DEMO_ENABLED
