/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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


#ifndef __BT_AWS_MCE_REPORT_INTERNAL_H__
#define __BT_AWS_MCE_REPORT_INTERNAL_H__

#include <stdbool.h>
#include <stdint.h>
#include "bt_type.h"
#include "bt_aws_mce_report.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** @brief The structure of callback table. */
typedef struct {
    bt_aws_mce_report_module_id_t    module_id;       /**< The owner of this action. */
    bt_aws_mce_report_callback_t callback;           /**< The callback of the owner. */
} bt_aws_mce_report_callback_table_t;

/**
*  @brief This structure is the header of #bt_sink_srv_aws_mce_packet_t.
*/
typedef struct {
    bt_aws_mce_report_module_id_t  module_id;    /**<  The module id of the application for the AWS MCE packet. */
    uint8_t                      reserved;       /**<  The reserved byte. */
    uint16_t                     payload_length; /**<  The payload length of the AWS MCE packet. */
} bt_aws_mce_report_packet_header_t;

/**
 *  @brief This structure is the bt_aws_mce_report packet format.
 *     1byte       1byte             2bytes               length bytes
 * +------ +---------+----------------+-------------+
 * | Module |  Reserved |    Payload Length   |      Payload    |
 * +------ +---------+----------------+-------------+
 */
typedef struct {
    bt_aws_mce_report_packet_header_t header; /**<  The header of the AWS MCE packet. */
    uint8_t payload[1];                       /**<  The payload  of the AWS MCE packet. */
} bt_aws_mce_report_packet_t;


/**
 *  @brief This structure defines the parameters of sync payload header format.
 */
typedef struct {
    bool       is_sync;          /**< The parameter indicates whether this event is synced between Agent and Partner or not.*/
    uint8_t         len;         /**< The length of the parameter. */
    uint16_t      nclk_intra;    /**< The Bluetooth clock is shown in microseconds.*/
    uint32_t      nclk;          /**< The Bluetooth clock unit is 312.5 microseconds.*/
} bt_aws_mce_report_sync_payload_header_t;


/**
 *  @brief This structure defines the parameters of sync payload packet.
 */
typedef struct {
    bt_aws_mce_report_sync_payload_header_t payload_header; /**< The header of the synced payload packet. */
    uint8_t  param[1];                                      /**< The params of the event to be synced between Agent and Partner. */
} bt_aws_mce_report_sync_payload_t;


/**
 *  @brief This structure defines the parameters of no-sync payload header format.
 */
typedef struct {
    bool       is_sync;   /**< The parameter indicates whether this event is synced between Agent and Partner or not. */
    uint16_t  len;         /**< The length of the parameter. */
    uint16_t padding;     /**< The padding bytes to align 4 byte*/
} bt_aws_mce_report_payload_header_t;

/**
 *  @brief This structure defines the parameters of no-sync payload packet.
 */
typedef struct {
    bt_aws_mce_report_payload_header_t payload_header;/**< The header of no-sync payload packet. */
    uint8_t  param[1];                                /**< The parameters of the event. */
} bt_aws_mce_report_payload_t;


/**
 *  @brief This structure defines the parameters of this module context.
 */
typedef struct {
    uint32_t aws_handle;                           /**< The aws handle. */
    bt_bd_addr_t remote_addr;                      /**< The remote address of the connection handle. */
    bt_aws_mce_role_t role;                        /**< The role of the local device. */
    bt_aws_mce_agent_state_type_t aws_state;       /**< The aws state of the local device. */
} bt_aws_mce_report_context_t;

BT_PACKED(
typedef struct
{
    uint8_t module_id;
    uint16_t data_len;
    uint8_t data[1];
})bt_aws_mce_report_old_packet_t;

/**
 *  @brief This structure defines the parameters of non-sync payload header format.
 */
typedef struct {
    bt_aws_mce_report_packet_header_t common_header;
    bt_aws_mce_report_payload_header_t none_sync_header;
} bt_aws_mce_report_none_sync_header_t;

/**
 *  @brief This structure defines the parameters of sync payload header format.
 */
typedef struct {
    bt_aws_mce_report_packet_header_t common_header;
    bt_aws_mce_report_sync_payload_header_t sync_header;
} bt_aws_mce_report_sync_header_t;

BT_PACKED(
typedef struct
{
    uint8_t module_id;
    uint16_t data_len;
})bt_aws_mce_report_old_header_t;

#define BT_AWS_MCE_PACKET_PAYLOAD_MAX_DATA_LENGTH  (880-sizeof(bt_aws_mce_report_packet_header_t)) /**<This value is the max payload length of the bt_aws_mce_report packet.*/
#define BT_AWS_MCE_MUX_PAYLOAD_MAX_DATA_LENGTH         880
  
/**
 * @brief               This function sends information data from the Agent to Partner or the Partner to Agent. The #BT_AWS_MCE_INFOMATION_PACKET_IND event is received at the remote end.
 *                      Note: The length for the information packet must be less than 200B.
 * @param[in] handle    is the connection handle for the specified link.
 * @param[in] information defines the updated information from the upper layer.
 * @param[in] urgent defines if the information packet should be sent out instantly. To ensure the audio quality, the urgent is set true only for A2DP and Call information.
 * @param[in] header    is the extra data which will be append in information->data's head. Set NULL if there is no extra header needed.
 * @param[in] header_length is the length of extra header.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation failed because there is not enough memory.
 */
extern bt_status_t bt_aws_mce_send_information_with_extra_header(uint32_t handle, const bt_aws_mce_information_t *information, bool urgent, uint8_t *header, uint16_t header_length);


#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 * @}
 */


#endif /* __BT_AWS_MCE_REPORT_INTERNAL_H__ */

