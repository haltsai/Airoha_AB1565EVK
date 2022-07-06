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


#ifndef __RACE_CMD_BLUETOOTH_H__
#define __RACE_CMD_BLUETOOTH_H__


#include "race_cmd_feature.h"
#ifdef RACE_BT_CMD_ENABLE
#include "stdint.h"
#include "bt_type.h"
#include "bt_hfp.h"
#include "race_cmd.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_BLUETOOTH_SET_LE_CONNECTION_PARAMETER          0x0CD1
#define RACE_BLUETOOTH_GET_LE_LINK_STATUS                   0x0CD2
#define RACE_BLUETOOTH_GET_CIENT_EXISTENCE                  0x0CD3    /* AWS MCE agent & client */
#define RACE_BLUETOOTH_IS_AGENT_RIGHT_DEVICE                0x0CD4    /* right or left device */

/**
 * @addtogroup BLUETOOTH
 * @{
 */
/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0C 0xD5
 * <tr><td> Role                         <td> 1 byte            <td> 0x00 for agent, 0x01 for partner
 * </table>
 * <b>[Field Description]</b>
 *    BD address read.
 * <b>[Example]</b>
 *    An example for getting agent's BD address is shown as below.
 *    0x05 0x5A 0x03 0x00 0xD5 0x0C 0x00
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
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x0A
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0C 0xD5
 * <tr><td> Status                       <td> 1 byte            <td> 0x00 for success, others for failure
 * <tr><td> Role                         <td> 1 byte            <td> 0x00 for agent, 0x01 for partner
 * <tr><td> BD address                   <td> 6 bytes           <td> 0x11 0x22 0x33 0x44 0x55 0x66
 * </table>
 * <b>[Field Description]</b>
 *    The response will report BD address of agent.
 * <b>[Example]</b>
 *    An example for a response reporting agent's BD address is shown as below.
 *    0x05 0x5B 0x0A 0x00 0xD5 0x0C 0x00 0x00 0x11 0x22 0x33 0x44 0x55 0x66
 * <b>[Note]</b>
 *    None
 * </pre>
 */
#define RACE_BLUETOOTH_GET_BD_ADDR                          0x0CD5
/**
*@}
*/

/**
 * @addtogroup BLUETOOTH
 * @{
 */
/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0C 0xD6
 * <tr><td> Role                         <td> 1 byte            <td> 0x00 for agent, 0x01 for partner
 * </table>
 * <b>[Field Description]</b>
 *    Battery level read.
 * <b>[Example]</b>
 *    An example for getting agent's battery level is shown as below.
 *    0x05 0x5A 0x03 0x00 0xD6 0x0C 0x00
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
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x05
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0C 0xD6
 * <tr><td> Status                       <td> 1 byte            <td> 0x00 for success, others for failure
 * <tr><td> Role                         <td> 1 byte            <td> 0x00 for agent, 0x01 for partner
 * </table>
 * <b>[Field Description]</b>
 *    The response will report battery level of agent.
 * <b>[Example]</b>
 *    An example for a response reporting agent's battery level is shown as below.
 *    0x05 0x5D 0x05 0x00 0xD6 0x0C 0x00 0x00 0x50
 * <b>[Note]</b>
 *    None
 * </pre>
 */
#define RACE_BLUETOOTH_GET_BATTERY                          0x0CD6
/**
*@}
*/

#define RACE_BLUETOOTH_DUAL_ROLE_SWITCH                     0x0CD7
#define RACE_BLUETOOTH_SET_BLE_ADV                          0x0CD8
#define RACE_BLUETOOTH_EDR_INQUERY_PAGE_SCAN                0x0CD9
#define RACE_BLUETOOTH_DISABLE_WBRSSI                       0x0CDA
#define RACE_BLUETOOTH_SET_SNIFF_PARAM                      0x0CDB
#define RACE_BLUETOOTH_SET_RANDOM_ADDR                      0x0CDC
#define RACE_BLUETOOTH_CONN_BLE_DEV                         0x0CDD
#define RACE_BLUETOOTH_ENABLE_SNIFF_MODE                    0x0CDE
#define RACE_BLUETOOTH_AWS_RESET                            0x0CDF

#define RACE_BLUETOOTH_GET_EDR_LINK_KEY                     0x0CC0
#define RACE_BLUETOOTH_AWS_ENABLE                           0x0CC1
#define RACE_BLUETOOTH_SET_LOCAL_ADDR                       0x0CC2
#define RACE_BLUETOOTH_GET_EDR_CONNECTED_DEV                0x0CC3
#define RACE_BLUETOOTH_AWS_GET_ROLE                         0x0CC4
#define RACE_BLUETOOTH_GET_A2DP_CODEC_PARAMETERS            0x0CCF

#define RACE_BLUETOOTH_GET_RSSI                             0x1700

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    bt_bd_addr_t bd_addr;
    uint8_t bd_addr_type;
}PACKED race_bt_le_link_status_struct;

typedef struct
{
    uint8_t status;
    uint8_t active_link_num;
    race_bt_le_link_status_struct link_status[0];  /* actually size of the array is active_link_num */
}PACKED race_bt_get_le_link_status_noti_struct;

typedef struct
{
    uint8_t status;
    uint8_t addr_type;
    bt_bd_addr_t bd_addr;
}PACKED race_bt_set_le_conn_param_noti_struct;

typedef struct
{
    uint8_t status;
    uint8_t agent_or_partner;
    uint8_t battery_level;
}PACKED race_bt_get_battery_level_noti_struct;

typedef struct
{
    uint8_t status;
}PACKED race_bt_dual_role_switch_noti_struct;

typedef enum {
    RACE_RSSI_PHONE_WITH_DEVICE = 0,
    RACE_RSSI_AGENT_WITH_PARTNER,
    RACE_RSSI_PARTNER_WITH_AGENT
}race_bt_rssi_type_t;

typedef struct {
    int8_t AgentPartnerRssi;
    int8_t AgRssi;
    uint32_t IfpErrCnt;
    uint32_t AclErrCnt;
    uint32_t AudioPktNum;
    uint32_t DspLostCnt;

    int8_t AgentPartnerAagcRssi;
    int8_t AgAagcRssi;
    uint8_t AgentPartnerAagcGain;
    uint8_t AgAagcGain;
}race_bt_rssi_info_t;


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*!
  @brief Process BLUETOOTH related RACE commands.

  @param pRaceHeaderCmd This parameter represents the raw data such as "05 5A...".
  @param Lenth Total bytes of this RACE command.
  @param channel_id Channel identifier
*/
void* RACE_CmdHandler_BLUETOOTH(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id);
void* RACE_CmdHandler_GET_RSSI(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id);
bt_status_t race_bt_set_rssi(void *buff);
bt_status_t race_bt_notify_rssi(void);

#endif /* RACE_BT_CMD_ENABLE */
#endif /* __RACE_CMD_BLUETOOTH_H__ */

