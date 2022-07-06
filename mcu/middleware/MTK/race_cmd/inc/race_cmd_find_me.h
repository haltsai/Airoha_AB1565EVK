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

#ifndef _RACE_CMD_FIND_ME_H_
#define _RACE_CMD_FIND_ME_H_

#include "race_cmd_feature.h"

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif

#ifdef RACE_FIND_ME_ENABLE
#include "race_cmd.h"
#include "race_event.h"

/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x02
 * <tr><td> ID                           <td> 2 bytes           <td> 0x2C 0x00
 * </table>
 * <b>[Field Description]</b>
 *    Query race find me state.
 * <b>[Example]</b>
 *    An example for sending a key event.
 *    0x05 0x5A 0x02 0x00 0x00 0x2C
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
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x04
 * <tr><td> ID                           <td> 2 bytes           <td> 0x2C 0x00
 * <tr><td> Status                       <td> 1 byte            <td> 0x00 or 0x02
 * <tr><td> Connection status            <td> 1 byte            <td> 0x00 or 0x01
 * </table>
 * <b>[Field Description]</b>
 *    The response will report status, 0x00 for success and 0x02 for not supoort.
 * <b>[Example]</b>
 *    An example for a response is shown as below.
 *    0x05 0x5B 0x04 0x00 0x00 0x2C 0x00 0x01
 * <b>[Note]</b>
 *    None
 * </pre>
 */

/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x05
 * <tr><td> ID                           <td> 2 bytes           <td> 0x2C 0x01
 * <tr><td> Is_blink                     <td> 1 bytes           <td> 0x00 or 0x01
 * <tr><td> Is_tone                      <td> 1 bytes           <td> 0x00 or 0x01
 * <tr><td> Left or right                <td> 1 bytes           <td> 0x00 or 0x01
 * </table>
 * <b>[Field Description]</b>
 *    Trigger find me.
 * <b>[Example]</b>
 *    An example for trigger find me on left or right ear.
 *    0x05 0x5A 0x05 0x00 0x01 0x2C 0x01 0x01 0x00
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
 * <tr><td> ID                           <td> 2 bytes           <td> 0x2C 0x01
 * <tr><td> Status                       <td> 2 byte            <td> 0x00 or 0x01
 * </table>
 * <b>[Field Description]</b>
 *    The response will report status, 0x00 for success and 0x01 for fail.
 * <b>[Example]</b>
 *    An example for a response is shown as below.
 *    0x05 0x5B 0x03 0x00 0x01 0x2C 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */

////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_FIND_ME_QUERY_STATE    0x2C00
#define RACE_CMD_FIND_ME 0x2C01

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint8_t status;
    uint8_t recipient;
} PACKED race_find_me_noti_struct;

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
typedef struct
{
    uint8_t by_spp;
    uint8_t by_ble;
}PACKED race_find_me_by_method_t;
#endif

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

void* RACE_CmdHandler_FIND_ME(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id);

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE

void race_cmd_find_me_init();
 
void race_cmd_find_me_deinit();

void race_cmd_set_find_me_trans_method(race_event_type_enum event_type);

#endif

#endif /* RACE_FIND_ME_CMD_ENABLE */
#endif /* _RACE_CMD_FIND_ME_H_ */

