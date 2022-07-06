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
#include "race_noti.h"


RACE_ERRCODE race_send_delay_noti_msg(void *noti, uint8_t channel_id)
{
    /* Send MSG to send noti out. */
    race_general_msg_t msg_queue_item = {0};
    race_noti_delay_msg_data_struct *msg_data = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    if (!noti)
    {
        return RACE_ERRCODE_FAIL;
    }

    msg_data = race_mem_alloc(sizeof(race_noti_delay_msg_data_struct));
    if (!msg_data)
    {
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }
    msg_data->noti = noti;
    msg_data->channel_id = channel_id;

    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_DELAY_NOTI_IND;
    msg_queue_item.msg_data = (uint8_t*)msg_data;
    ret = race_send_msg(&msg_queue_item);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        race_mem_free((void *)msg_data);
    }

    return ret;
}


/* If the return value is RACE_ERRCODE_SUCCESS, noti will be freed after sent automatically.
  * In other cases, noti should be freed manually.
  */
RACE_ERRCODE race_noti_send(void *noti,
                                uint8_t channel_id,
                                bool noti_delay)
{
    int32_t ret = RACE_ERRCODE_FAIL;

    if (!noti)
    {
        return RACE_ERRCODE_FAIL;
    }

    if (!noti_delay)
    {
        /* noti will be freed by race_flush_packet() if RACE_STATUS_OK is returned. */
        ret = race_flush_packet((uint8_t *)noti, channel_id);
        ret = RACE_STATUS_OK == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }
    else
    {
        /* noti will be freed by race_noti_delay_msg_process() if RACE_ERRCODE_SUCCESS is returned. */
        ret = race_send_delay_noti_msg(noti, channel_id);
    }

    RACE_LOG_MSGID_I("ret:%x",1, ret);
    return ret;
}


void race_noti_delay_msg_process(race_general_msg_t *msg)
{
    race_noti_delay_msg_data_struct *msg_data = NULL;

    if (!msg || !msg->msg_data)
    {
        return;
    }

    msg_data = (race_noti_delay_msg_data_struct *)msg->msg_data;

    race_flush_packet((uint8_t *)msg_data->noti, msg_data->channel_id);

    race_mem_free(msg_data);
}

