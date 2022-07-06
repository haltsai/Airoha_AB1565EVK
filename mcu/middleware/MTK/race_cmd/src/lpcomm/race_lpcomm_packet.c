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


#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_xport.h"
#include "race_lpcomm_packet.h"
#include "race_lpcomm_trans.h"


void race_lpcomm_packet_free(uint8_t *packet)
{
    if (packet)
    {
        race_mem_free(packet);
    }
}


race_lpcomm_packet_struct *race_lpcomm_packet_header_get(uint8_t *packet, race_lpcomm_trans_method_enum method)
{
    race_lpcomm_trans_info_struct *method_info = race_lpcomm_trans_find_info(method);

    if (!packet || !method_info)
    {
        return NULL;
    }

    return (race_lpcomm_packet_struct *)((uint8_t *)packet + method_info->header_len);
}


uint8_t *race_lpcomm_packet_create(uint8_t *payload,
                                   uint16_t payload_len,
                                   race_lpcomm_role_enum sender_role,
                                   race_lpcomm_packet_type_enum packet_type,
                                   uint16_t cmd_id,
                                   uint8_t app_id,
                                   uint8_t channel_id,
                                   uint16_t process_id,
                                   race_lpcomm_trans_method_enum trans_method)
{
    uint8_t *packet = NULL;
    race_lpcomm_packet_struct *lpcomm_packet = NULL;
    race_lpcomm_trans_info_struct *method_info = race_lpcomm_trans_find_info(trans_method);

    if (!method_info ||
        (!payload && payload_len) ||
        (payload && !payload_len))
    {
        return NULL;
    }

    packet = (uint8_t *)race_mem_alloc(method_info->header_len + RACE_LPCOMM_PACKET_HEADER_SIZE + payload_len);
    if (packet)
    {
        memset(packet, 0, method_info->header_len + RACE_LPCOMM_PACKET_HEADER_SIZE + payload_len);

        /* 1. Init the trans header */
        if (method_info->header_init_func)
        {
            method_info->header_init_func(packet, RACE_LPCOMM_PACKET_HEADER_SIZE + payload_len);
        }

        /* 2. Init the race header */
        lpcomm_packet = (race_lpcomm_packet_struct *)(packet + method_info->header_len);

        /* packet_type */
#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
        RACE_LPCOMM_PACKET_SET_SENDER_ROLE(packet_type, sender_role);
#endif
        lpcomm_packet->packet_type = packet_type;

        /* cmd_id */
        lpcomm_packet->cmd_id = cmd_id;

        /* app_id */
        lpcomm_packet->app_id = app_id;

        /* channel_id */
        lpcomm_packet->channel_id = channel_id;

        /* process_id */
        lpcomm_packet->process_id = process_id;

        /* trans_method */
        lpcomm_packet->trans_method = trans_method;

        /* payload_len */
        lpcomm_packet->payload_len = payload_len;

        /* 3. Init the payload */
        if (payload)
        {
            memcpy(lpcomm_packet->payload, payload, payload_len);
        }
    }

    return packet;
}
#endif /* RACE_LPCOMM_ENABLE */

