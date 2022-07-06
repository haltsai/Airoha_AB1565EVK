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

#ifndef USBHID_ADAP_H
#define USBHID_ADAP_H

#if 0
typedef enum {
    CDC_ONLY_PUSH = 0,  //Only push buffer  ( initial sate: prepare buffer for USB )
    CDC_ONLY_GET,       //  Only get data   ( there might be no FLC buffer anymore )
    CDC_GET_DATA,       //  normal state    ( get data & push another buffer )
    CDC_POP_BUFF        //  Get empty buffer  (leave RNDIS mode „³ release FLC buffer )
} USB_Requ_type;


extern void USB2UART_init(void);
extern void USB2UART_Clear_Tx_Buffer(uint8_t port);
extern void USB2UART_Clear_Rx_Buffer(uint8_t port);


extern void USB2UART_ClrRxBuffer(uint8_t port);
extern uint16_t USB2UART_GetRxAvail(uint8_t port);
extern uint16_t USB2UART_GetBufAvail(BUFFER_INFO *buf_info);
extern uint16_t USB2UART_GetBytes(uint8_t port, uint8_t *buffaddr, uint16_t length);
extern void USB2UART_ClrTxBuffer(uint8_t port);
extern uint16_t USB2UART_GetTxRoomLeft(uint8_t port);
extern uint16_t USB2UART_PutBytes(uint8_t port, uint8_t *buffaddr, uint16_t length);
extern uint8_t USB2UART_GetByte_Polling(uint8_t port);
extern void USB2UART_PutData_Polling(uint8_t port, uint8_t *Buffaddr, uint16_t Length);
extern uint16_t USB2UART_GetBuffRoomLeft(BUFFER_INFO *buf_info);
extern uint16_t USB2UART_GetTxRoomLeft(uint8_t port);
#endif
#endif /* USBHID_ADAP_H */

