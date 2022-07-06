/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#ifndef _RACE_CFU_H_
#define _RACE_CFU_H_

#include "race_cmd_feature.h"
#ifdef RACE_CFU_ENABLE
#include "types.h"
#include "race_cmd.h"


#define RACE_CFU_PACKET_ID_INVALID    (0)


typedef enum
{
    RACE_CFU_UPDATE_MODE_UNKNOWN,    /**< Do not know or do not care the update mode. */
    RACE_CFU_UPDATE_MODE_SINGLE_DEVICE,    /**< Only a single device which is connected with the dongle directly will be updated. */
    RACE_CFU_UPDATE_MODE_DUAL_DEVICE,    /**< Both the agent and the partner of the earbuds will be updated. */

    RACE_CFU_UPDATE_MODE_MAX = 0xFF    /**< The maximum value of this enum. */
} race_cfu_udpate_mode_enum;


typedef enum
{
    RACE_CFU_PACKET_TYPE_CMD,    /**< Command */
    RACE_CFU_PACKET_TYPE_RSP,    /**< Response */

    RACE_CFU_PACKET_TYPE_MAX = 0xFF    /**< The maximum value of this enum. */
} race_cfu_packet_type_enum;


typedef struct
{
    race_cfu_udpate_mode_enum update_mode;    /**< The update mode which is decided by headset/earbuds. */
    race_cfu_packet_type_enum packet_type;    /**< The packet type. */
    U16 packet_id;    /**< A unique id of the packet. */
    U16 data_len;    /**< The length of data in bytes. */
    U8 data[0];    /**< data format: report_count, report_count x report. report format: report id, report data. */
}PACKED race_cfu_packet_struct;


/**
 * @brief Function for receiving the CFU packet from the race cmd CFU sub-module.
 * @param[out] packet the CFU packet to be received.
 *                  packet->update_mode: The udpate mode.
 *                  packet->packet_type: The packet type.
 *                  packet->packet_id: A unique id of the packet to be sent. It is used to match the command and its response.
 *                  packet->data_len: The length of packet->data in bytes.
 *                  packet->data: data format: report_count, report_count x report. report format: report id, report data.
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
typedef void (*race_cfu_receive_callback)(race_cfu_packet_struct *packet);


/**
 * @brief Function for initializing the race cmd CFU sub-module.
 * @param[in] update_mode The default update mode. If it is dongle, it could be set to RACE_CFU_UPDATE_MODE_UNKNOWN. If it is headset, set it to
 *                                      RACE_CFU_UPDATE_MODE_SINGLE_DEVICE. If it is earbuds, set it to RACE_CFU_UPDATE_MODE_DUAL_DEVICE.
 * @param[in] recv_cb The callback to receive the CFU packet from the race cmd CFU sub-module.
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
RACE_ERRCODE race_cfu_init(race_cfu_udpate_mode_enum update_mode, race_cfu_receive_callback recv_cb);


/**
 * @brief Function for sending the CFU packet to the race cmd CFU sub-module.
 * @param[in] packet the CFU packet to be sent.
 *                  packet->update_mode: The udpate mode. Set it to RACE_CFU_UPDATE_MODE_UNKNOWN if do not know or do not care.
 *                  packet->packet_type: The packet type.
 *                  packet->packet_id: A unique id of the packet to be sent. It is used to match the command and its response. When the packet_type
 *                                              is response, it should be the same packet_id of the corresponding command received before.
 *                  packet->data_len: The length of packet->data in bytes.
 *                  packet->data: data format: report_count, report_count x report. report format: report id, report data.
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
RACE_ERRCODE race_cfu_send(race_cfu_packet_struct *packet);


/**
 * @brief Function for getting the update mode.
 * @return RACE_CFU_UPDATE_MODE_MAX fail; otherwise, succeed.
 */
race_cfu_udpate_mode_enum race_cfu_get_update_mode(void);


/**
 * @brief Function for setting the update mode. update_mode will be reset to the default value automatically after reboot. And changing the update mode
 *           during cfu download will make the current update fail.
 * @param[in] update_mode The new update mode.
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
RACE_ERRCODE race_cfu_set_update_mode(race_cfu_udpate_mode_enum update_mode);


#endif /* RACE_CFU_ENABLE */
#endif /* _RACE_CFU_H_ */

