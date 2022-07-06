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


#include "race_cmd_feature.h"
#ifdef RACE_CFU_ENABLE
#include "race_xport.h"
#include "race_event.h"
#include "race_cfu.h"
#include "race_cfu_internal.h"
#ifdef RACE_CFU_HANDLER_ENABLE
#include "cfu.h"
#include "race_cfu_handler.h"
#endif
#ifdef RACE_CFU_BUILDER_ENABLE
#include "race_cfu_builder.h"
#endif


/* update_mode will be decided by headset/earbuds and can be changed in headset/earbuds. Dongle should record the update_mode returned in the offer response and
  * set the same update_mode in the following commands to the headset/earbuds. It also should reset it to unknown after sending the last command to the headset/earbuds.
  */
typedef struct
{
    race_cfu_udpate_mode_enum update_mode;
    race_cfu_receive_callback recv_cb;
    S32 register_id;
} race_cfu_cntex_struct;


race_cfu_cntex_struct g_race_cfu_cntex;
race_cfu_cntex_struct *g_race_cfu_cntex_ptr;


race_cfu_receive_callback race_cfu_get_recv_cb(void)
{
    return g_race_cfu_cntex_ptr && g_race_cfu_cntex_ptr->recv_cb ? g_race_cfu_cntex_ptr->recv_cb : NULL;
}


race_cfu_udpate_mode_enum race_cfu_get_update_mode(void)
{
    return g_race_cfu_cntex_ptr ? g_race_cfu_cntex_ptr->update_mode : RACE_CFU_UPDATE_MODE_MAX;
}


RACE_ERRCODE race_cfu_set_update_mode(race_cfu_udpate_mode_enum update_mode)
{
#ifdef RACE_CFU_HANDLER_ENABLE
    if (cfu_is_running() ||
        race_cfu_handler_wait_rsp())
    {
        return RACE_ERRCODE_NOT_ALLOWED;
    }
#endif

    if (g_race_cfu_cntex_ptr && RACE_CFU_UPDATE_MODE_MAX != update_mode)
    {
        g_race_cfu_cntex_ptr->update_mode = update_mode;
        return RACE_ERRCODE_SUCCESS;
    }

    return RACE_ERRCODE_WRONG_STATE;
}


RACE_ERRCODE race_cfu_race_event_processer(int32_t register_id, race_event_type_enum event_type, void *param, void *user_data)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

    if (!g_race_cfu_cntex_ptr ||
        register_id != g_race_cfu_cntex_ptr->register_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

#ifdef RACE_CFU_HANDLER_ENABLE
    ret = race_cfu_handler_race_event_processer(register_id, event_type, param, user_data);
#endif

    return ret;
}


void race_cfu_msg_processer(race_general_msg_t *msg)
{
    if (msg && g_race_cfu_cntex_ptr)
    {
        switch (msg->dev_t)
        {
            case RACE_CFU_MSG_ID_SEND_DATA:
            {
#ifdef RACE_CFU_BUILDER_ENABLE
                race_cfu_builder_send_data_msg_processer((race_cfu_packet_struct *)msg->msg_data);
#elif defined(RACE_CFU_HANDLER_ENABLE)
                race_cfu_handler_send_data_msg_processer((race_cfu_packet_struct *)msg->msg_data);
#endif
                break;
            }

            default:
                break;
        }
    }

    if (msg && msg->msg_data)
    {
        race_mem_free(msg->msg_data);
    }
}


RACE_ERRCODE race_cfu_send(race_cfu_packet_struct *packet)
{
    if (!packet)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_general_msg_t msg;
    U8 *msg_data = NULL;

    U32 packet_len = sizeof(race_cfu_packet_struct) + packet->data_len;

    msg_data = race_mem_alloc(packet_len);
    if (!msg_data)
    {
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    memcpy(msg_data, packet, packet_len);

    msg.msg_id = MSG_ID_RACE_LOCAL_CFU_IND;
    msg.dev_t = RACE_CFU_MSG_ID_SEND_DATA;
    msg.msg_data = msg_data;

    ret = race_send_msg(&msg);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        race_mem_free(msg_data);
    }

    return ret;
}


RACE_ERRCODE race_cfu_init(race_cfu_udpate_mode_enum update_mode, race_cfu_receive_callback recv_cb)
{
    if (g_race_cfu_cntex_ptr)
    {
        return RACE_ERRCODE_SUCCESS;
    }

    if (RACE_CFU_UPDATE_MODE_MAX == update_mode ||
#ifndef RACE_LPCOMM_ENABLE
        RACE_CFU_UPDATE_MODE_DUAL_DEVICE == update_mode ||
#endif
        !recv_cb)
    {
        RACE_LOG_MSGID_W("[CFU][RACE] invalid param. update_mode:%d recv_cb:%x", 2, update_mode, recv_cb);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    g_race_cfu_cntex_ptr = &g_race_cfu_cntex;
    g_race_cfu_cntex_ptr->update_mode = update_mode;
    g_race_cfu_cntex_ptr->recv_cb = recv_cb;

    race_event_register(&(g_race_cfu_cntex_ptr->register_id), race_cfu_race_event_processer, NULL);

#ifdef RACE_CFU_HANDLER_ENABLE
    race_cfu_handler_init();
#endif

    return RACE_ERRCODE_SUCCESS;
}
#endif

