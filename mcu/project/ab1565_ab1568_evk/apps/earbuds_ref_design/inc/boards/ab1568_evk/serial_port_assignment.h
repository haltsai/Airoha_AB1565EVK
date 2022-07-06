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

#ifndef __SERIAL_PORT_ASSIGNMENT_H__
#define __SERIAL_PORT_ASSIGNMENT_H__

#include "hal_platform.h"
#include "hal_uart.h"
#include "serial_port.h"
#include "mux.h"

#ifdef __cplusplus
extern "C" {
#endif



/*This port only for system initial period syslog output port, And USB Port cannot assign to it.
* Because USB not work in this period.
*
* ####MUST BE UART PORT####*/
#define     CONFIG_SYSLOG_INIT_STAGE_PORT       HAL_UART_0

/* This port is for task running period syslog output port. This Port can be USB or UART types
* If your want use USB port, must be define  MTK_USB_ENABLED  in file( such as:
* ./GCC/feature_ab1552_evb.mk).*/
#ifdef MTK_PORT_SERVICE_ENABLE
#define     CONFIG_SYSLOG_RUNNING_STAGE_PORT    SERIAL_PORT_DEV_UART_0 //SERIAL_PORT_DEV_USB_COM1
#else
#define     CONFIG_SYSLOG_RUNNING_STAGE_PORT    HAL_UART_0
#endif

#ifdef HAL_UART_FEATURE_3M_BAUDRATE
#define     CONFIG_SYSLOG_BAUDRATE      HAL_UART_BAUDRATE_3000000
#else
#define     CONFIG_SYSLOG_BAUDRATE      HAL_UART_BAUDRATE_921600
#endif



/*This port only for ATCI communication port, And can be USB or UART Port.
* If you want use USB Port for ATCI, must be define   MTK_ATCI_VIA_PORT_SERVICE  &
* MTK_PORT_SERVICE_ENABLE   in file( such as:./GCC/feature_ab1552_evb.mk).*/
#define     CONFIG_ATCI_PORT            MUX_UART_2  //SERIAL_PORT_DEV_USB_COM1
#define     CONFIG_ATCI_BAUDRATE        HAL_UART_BAUDRATE_921600


/*This port only for RACE communication port, And can be USB or UART Port.
* If you want use USB Port for ATCI, must be define   MTK_ATCI_VIA_PORT_SERVICE  &
* MTK_PORT_SERVICE_ENABLE   in file( such as:./GCC/feature_ab1552_evb.mk). */
#define     CONFIG_RACE_PORT            CONFIG_SYSLOG_RUNNING_STAGE_PORT  //SERIAL_PORT_DEV_USB_COM1
#define     CONFIG_RACE_BAUDRATE        HAL_UART_BAUDRATE_921600


/*
* Config ONE  WIRE UART Info
*
*/
#define     PORT_UART_PORT_NUMBER       HAL_UART_1              /*one wire uart port,decide by your architect*/
#define     PORT_UART_BAUDRATE          HAL_UART_BAUDRATE_115200/*one wire uart baudrate*/

/*Below macro can be found in Chip 's Datasheet*/
#define     PORT_UART_TX_PIN            HAL_GPIO_10             /*one wire uart tx pin, decide by chip*/
#define     PORT_UART_TX_UART_MODE      HAL_GPIO_10_UART1_TXD   /*one wire uart tx pin mode, decide by chip*/

#define     PORT_UART_RX_PIN            HAL_GPIO_9             /*one wire uart rx pin, decide by chip*/
#define     PORT_UART_RX_UART_MODE      HAL_GPIO_9_UART1_RXD   /*one wire uart rx pin mode, decide by chip*/


#define   GET_SERIAL_PORT_TYPE_INDEX_FROM_PORT_DEV(port_dev, type, idx)   \
         do{                                                            \
                if(port_dev <= SERIAL_PORT_DEV_UART_TYPE_END) {         \
                    type = SYSLOG_PORT_TYPE_UART;                       \
                    idx  = port_dev - SERIAL_PORT_DEV_UART_TYPE_BEGIN;  \
                } else if(port_dev <= SERIAL_PORT_DEV_USB_TYPE_END) {   \
                    type = SYSLOG_PORT_TYPE_USB;                        \
                    idx  = port_dev - SERIAL_PORT_DEV_USB_TYPE_BEGIN;   \
                } else {                                                \
                    type = SYSLOG_PORT_TYPE_UART;                       \
                    idx  = 0;                                           \
                }                                                       \
            }while(0)                                                   \




#ifdef __cplusplus
}
#endif
#endif /*End  SERIAL_PORT_ASSIGN_H*/


