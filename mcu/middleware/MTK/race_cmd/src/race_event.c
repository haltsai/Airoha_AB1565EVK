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
#include "race_xport.h"
#include "race_fota_util.h"
#include "race_event_internal.h"
#include "race_lpcomm_util.h"


#ifdef MTK_RACE_EVENT_ID_ENABLE
#define RACE_EVENT_BT_GET_IDX_BY_REGISTER_ID(reg_id)  ((reg_id) - RACE_EVENT_REGISTER_ID_BASE)
#define RACE_EVENT_BT_GET_REGISTER_ID_BY_IDX(id)      ((id) + RACE_EVENT_REGISTER_ID_BASE)
#endif


typedef struct
{
    race_event_callback callback;
    void *user_data;
    bool is_used;
}race_event_register_info_struct;


race_event_register_info_struct *g_race_event_register_info_ptr;


void race_event_init(void)
{
    if (!g_race_event_register_info_ptr)
    {
        g_race_event_register_info_ptr = race_mem_alloc(RACE_EVENT_REGISTER_MAX_NUM * sizeof(race_event_register_info_struct));
    }

    if (g_race_event_register_info_ptr)
    {
        memset(g_race_event_register_info_ptr, 0, RACE_EVENT_REGISTER_MAX_NUM * sizeof(race_event_register_info_struct));
    }
}


#ifdef MTK_RACE_EVENT_ID_ENABLE
race_event_register_info_struct *race_event_register_info_find_by_callback(int32_t *register_id, race_event_callback callback)
#else
race_event_register_info_struct *race_event_register_info_find(race_event_callback callback)
#endif
{
    int32_t i = 0;

#ifdef MTK_RACE_EVENT_ID_ENABLE
    if (!register_id)
    {
        return NULL;
    }

    *register_id = RACE_EVENT_INVALID_REGISTER_ID;
#endif

    if (!callback || !g_race_event_register_info_ptr)
    {
        return NULL;
    }

    for(i = 0; i < RACE_EVENT_REGISTER_MAX_NUM; i++)
    {
        if (callback == g_race_event_register_info_ptr[i].callback &&
            g_race_event_register_info_ptr[i].is_used)
        {
#ifdef MTK_RACE_EVENT_ID_ENABLE
            *register_id = RACE_EVENT_BT_GET_REGISTER_ID_BY_IDX(i);
#endif
            return &g_race_event_register_info_ptr[i];
        }
    }

    return NULL;
}


#ifdef MTK_RACE_EVENT_ID_ENABLE
race_event_register_info_struct *race_event_register_info_find_by_id(int32_t register_id)
{
    int i = RACE_EVENT_BT_GET_IDX_BY_REGISTER_ID(register_id);

    if (g_race_event_register_info_ptr &&
        RACE_EVENT_REGISTER_ID_MIN <= register_id &&
        RACE_EVENT_REGISTER_ID_MAX >= register_id &&
        g_race_event_register_info_ptr[i].is_used)
    {
        return &g_race_event_register_info_ptr[i];
    }

    return NULL;
}
#endif


#ifdef MTK_RACE_EVENT_ID_ENABLE
race_event_register_info_struct *race_event_register_info_find_empty(int32_t *register_id)
#else
race_event_register_info_struct *race_event_register_info_find_empty(void)
#endif
{
    int32_t i = 0;

#ifdef MTK_RACE_EVENT_ID_ENABLE
    if (!register_id)
    {
        return NULL;
    }

    *register_id = RACE_EVENT_INVALID_REGISTER_ID;
#endif

    if (!g_race_event_register_info_ptr)
    {
        return NULL;
    }

    for(i = 0; i < RACE_EVENT_REGISTER_MAX_NUM; i++)
    {
        if (!g_race_event_register_info_ptr[i].callback &&
            g_race_event_register_info_ptr[i].is_used)
        {
            g_race_event_register_info_ptr[i].is_used = FALSE;
        }

        if (!g_race_event_register_info_ptr[i].is_used)
        {
            memset(&g_race_event_register_info_ptr[i], 0, sizeof(race_event_register_info_struct));
#ifdef MTK_RACE_EVENT_ID_ENABLE
            *register_id = RACE_EVENT_BT_GET_REGISTER_ID_BY_IDX(i);
#endif
            return &g_race_event_register_info_ptr[i];
        }
    }

    return NULL;
}


#ifdef MTK_RACE_EVENT_ID_ENABLE
RACE_ERRCODE race_event_register(int32_t *register_id, race_event_callback callback, void *user_data)
{
    race_event_register_info_struct *reg_info = race_event_register_info_find_by_callback(register_id, callback);

    if (!register_id || !callback)
    {
        RACE_LOG_MSGID_E("Register failed. ret:%d register_id:%x callback:%x",3,
                   RACE_ERRCODE_PARAMETER_ERROR,
                   register_id,
                   callback);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (!g_race_event_register_info_ptr)
    {
        RACE_LOG_MSGID_E("Register failed. ret:%d",1, RACE_ERRCODE_WRONG_STATE);
        return RACE_ERRCODE_WRONG_STATE;
    }

    if (reg_info)
    {
        if (reg_info->user_data != user_data)
        {
            RACE_LOG_MSGID_E("Register failed. register_id:%d ret:%d",2, *register_id, RACE_ERRCODE_CONFLICT);
            return RACE_ERRCODE_CONFLICT;
        }

        RACE_LOG_MSGID_W("Register succeed(duplicate). register_id:%d",1, *register_id);
        return RACE_ERRCODE_SUCCESS;
    }

    reg_info = race_event_register_info_find_empty(register_id);
    if (reg_info)
    {
        reg_info->is_used = TRUE;
        reg_info->callback = callback;
        reg_info->user_data = user_data;

        RACE_LOG_MSGID_I("Register succeed(new), register_id:%d",1, *register_id);
        return RACE_ERRCODE_SUCCESS;
    }

    RACE_LOG_MSGID_E("Register failed. ret:%d",1, RACE_ERRCODE_NOT_ENOUGH_MEMORY);
    return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
}
#else
RACE_ERRCODE race_event_register(race_event_callback callback, void *user_data)
{
    race_event_register_info_struct *reg_info = race_event_register_info_find(callback);

    if (!callback)
    {
        RACE_LOG_MSGID_E("Rgeister failed. ret:%d",1, RACE_ERRCODE_PARAMETER_ERROR);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (reg_info)
    {
        if (reg_info->user_data != user_data)
        {
            RACE_LOG_MSGID_E("Rgeister failed. ret:%d",1, RACE_ERRCODE_CONFLICT);
            return RACE_ERRCODE_CONFLICT;
        }

        RACE_LOG_MSGID_I("Rgeister succeed", 0);
        return RACE_ERRCODE_SUCCESS;
    }

    reg_info = race_event_register_info_find_empty();
    if (reg_info)
    {
        reg_info->is_used = TRUE;
        reg_info->callback = callback;
        reg_info->user_data = user_data;

        RACE_LOG_MSGID_I("Rgeister succeed", 0);

        return RACE_ERRCODE_SUCCESS;
    }

    RACE_LOG_MSGID_E("Rgeister failed. ret:%d",1, RACE_ERRCODE_NOT_ENOUGH_MEMORY);
    return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
}
#endif


void race_event_register_notify(race_event_type_enum event_type, void *param)
{
    int32_t i = 0;
#ifdef MTK_RACE_EVENT_ID_ENABLE
    int32_t reg_id = RACE_EVENT_INVALID_REGISTER_ID;
#endif

    if (!g_race_event_register_info_ptr)
    {
        RACE_LOG_MSGID_E("Register not initialized.", 0);
        return;
    }

    for(i = 0; i < RACE_EVENT_REGISTER_MAX_NUM; i++)
    {
        if (g_race_event_register_info_ptr[i].callback &&
            g_race_event_register_info_ptr[i].is_used)
        {
#ifdef MTK_RACE_EVENT_ID_ENABLE
            reg_id = RACE_EVENT_BT_GET_REGISTER_ID_BY_IDX(i);
            /* ret will be detected to be not used if return the value to ret here too. */
            g_race_event_register_info_ptr[i].callback(reg_id, event_type, param, g_race_event_register_info_ptr[i].user_data);
#else
            RACE_LOG_MSGID_I("Rgeister notify i:%d.",1, i);
            g_race_event_register_info_ptr[i].callback(event_type, param, g_race_event_register_info_ptr[i].user_data);
#endif
        }
    }
}


RACE_ERRCODE race_send_event_notify_msg(race_event_type_enum event_type, void *param)
{
    race_general_msg_t msg_queue_item = {0};
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("event_type:%d, param:%x",2, event_type, param);

    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_EVENT_NOTIFY_REQ;
    msg_queue_item.dev_t = (serial_port_dev_t)event_type;
    msg_queue_item.msg_data = (uint8_t *)param;
    ret = race_send_msg(&msg_queue_item);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        RACE_LOG_MSGID_E("Failed to send EVENT_NOTIFY_REQ ret:%x.",1, ret);
    }
    return ret;
}


#ifdef RACE_FOTA_CMD_ENABLE
RACE_ERRCODE race_event_send_fota_start_event(bool is_dual_fota,
                                                       bool is_active_fota)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_event_start_param_struct *param = (race_event_start_param_struct *)race_mem_alloc(sizeof(race_event_start_param_struct));

    if (param)
    {
        param->is_dual_fota = is_dual_fota;
        param->is_active_fota = is_active_fota;
    }

    /* Even param is NULL, send the event also. */
    ret = race_send_event_notify_msg(RACE_EVENT_TYPE_FOTA_START, param);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        if (param)
        {
            race_mem_free(param);
        }
    }

    return ret;
}


RACE_ERRCODE race_event_send_fota_cancelling_event(race_fota_stop_originator_enum originator,
                                                             race_fota_stop_reason_enum reason)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_event_cancelling_param_struct *param = (race_event_cancelling_param_struct *)race_mem_alloc(sizeof(race_event_cancelling_param_struct));

    if (param)
    {
        param->originator = originator;
        param->reason = reason;

        ret = race_send_event_notify_msg(RACE_EVENT_TYPE_FOTA_CANCELLING, param);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            race_mem_free(param);
        }
    }

    return ret;
}


RACE_ERRCODE race_event_send_fota_cancel_event(bool result,
                                                         race_fota_stop_originator_enum originator,
                                                         race_fota_stop_reason_enum reason)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_event_cancel_param_struct *param = (race_event_cancel_param_struct *)race_mem_alloc(sizeof(race_event_cancel_param_struct));

    if (param)
    {
        param->result = result;
        param->originator = originator;
        param->reason = reason;
    }

    /* Even param is NULL, send the event also. */
    ret = race_send_event_notify_msg(RACE_EVENT_TYPE_FOTA_CANCEL, param);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        if (param)
        {
            race_mem_free(param);
        }
    }

    return ret;
}


RACE_ERRCODE race_event_send_fota_need_reboot_event(uint8_t *address, uint32_t address_length)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    uint8_t channel_id = 0xFF;
    race_event_need_reboot_param_struct *param = (race_event_need_reboot_param_struct *)race_mem_alloc(sizeof(race_event_need_reboot_param_struct));

    if (param)
    {
        memset(param, 0, sizeof(race_event_need_reboot_param_struct));

        if (address && address_length)
        {
            param->address_length = RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH > address_length ? address_length : RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH;
            memcpy(param->address, address, param->address_length);
        }
        else
        {
            ret = race_fota_channel_id_get(&channel_id);
            if (RACE_ERRCODE_SUCCESS == ret)
            {
                address = (uint8_t *)race_get_bt_connection_addr(channel_id);
                if (address)
                {
                    param->address_length = RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH > sizeof(bt_bd_addr_t) ? sizeof(bt_bd_addr_t) : RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH;
                    memcpy(param->address, address, param->address_length);
                }
            }
        }
    }

    /* Even param is NULL, send the event also. */
    ret = race_send_event_notify_msg(RACE_EVENT_TYPE_FOTA_NEED_REBOOT, param);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        if (param)
        {
            race_mem_free(param);
        }
    }

    return ret;
}
#endif


#ifdef RACE_BT_CMD_ENABLE
RACE_ERRCODE race_event_send_bt_rho_result_event(bool result)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_event_rho_result_param_struct *param = (race_event_rho_result_param_struct *)race_mem_alloc(sizeof(race_event_rho_result_param_struct));

    if (param)
    {
        param->result = result;
    }

    /* Even param is NULL, send the event also. */
    ret = race_send_event_notify_msg(RACE_EVENT_TYPE_BT_RHO_RESULT, param);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        if (param)
        {
            race_mem_free(param);
        }
    }

    return ret;
}

#endif


#ifdef RACE_FIND_ME_ENABLE
RACE_ERRCODE race_event_send_find_me_event(uint8_t is_blink, uint8_t is_tone)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
#if 0
    race_event_find_me_param_struvt *param = {0};
    param.is_blink = is_blink;
    param.is_tone = is_tone;
#endif
    race_event_find_me_param_struvt *param = (race_event_find_me_param_struvt *)race_mem_alloc(sizeof(race_event_find_me_param_struvt));
    if (param == NULL) {
        return ret;
    }

    param->is_blink = is_blink;
    param->is_tone = is_tone;
    RACE_LOG_MSGID_I("[FIND_ME]send_find_me_event:blink:%x, is_tone:%x, param_b: %x, param_t : %x",4, is_blink, is_tone, param->is_blink, param->is_tone);

    ret = race_send_event_notify_msg(RACE_EVENT_TYPE_FIND_ME, param);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        if (param)
        {
            race_mem_free(param);
        }
    }
    return ret;
}
#endif


void race_event_notify_msg_process(race_general_msg_t *msg)
{
    RACE_LOG_MSGID_I("msg:%x.",1, msg);

    if (msg)
    {
        race_event_register_notify((race_event_type_enum)msg->dev_t, (void *)msg->msg_data);
        if (msg->msg_data)
        {
            race_mem_free(msg->msg_data);
            msg->msg_data = NULL;
        }
    }
}

