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


#ifndef __RACE_CMD_FACTRST_H__
#define __RACE_CMD_FACTRST_H__

/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x04
 * <tr><td> ID                           <td> 2 bytes           <td> 0x11 0x01
 * <tr><td> KEY_EVENT                    <td> 2 byte            <td> XX XX
 * </table>
 * <b>[Field Description]</b>
 *    Send key event by race cmd.
 * <b>[Example]</b>
 *    An example for sending a key event.
 *    0x05 0x5A 0x04 0x00 0x01 0x11 0x95 0x00
 * <b>[Note]</b>
 *    None.
 * </pre>
 */

/** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055B
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x11 0x01
 * <tr><td> Status                       <td> 1 byte            <td> 0x00 or 0xFF
 * </table>
 * <b>[Field Description]</b>
 *    The response will report status, 0x00 for success and 0xFF for fail.
 * <b>[Example]</b>
 *    An example for a response is shown as below.
 *    0x05 0x5B 0x03 0x00 0x01 0x11 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */

#include "race_cmd_feature.h"
#include "race_cmd.h"
#include "race_xport.h"
#include "race_util.h"
#include "bt_type.h"
#include "hal_wdt.h"
#include "bt_gap.h"
#include "bt_connection_manager.h"
#include "race_cmd_bluetooth.h"
#include "bt_connection_manager_device_local_info.h"
#include "bt_sink_srv.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_ID_ATPORT_EVENT     0x1100

#define RACE_ID_KEY_EVENT     0x1101
#define RACE_KEY_ID_POWEROFF  0x0018
#define RACE_KEY_ID_FACTRST   0x0095
#define RACE_KEY_ID_FACTRST_AND_POWEROFF    0x0096
#define RACE_KEY_ID_GAMEMODE_ON       0x00A4
#define RACE_KEY_ID_GAMEMODE_OFF      0x00A5
#define RACE_KEY_ID_GAMEMODE_TOGGLE   0x00A6
#define RACE_KEY_ID_LEAUDIO_ON          0x00B1
#define RACE_KEY_ID_LEAUDIO_OFF         0x00B2
#define RACE_KEY_ID_LEAUDIO_TOGGLE      0x00B3

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void* RACE_CmdHandler_KEY(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id);



#endif /* __RACE_CMD_FCD_H__ */

