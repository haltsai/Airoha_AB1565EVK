/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#ifndef _RACE_CMD_RELAY_CMD_COSYS_H_
#define _RACE_CMD_RELAY_CMD_COSYS_H_

#include "race_cmd_feature.h"


#include "race_cmd.h"
#include "bt_aws_mce.h"
#include "bt_sink_srv.h"
#include "race_xport.h"

/**
 * @addtogroup Relay
 * @{
 */
////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x02
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0D 0x00
 * </table>
 * <b>[Field Description]</b>
 *    Query relay state.
 * <b>[Example]</b>
 *    An example for query relay state is shown as below.
 *    0x05 0x5A 0x02 0x00 0x00 0x0D
 * <b>[Note]</b>
 *    None
 * </pre>
 */

 /** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055B
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 4 bytes           <td> 0x00 0x04
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0D 0x00
 * <tr><td> Channel Type                 <td> 1 byte            <td> 0x05
 * <tr><td> Channel ID                   <td> 1 byte            <td> 0x04
 * </table>
 * <b>[Field Description]</b>
 *    The response will query relay state and get channel type and channel id.
 * <b>[Example]</b>
 *    An example for a response with the channel type and channel id are shown as below.
 *    0x05 0x5B 0x04 0x00 0x00 0x0D 0x05 0x04
 * <b>[Note]</b>
 *    None
 * </pre>
 */
#define RACE_CMDRELAY_GET_AVA_DST 0x0D00

/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x0A
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0D 0x01
 * <tr><td> Dst Type                     <td> 1 bytes           <td> 0x05
 * <tr><td> Dst ID                       <td> 1 bytes           <td> 0x06
 * <tr><td> Payload                      <td> Variable bytes    <td> 0x05 0x5A 0x02 0x00 0x0C 0x02
 * </table>
 * <b>[Field Description]</b>
 *    Relay payload to Destination with response.
 * <b>[Example]</b>
 *    An example for relay payload to Destination is shown as below.
 *    0x05 0x5A 0x0A 0x00 0x01 0x0D 0x05 0x06 0x05 0x5A 0x02 0x00 0x0C 0x02
 * <b>[Note]</b>
 *    None
 * </pre>
 */

 /** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055B
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 3 bytes           <td> 0x00 0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0D 0x01
 * <tr><td> Status                       <td> 1 byte            <td> 0x00
 * </table>
 * <b>[Field Description]</b>
 *    The status indicates if the command is processed successfully or not. If the status is 0x00, the relay command successed. Otherwise, the command fails.
 * <b>[Example]</b>
 *    An example for a response status.
 *    0x05 0x5B 0x03 0x00 0x01 0x0D 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */

/** <pre>
 * <b>[Command]</b>
 *    0x055C
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x0A
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0D 0x01
 * <tr><td> Dst Type                     <td> 1 bytes           <td> 0x05
 * <tr><td> Dst ID                       <td> 1 bytes           <td> 0x06
 * <tr><td> Payload                      <td> Variable bytes    <td> 0x05 0x5A 0x02 0x00 0x0C 0x02
 * </table>
 * <b>[Field Description]</b>
 *    Relay payload to Destination without response.
 * <b>[Example]</b>
 *    An example for relay payload to Destination without response is shown as below.
 *    0x05 0x5C 0x0A 0x00 0x01 0x0D 0x05 0x06 0x05 0x5A 0x02 0x00 0x0C 0x02
 * <b>[Note]</b>
 *    None
 * </pre>
 */

 /** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055D
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x0A
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0D 0x01
 * <tr><td> Src Type                     <td> 1 bytes           <td> 0x05
 * <tr><td> Src ID                       <td> 1 bytes           <td> 0x06
 * <tr><td> Payload                      <td> Variable bytes    <td> 0x05 0x5A 0x02 0x00 0x0C 0x02
 * </table>
 * <b>[Field Description]</b>
 *    The Notification will relay payload to source.
 * <b>[Example]</b>
 *    An example for a notification that relay payload to source is shown as below.
 *    0x05 0x5D 0x0A 0x00 0x01 0x0D 0x05 0x06 0x05 0x5A 0x02 0x00 0x0C 0x02
 * <b>[Note]</b>
 *    None
 * </pre>
 */

#define RACE_CMDRELAY_PASS_TO_DST 0x0D01

/**
*@}
*/

/* Allign 1562. */
#define RACE_RELAY_CHANNEL_TYPE_UART 0x00
#define RACE_RELAY_CHANNEL_ID_UART 0x02

#define RACE_CMD_RELAY_FROM_AGENT 0x00
#define RACE_CMD_RSP_FROM_PARTNER 0x01
#define RACE_CMD_RELAY_TO_REMOTE_INTERNAL 0x03
#define RACE_CMD_RSP_FROM_REMOTE_INTERNAL 0x04

#define RACE_CMD_HDR_LEN sizeof(RACE_COMMON_HDR_STRU)

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



/************************************* NOTI Definition End *************************************/


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*!
  @brief Process RELAY RACE CMD related RACE commands.

  @param pRaceHeaderCmd This parameter represents the raw data such as "05 5A...".
  @param Lenth Total bytes of this RACE command.
  @param channel_id Channel identifier
*/

bt_status_t bt_send_aws_mce_race_cmd_data(race_pkt_t *race_pkt, uint16_t length, uint8_t channel_id, uint8_t type, uint8_t send_idx);
void race_cmd_relay_aws_mce_msg_process(race_general_msg_t *msg);
void* RACE_CmdHandler_RELAY_RACE_CMD(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id);
void race_relay_cmd_init(void);
bool race_relay_send_cosys_to_remote_internal(race_pkt_t *race_pkt, uint16_t length, uint8_t channel_id);

#endif /* _RACE_CMD_RELAY_CMD_COSYS_H_ */

