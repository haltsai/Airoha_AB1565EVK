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

#ifndef __RACE_H__
#define __RACE_H__


#include "race_cmd_feature.h"
#include "serial_port.h"


/**
 * @addtogroup Race_CMD_Group Race CMD
 * @{
 * @addtogroup  Race Race
 * @{
 * Race cmd module provides multiple ways to communicate with the smartphone
 * or the PC by using commands with a specific format. It supports the communication
 * using UART, SPP, BLE and so on.
 */


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup race_struct Struct
 * @{
 */

/**
 * @brief The structure for the race message.
 */
typedef struct {
    uint16_t msg_id;    /**< The message ID. */
    serial_port_dev_t dev_t;    /**< The port service device. */
    uint8_t* msg_data;    /**< The message data. */
#if (defined AIR_DONGLE_RELAY_RACE_PACKET_ENABLE) || (defined AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
    uint8_t msg_flag;  /**< The message is for local(0) or by pass to others(1) */
#endif
} race_general_msg_t;

/**
 * @}
 */


/**
 * @brief Function for initializing the race module. It should be called before
 * any other race API is called.
 * @return none.
 */
void race_init(void);


/**
* @brief This function is used for the RACE module. The message will be copied to the message queue and should be freed after this API returns if it is allocated dynamically.
* @param[in] msg It is the data structure for RACE to send the message, including the message ID, device and message pointer.
* @return   RACE_ERRCODE_SUCCESS the RACE sent the message successfully. \n
* @note     The race_send_msg() can select the UART, USB or BT SPP server/client port through port service for data transmission.
* @par       Example
* @code
*       race_general_msg_t msg_queue_item;
*
*       switch (channel_id)
*       {
*           case RACE_SERIAL_PORT_TYPE_UART:
*               msg_queue_item.dev_t = SERIAL_PORT_DEV_UART_0;
*               break;
*           case RACE_SERIAL_PORT_TYPE_SPP:
*               msg_queue_item.dev_t = SERIAL_PORT_DEV_BT_SPP;
*               break;
*           case RACE_SERIAL_PORT_TYPE_BLE:
*               msg_queue_item.dev_t = SERIAL_PORT_DEV_BT_LE;
*               break;
*          case RACE_SERIAL_PORT_TYPE_AIRUPDATE:
*               msg_queue_item.dev_t = SERIAL_PORT_DEV_BT_AIRUPDATE;
*               break;
*           default:
*               msg_queue_item.dev_t = SERIAL_PORT_DEV_UNDEFINED;
*               break;
*       }
*       RACE_ERRCODE error = race_send_msg(&msg_queue_item);
*       if(error != RACE_ERRCODE_SUCCESS) {
*           RACE_LOG_MSGID_E("error log idx:%x",1,error);
*       }
* @endcode
*/
RACE_ERRCODE race_send_msg(race_general_msg_t *msg);

#ifdef __cplusplus
}
#endif


/**
 * @}
 * @}
 */
#endif /* __RACE_H__ */

