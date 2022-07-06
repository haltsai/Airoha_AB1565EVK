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
#include "hal_gpt.h"
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
#include "battery_management_core.h"
#endif
#ifdef RACE_FOTA_CMD_ENABLE
#include "fota_flash.h"
#endif
#include "race_util.h"
#include "race_xport.h"
#include "bt_sink_srv_ami.h"

RACE_ERRCODE race_list_insert(race_template_list_struct **list, race_template_list_struct *list_node)
{
    race_template_list_struct *list_node_tmp = NULL;

    //RACE_LOG_MSGID_I("race_list_insert", 0);

    if (!list || !list_node || list_node->next)
    {
        RACE_LOG_MSGID_I("Invalid param list:%x, list_node:%x",2, list, list_node);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (!(*list))
    {
        *list = list_node;
    }
    else
    {
        list_node_tmp = *list;
        while (list_node_tmp->next)
        {
            list_node_tmp = list_node_tmp->next;
        }

        list_node_tmp->next = list_node;
    }

    return RACE_ERRCODE_SUCCESS;
}


/* The node will not be freed in this API. */
RACE_ERRCODE race_list_remove(race_template_list_struct **list, race_template_list_struct *list_node)
{
    race_template_list_struct *list_node_tmp = NULL;


    if (!list || !(*list) || !list_node)
    {
        RACE_LOG_MSGID_I("Invalid param list:%x, list_node:%x",2, list, list_node);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (*list == list_node)
    {
        *list = (*list)->next;
    }
    else
    {
        list_node_tmp = *list;
        while (list_node_tmp && list_node_tmp->next != list_node)
        {
            list_node_tmp = list_node_tmp->next;
        }

        if (list_node_tmp)
        {
            list_node_tmp->next = list_node->next;
        }
        else
        {
            RACE_LOG_MSGID_I("Remove fail: Not Found", 0);
            return RACE_ERRCODE_FAIL;
        }
    }

    list_node->next = NULL;

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_list_destory(race_template_list_struct **list,
                                   race_list_free_func free_func)
{
    race_template_list_struct *list_node = NULL, *list_node_next = NULL;

    if (!list || !free_func)
    {
        RACE_LOG_MSGID_I("Invalid Param list:%x free_func:%x",2, list, free_func);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    list_node = *list;
    while (list_node)
    {
        list_node_next = list_node->next;

        /* *list will be set to NULL at last by race_list_remove. */
        race_list_remove(list, list_node);
        free_func(list_node);

        list_node = list_node_next;
    }

    return RACE_ERRCODE_SUCCESS;
}


SemaphoreHandle_t race_mutex_create(void)
{
    return xSemaphoreCreateMutex();
}


void race_mutex_free(SemaphoreHandle_t mutex)
{
    if (mutex)
    {
        vSemaphoreDelete(mutex);
    }
}


RACE_ERRCODE race_mutex_lock(SemaphoreHandle_t mutex)
{
    if (!mutex)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (xSemaphoreTake(mutex, portMAX_DELAY) != pdTRUE)
    {
        return RACE_ERRCODE_FAIL;
    }

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_mutex_unlock(SemaphoreHandle_t mutex)
{
    if (!mutex)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (xSemaphoreGive(mutex) != pdTRUE)
    {
        return RACE_ERRCODE_FAIL;
    }

    return RACE_ERRCODE_SUCCESS;
}


/* 0 is the invalid process_id. */
uint16_t race_gen_process_id(void)
{
    static uint16_t process_id = 0;

    process_id++;
    process_id %= 0xFFFF;
    if (0 == process_id)
    {
        process_id++;
    }

    return process_id;
}


RACE_ERRCODE race_query_partition_info(uint8_t *storage_type,
                                            uint32_t *address,
                                            uint32_t *length,
                                            uint8_t partition_id)
{
    int32_t ret = RACE_ERRCODE_FAIL;

    if (!storage_type || !address || !length)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    switch (partition_id)
    {
#ifdef RACE_FOTA_CMD_ENABLE
        case RACE_STORAGE_PARTITION_ID_FOTA:
        {
            ret = fota_flash_get_fota_partition_info(storage_type,
                                                     address,
                                                     length);

            if (FOTA_ERRCODE_SUCCESS == ret)
            {
                /* Reduce the last 4K which is used to store history states or upgrade info. */
                (*length) -= 0x1000;
                ret = RACE_ERRCODE_SUCCESS;
            }
            else
            {
                ret = RACE_ERRCODE_FAIL;
            }
            break;
        }
#endif /* RACE_FOTA_CMD_ENABLE */

        case RACE_STORAGE_PARTITION_ID_FS:
        {
            /* 1530 feature only which merges filesystem sector and fota sector as a new extended fota sector. */
            ret = RACE_ERRCODE_NOT_SUPPORT;
            break;
        }

        default:
        {
            ret = RACE_ERRCODE_NOT_SUPPORT;
            break;
        }
    }

    return ret;
}


uint32_t race_get_curr_time_in_ms(void)
{
    uint32_t count = 0;
    uint64_t count64 = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    count64 = ((uint64_t)count) * 1000 / 32768;
    return (uint32_t)count64;
}


uint32_t race_get_duration_in_ms(uint32_t start_time, uint32_t end_time)
{
    uint32_t duration = 0;

    /* Normally, it is rare or impossble for race cmd case that the duration takes just a round. On the contrary, the timer check may
     * be executed just after the timer is started. And in such case, the start_timer and end_timer may be equal. Therefore, processing
     * the end_time == start_timer in the if block.
     */
    if (end_time >= start_time)
    {
        duration = end_time - start_time;
    }
    else
    {
        duration = end_time + ((uint64_t)0xFFFFFFFF + 1) * 1000 / 32768 - start_time;
    }

    return duration;
}


race_device_role_enum race_get_device_role(void)
{
    audio_channel_t role = ami_get_audio_channel();
    if (role != AUDIO_CHANNEL_NONE)
    {
        RACE_LOG_MSGID_I("device_role:%d",1, role);
        return AUDIO_CHANNEL_L == role ? RACE_DEVICE_ROLE_LEFT : RACE_DEVICE_ROLE_RIGHT;
    }

#ifdef RACE_AWS_ENABLE
    return RACE_DEVICE_ROLE_MAX;
#else
    return RACE_DEVICE_ROLE_RIGHT;
#endif
}


uint8_t race_get_battery_level(void)
{
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
    uint8_t battery_level = battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE_IN_PERCENT);

    RACE_LOG_MSGID_I("battery_level:%d",1, battery_level);
#ifdef RACE_BT_BATTERY_FAKE_VALUE
    battery_level = 70;
#endif

    return battery_level;
#else
    return 0;
#endif
}


void *race_pointer_cnv_send_pkt_to_pkt(race_send_pkt_t *pPacket)
{
    void *pNoti = NULL;

    if (pPacket)
    {
        pNoti = pPacket->race_data.payload;
    }

    RACE_LOG_MSGID_I("race_pointer_cnv_send_pkt_to_pkt, send_pkt[0x%x], noti[0x%x]", 2, pPacket, pPacket->race_data.payload);
    return pNoti;
}


race_send_pkt_t *race_pointer_cnv_pkt_to_send_pkt(void *pNoti)
{
    race_send_pkt_t *send_pkt = NULL;

    if (pNoti)
    {
        race_pkt_t* pkt = NULL;

        /* Convert payload pointer to the pointer points to the begining of the whole package. */
        pkt = CONTAINER_OF(pNoti, race_pkt_t, payload);
        send_pkt = CONTAINER_OF(pkt, race_send_pkt_t, race_data);
    }

    //RACE_LOG_MSGID_I("race_pointer_cnv_pkt_to_send_pkt, send_pkt[0x%x], pNoti[0x%x]", 2, send_pkt, pNoti);

    return send_pkt;
}


race_recipient_type_enum race_recipient_type_convt(uint8_t recipient)
{
    RACE_LOG_MSGID_I("race_recipient_type_convt, recipient[0x%x]", 1, recipient);

    if ((RACE_CMD_RECIPIENT_AGENT & recipient) &&
        (RACE_CMD_RECIPIENT_PARTNER & recipient))
    {
        RACE_LOG_MSGID_I("recipient_type:%d",1, RACE_RECIPIENT_TYPE_AGENT_PARTNER);
        return RACE_RECIPIENT_TYPE_AGENT_PARTNER;
    }
    else if (RACE_CMD_RECIPIENT_AGENT & recipient)
    {
        RACE_LOG_MSGID_I("recipient_type:%d",1, RACE_RECIPIENT_TYPE_AGENT_ONLY);
        return RACE_RECIPIENT_TYPE_AGENT_ONLY;
    }
    else if (RACE_CMD_RECIPIENT_PARTNER & recipient)
    {
        RACE_LOG_MSGID_I("recipient_type:%d",1, RACE_RECIPIENT_TYPE_PARTNER_ONLY);
        return RACE_RECIPIENT_TYPE_PARTNER_ONLY;
    }

    RACE_LOG_MSGID_I("recipient_type:%d",1, RACE_RECIPIENT_TYPE_NONE);
    return RACE_RECIPIENT_TYPE_NONE;
}


RACE_ERRCODE race_recipient_param_parse(uint8_t recipient_count,
                                                uint8_t *recipient_param,
                                                uint8_t recipient_param_size,
                                                uint8_t **agent_recipient_param,
                                                uint8_t **partner_recipient_param)
{
    race_recipient_param_general_struct *recipient_param_tmp = NULL;

    if (0 == recipient_count || 2 < recipient_count ||
        !recipient_param || !recipient_param_size ||
        !agent_recipient_param || *agent_recipient_param ||
        !partner_recipient_param || *partner_recipient_param)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    do
    {
        recipient_param_tmp = (race_recipient_param_general_struct *)recipient_param;
        if (RACE_RECIPIENT_PARAM_RECIPIENT_AGENT == recipient_param_tmp->recipient)
        {
            if (*agent_recipient_param)
            {
                return RACE_ERRCODE_PARAMETER_ERROR;
            }
            *agent_recipient_param = (uint8_t *)recipient_param_tmp;
        }
        else if (RACE_RECIPIENT_PARAM_RECIPIENT_PARTNER == recipient_param_tmp->recipient)
        {
            if (*partner_recipient_param)
            {
                return RACE_ERRCODE_PARAMETER_ERROR;
            }
            *partner_recipient_param = (uint8_t *)recipient_param_tmp;
        }
        else
        {
            return RACE_ERRCODE_NOT_SUPPORT;
        }

        recipient_param += recipient_param_size;
    }while (--recipient_count);

    if ((!(*agent_recipient_param)) && (!(*partner_recipient_param)))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    return RACE_ERRCODE_SUCCESS;
}

