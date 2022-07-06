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

#ifndef _RACE_CMD_FOTA_H_
#define _RACE_CMD_FOTA_H_

#include "race_cmd_feature.h"
#ifdef RACE_FOTA_CMD_ENABLE
#include "fota_platform.h"
#include "fota_util.h"
#include "race_cmd.h"
#include "race_util.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_FOTA_QUERY_PARTITION_INFO               0x1C00
#define RACE_FOTA_CHECK_INTEGRITY                    0x1C01
#define RACE_FOTA_COMMIT                             0x1C02
#define RACE_FOTA_STOP                               0x1C03
#define RACE_FOTA_QUERY_STATE                        0x1C04
#define RACE_FOTA_WRITE_STATE                        0x1C06
/**
 * @addtogroup FOTA
 * @{
 */

/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x1C 0x07
 * <tr><td> agent_or_partner             <td> 1 byte            <td> 0x00: Agent \n 0x01: Partner
 * </table>
 * <b>[Field Description]</b>
 *    Query the FOTA release version of the agent or the partner.
 * <b>[Example]</b>
 *    An example for querying the FOTA release version of the agent is shown as below.
 *    0x05 0x5A 0x03 0x00 0x07 0x1C 0x00
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
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x1C 0x07
 * <tr><td> status                       <td> 1 byte            <td> 0x00: SUCCESS \n else: FAIL
 * </table>
 * <b>[Field Description]</b>
 *    The status indicates if the command is being processed or not. If the status of 0x00 is returned, a notification containing the FOTA
 *    release version will be returned later. Otherwise, the command fails.
 * <b>[Example]</b>
 *    An example for a response with the status of 0x00 is shown as below.
 *    0x05 0x5B 0x03 0x00 0x07 0x1C 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */

/** <pre>
 * <HR>
 * <b>[Notification]</b>
 *    0x055D
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> The total length of all command fields following the length field
 * <tr><td> ID                           <td> 2 bytes           <td> 0x1C 0x07
 * <tr><td> status                       <td> 1 byte            <td> 0x00: SUCCESS \n else: FAIL
 * <tr><td> agent_or_partner             <td> 1 byte            <td> 0x00: Agent \n 0x01: Partner
 * <tr><td> version_len                  <td> 2 bytes           <td> The length of the FOTA release version
 * <tr><td> version                      <td> Variable bytes    <td> The FOTA release version
 * </table>
 * <b>[Field Description]</b>
 *    The status indicates if the command is processed successfully or not. If the status is 0x00, the FOTA release version is included in
 *    the notification. Otherwise, the command fails.
 * <b>[Example]</b>
 *    An example for a notification with the status of 0x00 is shown as below.
 *    0x05 0x5D 0x0B 0x00 0x07 0x1C 0x00 0x00 0x06 0x76 0x31 0x2E 0x30 0x2E 0x30
 * <b>[Note]</b>
 *    None
 * </pre>
 */
#define RACE_FOTA_GET_VERSION                        0x1C07

/**
*@}
*/
#define RACE_FOTA_START                              0x1C08
#define RACE_FOTA_NEW_TRANSACTION                    0x1C0A
#define RACE_FOTA_DUAL_DEVICES_NEW_TRANSACTION       0x1C10
#define RACE_FOTA_DUAL_DEVICES_COMMIT                0x1C11
#define RACE_FOTA_DUAL_DEVICES_QUERY_STATE           0x1C12
#define RACE_FOTA_DUAL_DEVICES_WRITE_STATE           0x1C13
#define RACE_FOTA_DUAL_DEVICES_QUERY_PARTITION_INFO  0x1C14
#define RACE_FOTA_ACTIVE_FOTA_PREPARATION            0x1C19
#define RACE_FOTA_PING                               0x1C1B
#define RACE_FOTA_QUERY_TRANSMIT_INTERVAL            0x1C1C


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
typedef struct
{
    uint8_t recipient;
    uint8_t storage_type;
}PACKED race_fota_check_integrity_recipient_param_struct;
#endif

typedef struct
{
    uint8_t recipient;
    uint16_t transmit_interval;
}PACKED race_fota_query_transmit_interval_recipient_param_struct;

/************************************* NOTI Definition Begin *************************************/
typedef struct
{
    uint8_t status;
    uint8_t agent_or_partner;
    uint8_t version_len;
    uint8_t version[0];
}PACKED race_fota_get_version_noti_struct;

typedef struct
{
    uint8_t status;
} PACKED race_fota_dual_start_transaction_noti_struct;

typedef struct
{
    uint8_t status;
    uint8_t recipient;
    uint8_t fota_mode; /* 0: background; 1: active */
} PACKED race_fota_start_noti_struct;

typedef struct
{
    uint8_t status;
    uint16_t agent_fota_state;
    uint16_t partner_fota_state;
} PACKED race_fota_dual_query_state_noti_struct;

typedef struct
{
    uint8_t status;
    uint16_t agent_fota_state;
    uint16_t client_fota_state;
} PACKED race_fota_dual_write_state_noti_struct;

typedef struct
{
    uint8_t status;
    uint8_t partition_id;
    uint8_t agent_storage_type;
    uint32_t agent_partition_addr;
    uint32_t agent_partition_len;
    uint8_t cli_storage_type;
    uint32_t cli_partition_addr;
    uint32_t cli_partition_len;
} PACKED race_fota_dual_query_partition_info_noti_struct;

typedef struct
{
    uint8_t status;
    uint8_t agent_or_partner;
}PACKED race_fota_active_fota_preparation_noti_struct;

typedef struct
{
    uint8_t status;
    uint8_t sender;
    uint8_t recipient;
}PACKED race_fota_stop_noti_struct;

#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
typedef struct
{
    uint8_t status;
    uint8_t recipient_count;
    race_fota_check_integrity_recipient_param_struct recipient_param[0];
} PACKED race_fota_check_integrity_noti_struct;
#endif

typedef struct
{
    uint8_t status;
    uint8_t recipient_count;
    race_recipient_param_general_struct recipient_param[0];
} PACKED race_fota_ping_noti_struct;

typedef struct
{
    uint8_t status;
    uint8_t recipient_count;
    race_fota_query_transmit_interval_recipient_param_struct recipient_param[0];
} PACKED race_fota_query_transmit_interval_noti_struct;
/************************************* NOTI Definition End *************************************/


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*!
  @brief Process FOTA related RACE commands.

  @param pRaceHeaderCmd This parameter represents the raw data such as "05 5A...".
  @param Lenth Total bytes of this RACE command.
  @param channel_id Channel identifier
*/

void* RACE_CmdHandler_FOTA(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id);

#endif /* RACE_FOTA_CMD_ENABLE */
#endif /* _RACE_CMD_FOTA_H_ */

